# Add high-level persistence with SaveServer and @persistent annotation

## 1. Describe the project you are working on

I am working on the Godot Engine core, specifically aiming to enable native support for complex open-world RPGs and simulation games. These projects require managing thousands of persistent objects (entities, inventory items, world state) that change dynamically. To truly support this scale of development, the engine needs a reliable, built-in way to stream this data to disk without stalling the main thread, while maintaining robust versioning and data integrity.

## 2. Describe the problem or limitation you are having in your project

Saving game state in Godot is currently a tedious manual process. There is no standard "Godot way" to save data. Developers are forced to:

1. Manually build dictionaries for every saveable object.
2. Implement their own file I/O handling (often blocking the main thread).
3. Reinvent systems for encryption, compression, and backups.
4. Manually handle version migration when save formats change.

Using `ResourceSaver` to save whole packed scenes is often overkill and inflexible for runtime state, while `FileAccess` is too low-level. This leads to fragile, boilerplate-heavy code that is prone to bugs and data corruption. New developers frequently struggle with this, implementing unsafe systems that break easily.

## 3. Describe the feature / enhancement and how it helps to overcome the problem or limitation

I propose adding a **High-Level Persistence System** consisting of the `SaveServer` singleton, the `@persistent` annotation, and the `_save_persistence` / `_load_persistence` virtual methods.

**Important Distinction:** `SaveServer` is not a rigid "save game" feature. It is an **infrastructure layer**. It abstracts away the complex "plumbing" of persistence (threaded I/O, encryption, atomic writes, compression) so that developers can build their own specific save systems on top of it.

While `@persistent` handles simple property serialization, the system evolves traditional persistence into a **"Git for Gameplay"** model. Instead of saving full world snapshots (which cause "Save Stutter"), we utilize **State Increments**. This allows the engine to track only modified objects ("Dirty Tracking") and save fractional updates, reducing disk I/O by up to 95%.

## 4. Describe how your proposal will work, with code, pseudo-code, mock-ups, and/or diagrams

The core of the proposal is the **`@persistent`** annotation system, which works on any `Object` (Nodes, Resources, RefCounted).

### The `@persistent` Annotation

```gdscript
extends CharacterBody3D

# --- Basic Usage ---
# Automatically saved to the snapshot
@persistent var player_name: String = "Hero"

# --- Editor Integration (@export) ---
# Designers set initial value in editor. SaveServer saves runtime changes.
@export @persistent var max_health: int = 100
@export @persistent var current_health: int = 100

# --- Tags (Filtered Saves) ---
# Adds "inventory" tag. Allows saving ONLY this subset later.
@persistent("inventory") var items: Array = []

# --- Groups (Syntactic Sugar) ---
# Clean way to apply "progression" tag to multiple variables
@persistent_group("progression")
@persistent var level: int = 1
@persistent var experience: int = 0
```

### The `SaveServer` Singleton

A new singleton handles the heavy lifting:

```gdscript
# Save the entire game state starting from the root node
# Async, encrypted, compressed, and atomic (safe against crashes)
await SaveServer.save_snapshot(get_tree().root, "save_slot_1")

# Load it back
SaveServer.load_snapshot(get_tree().root, "save_slot_1")

# Save only specific tags (e.g., just the inventory)
SaveServer.save_snapshot(get_tree().root, "slot_1", true, ["inventory"])

# --- INCREMENTAL SAVE (The Delta Model) ---
# Saves ONLY dirty objects (modified since load). Git-style "Diff".
# Effectively eliminates save stutter in massive games.
SaveServer.save_incremental(get_tree().root, "save_slot_1")
```

### The `Snapshot` Resource

All saved state is encapsulated in a new `Resource` type called `Snapshot`. It separates heavy game data from lightweight metadata:

- **`snapshot`** (Dictionary): The full persistence data.
- **`metadata`** (Dictionary): UI-facing data (playtime, level name, etc.).
- **`thumbnail`** (Texture2D): Optional screenshot.
- **`checksum`** (String): SHA-256 hash for integrity.

This design allows loading **only** metadata and thumbnails for instant "Load Game" menus, without needing to deserialize gigabytes of game state.

### The `_save_persistence` and `_load_persistence` Virtual Functions

These functions are built into the `Object` class and serve as native lifecycle hooks, just like `_ready()`, `_process()`, or `_physics_process()`. The "Persistence Twins" are executed by the system precisely when a save or load event occurs.

While `@persistent` is powerful for individual properties, it is not designed to track large collections of dynamic data, such as 50 enemies spawned at runtime. In these cases, the "Manager" object (e.g., the Spawner) takes responsibility.

#### Pattern 1: Manual State Gathering (Full Control)

In this pattern, the manager is responsible for both the hierarchy AND the data loop. It does NOT rely on automatic ID reconciliation, acting as a complete manual fallback.

```gdscript
extends Node2D

var enemy_scene = preload("res://enemy.tscn")

func _save_persistence(state: Dictionary, _tags: Array[StringName]):
    var enemy_data = []
    for enemy in get_children():
        enemy_data.append({
            "pos": enemy.position,
            "hp": enemy.hp,
            "type": enemy.type
        })
    state["spawned_enemies"] = enemy_data

func _load_persistence(state: Dictionary):
    # Clear existing and recreate based on manual dictionary data
    for child in get_children(): child.queue_free()
    for data in state.get("spawned_enemies", []):
        var e = enemy_scene.instantiate()
        e.position = data.pos
        e.hp = data.hp
        e.type = data.type
        add_child(e)
```

#### Pattern 2: Reconstructive ID Model (Engine Assisted)

For scenarios requiring high stability with complex objects, `SaveServer` provides a **Reconstructive ID Model**. The manager only handles the spawning and re-linking of the `persistence_id`, letting the engine handle the restoration of the node's internal properties automatically.

```gdscript
# Re-instantiate using engine-managed IDs
func _load_persistence(state: Dictionary):
    for child in get_children(): child.queue_free()

    for data in state.get("spawned_enemies", []):
        var e = enemy_scene.instantiate()
        # RE-APPLY the ID so the engine can map all @persistent properties
        e.persistence_id = data.id
        add_child(e)
        # SaveServer now proceeds to inject 'e.hp', 'e.ammo', etc., automatically
```

Furthermore, the **Global ID Registry** ensures that nodes are identified by their `persistence_id` rather than hierarchical `NodePath`. This resolves the classic problem of save files breaking when nodes are renamed or moved within the tree during development or runtime.

This ensures that `SaveServer` remains a flexible **persistence system**: simple state is automated, but complex game logic remains fully under the developer's control.

### Integration with Existing Systems

- **Text Saves (`.tres`)**: Uses `ResourceSaver` internally, so save files are debuggable in the editor.
- **Binary Saves (`.data`)**: Uses `FileAccess` for AES encryption and ZSTD compression, but keeps the internal structure compatible with Godot's variant system.

## 5. If this enhancement will not be used often, can it be worked around with a few lines of script?

It can be worked around, but not with "a few lines". It requires writing a full serialization framework, thread management for async I/O to avoid lag spikes, and robust error handling. Almost every serious project ends up writing their own version of this, leading to massive community duplication of effort and often sub-par, buggy implementations.

## 6. Is there a reason why this should be core and not an add-on in the asset library?

Yes.

1. **Performance**: Iterating over the entire scene tree to collect persistent properties needs to be fast. Doing this in C++ with direct access to object internals (`PROPERTY_USAGE_PERSISTENCE` flag) is significantly faster than a GDScript addon iterating via reflection.
2. **Reliability**: Save systems are critical infrastructure. They require deep integration with `ResourceLoader`/`Saver` to handle sub-resources, dependencies, and atomic writes correctly.
3. **Standardization**: This lowers the barrier to entry for beginners significantly. Just as Godot provides `HighLevelMultiplayer`, providing `HighLevelPersistence` makes the engine much more complete for actual game production.
