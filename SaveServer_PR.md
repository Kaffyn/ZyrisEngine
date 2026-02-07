# Implement High-Level Persistence: SaveServer and @persistent annotation

**Related Issue:** <https://github.com/godotengine/godot-proposals/issues/14140>

## Summary

This PR introduces **SaveServer**, a standardized persistence system for Godot. Rather than enforcing a specific way to save games, it provides the robust infrastructure (singleton, threading, encryption, integrity) that developers currently have to build from scratch. It exposes both a rapid-prototyping API (`@persistent`) and low-level hooks (`_save_persistence`/`_load_persistence`) to allow users to architect their own save systems efficiently.

## Visuals

<!-- Add screenshots or videos demonstrating the feature in action here. -->
<!-- Example: A GIF showing state restoration after closing/reopening the game. -->

[Insert Screenshot/Video Here]

## Test Project

To facilitate testing and review, I have prepared a demo project (**NeonShooter**) that fully utilizes the `SaveServer` features.

- **Repository:** [Link to NeonShooter Repository]
- **What to test:**
  - Play the game, collect items, and save.
  - Close and reopen to verify state restoration.
  - Check `user://saves/` for generated files.

## The Problem

Saving game state in Godot right now is... tedious. Even for something simple like player health and position, you end up writing a lot of boilerplate. You manually build dictionaries, serialize to JSON or binary, handle file I/O, and if you want to avoid frame drops during autosave, you need to implement your own threading. Want encryption? Checksums? Backup files in case of corruption? That's all on you.

I've seen this pattern repeated in almost every Godot project I've worked on or reviewed. Everyone writes their own `SaveManager` class with `save_game()` and `load_game()` methods. It works, but it's repetitive and error-prone. New developers especially struggle with this - it's one of those "figure it out yourself" areas where the engine doesn't provide much guidance.

Using `ResourceSaver` to save entire scenes is overkill for most cases - you don't want to save the entire scene structure, just the dynamic state. But manually tracking which properties need saving is annoying and easy to forget when you add new features.

## Proposal

The core idea is simple: let developers mark properties as persistent, and the engine handles the rest.

### 1. The `@persistent` Annotation System

The core idea is simple: mark properties for automatic saving. The system is flexible, supporting tags, groups, and native integration with `@export` on any `Object` (Nodes or Resources).

```gdscript
extends CharacterBody3D

# --- Basic ---
# Automatically saved to the snapshot
@persistent var player_name: String = "Hero"

# --- Editor Integration (@export) ---
# Designers set initial value in editor. SaveServer saves runtime changes.
@export @persistent var max_health: int = 100
@export @persistent var current_health: int = 100

# --- Tags (Filtered Saves) ---
# Adds "inventory" tag. Allows saving ONLY this later:
# SaveServer.save_snapshot(root, "slot_1", true, ["inventory"])
@persistent("inventory") var items: Array = []

# --- Groups (Syntactic Sugar) ---
# Clean way to apply "progression" tag to multiple variables
@persistent_group("progression")
@persistent var level: int = 1
@persistent var experience: int = 0
```

This completely eliminates the need for manual dictionaries or custom `ResourceSaver` logic.

**Advanced Use Cases:**
Since it works at the object level, you can preserve volatile state that is usually lost, like audio playback:

```gdscript
extends AudioStreamPlayer

# Resumes music exactly where it stopped
@persistent var playback_position: float = 0.0
@persistent var is_playing: bool = false
```

No complex serialization logic needed; just mark what matters.

### 2. Building Custom Systems: `_save_persistence` and `_load_persistence`

While `@persistent` covers simple use cases, the `_save_persistence` and `_load_persistence` virtual functions are designed for developers who need to build complex, custom save architectures.

#### Pattern 1: Manual State Gathering (Full Control)

This separation allows developers to create sophisticated save systems (using the "twins" to define the logic) without ever touching `FileAccess` or worrying about blocking the main thread.

Here is a practical example - an spawner that needs to manually gather data from children:

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

For scenarios requiring high stability with complex objects, `SaveServer` provides a **Reconstructive ID Model**. The manager only handles the spawning and re-linking of the `persistence_id`.

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

### 3. The Snapshot Resource

The `Snapshot` class is a new `Resource` type that encapsulates saved game state. It serves as the container format for all save files, whether text or binary.

**Properties:**

- **`snapshot`** (Dictionary): The actual game state data collected from persistent properties and `_save_persistence()` calls
- **`version`** (String): The game version when this save was created (from `application/config/version`)
- **`metadata`** (Dictionary): Lightweight data for UI display (playtime, level name, character name, etc.)
- **`thumbnail`** (Texture2D): Optional screenshot taken at save time
- **`checksum`** (String): SHA-256 hash of the snapshot data for integrity validation

The separation of `metadata` from `snapshot` is crucial for performance. When displaying a save file list in the UI, you can load just the metadata and thumbnail without deserializing the entire game state:

```gdscript
# Load menu can show save info without loading full state
var save_info = SaveServer.get_save_metadata("slot_1")
$Label.text = "%s - Level %d - %s" % [
    save_info["character_name"],
    save_info["level"],
    save_info["playtime"]
]
$TextureRect.texture = save_info["thumbnail"]
```

This design mirrors how modern games display save file information instantly, without the loading delays that would come from deserializing gigabytes of game state just to show a preview.

### 4. The `SaveServer` Singleton

A new core singleton responsible for orchestrating the save process. It acts similarly to `AudioServer` or `PhysicsServer`, delegating the heavy work to a dedicated system.

Basic usage would look like this:

```gdscript
# Save the entire game state starting from the root node
await SaveServer.save_snapshot(get_tree().root, "save_slot_1")

# Load it back
SaveServer.load_snapshot(get_tree().root, "save_slot_1")
```

The server handles:

- **Background Threading**: Writing to disk happens asynchronously.
- **Atomic Backups**: Automatically keeps a `.bak` file. If the main save file is corrupted, it restores the backup.
- **Double-Layer Security**:
  - **Integrity Validation**: Computes SHA-256 checksums to validate data integrity before injection.
  - **Secure Binary Format**: Supports AES encryption for `.data` files using a project key.
- **Versioning**: Tracks game versions to allow for data migration.

## Technical Details

### Property Usage Flag

This implementation introduces a new `PROPERTY_USAGE_PERSISTENCE` flag (bit 30) in the `Object` class's property system. The `@persistent` annotation sets this flag on script variables during compilation.

When the GDScript compiler encounters `@persistent`, it:

1. Sets the `PROPERTY_USAGE_PERSISTENCE` flag on the property's `PropertyInfo`
2. Optionally stores tags in the property's `hint_string` (for filtered saving)
3. Validates that the property has explicit type annotation (required for reliable serialization)

The engine's property iteration system can then efficiently identify which properties need to be saved by checking for this flag, without requiring any runtime dictionary construction or manual property lists.

### Project Settings

The system would add a new `persistence/` section to Project Settings with the following options:

**`persistence/save_format`** (enum: Text, Binary)

- **Text**: Saves as `.tres` files (human-readable, version-control friendly, good for development)
- **Binary**: Saves as `.data` files (supports encryption and compression, good for production)
- Default: Text

**`persistence/save_path`** (String)

- Base directory for save files
- Default: `"user://saves/"`

**`persistence/encryption_key`** (String)

- AES encryption key for binary saves (32 characters)
- Auto-generated if empty (editor only)
- Only applies to binary format

**`persistence/compression_enabled`** (bool)

- Enables ZSTD compression for binary saves
- Significantly reduces file size with minimal CPU cost
- Default: true

**`persistence/backup_enabled`** (bool)

- Automatically maintains `.bak` shadow copies
- Restores from backup if main save is corrupted
- Default: true

**`persistence/integrity_check_level`** (enum: None, Signature, Strict)

- **None**: No validation (fastest)
- **Signature**: SHA-256 checksum validation
- **Strict**: Checksum + version compatibility check
- Default: Signature

These settings provide production-ready security and reliability out of the box, while remaining configurable for different project needs.

### Migration Support

Games evolve, and save files break. The `SaveServer` allows registering migration callbacks to upgrade old save data:

```gdscript
SaveServer.register_migration("1.0", "1.1", func(data):
    # Migrate data structure
    data["new_stat"] = 10
)
```

## Why This Matters

I think this would be a huge quality-of-life improvement for Godot. Save systems are one of those things that every non-trivial game needs, but the engine doesn't really help with. You either spend time building your own infrastructure or you ship with a fragile JSON-based system and hope for the best.

Having this built-in would put Godot ahead of Unity (which has nothing like this) and on par with Unreal's save game system, but simpler to use. More importantly, it would let developers - especially newer ones - focus on making their game instead of debugging why their save file got corrupted.

The Object-level implementation makes it flexible enough for any use case, from simple platformers to complex RPGs with thousands of persistent entities. And because it's declarative with `@persistent`, it's hard to mess up - you just mark what you want saved and the engine does the rest.
