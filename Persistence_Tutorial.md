# Documentation: High-Level Persistence System (v2)

This guide explains how to implement a robust, production-ready save system using the **SaveServer** and the `@persistent` annotation.

---

## 1. Core Philosophy: The Reconstructive Model

Our persistence system is built on a **Reconstructive ID model**. This ensures that both static and dynamic nodes can be perfectly restored across gaming sessions.

- 🏛️ **Static Nodes:** Objects that always exist in your scene (like the Player or UI). They use a **Global ID Registry** to reconnect their state even if they change name or hierarchy position.
- ⚡ **Dynamic Nodes:** Objects spawned during gameplay (Enemies, Loot). You must re-instantiate them during a load and re-apply their `persistence_id` to link them back to their saved data.

## 2. Setting Up a Persistent Variable

Saving data is as simple as adding the `@persistent` annotation.

> [!IMPORTANT]
> **Explicit typing is mandatory.** For performance and safety, variables marked as `@persistent` must have a defined type (e.g., `: int`, `: String`).

```gdscript
extends CharacterBody2D

@persistent var health: int = 100
@persistent var inventory_items: Array[String] = []
@persistent var current_checkpoint_id: StringName = &"checkpoint_alpha"
```

### 2.1 Using Tags for Selective Saves

You can group persistent variables using **Tags**. This allows you to save only "Combat" data or "Inventory" data separately.

```gdscript
@persistent("stats") var strength: int = 10
@persistent("inventory") var items: Array = []
```

## 3. Handling Dynamic Entities (The Manager Pattern)

When spawning objects at runtime, you must handle the **Reconstruction** phase.

### Step A: Capturing State (`_save_persistence`)

The parent node should implement `_save_persistence` to store metadata about its dynamic children.

```gdscript
# LevelManager.gd
func _save_persistence(state: Dictionary):
    var enemy_data = []
    for enemy in $Enemies.get_children():
        enemy_data.append({
            "id": enemy.persistence_id, # The unique ID managed by the engine
            "type": enemy.type_id,
            "pos": enemy.global_position
        })
    state["active_enemies"] = enemy_data
```

### Step B: Reconstructing Nodes (`_load_persistence`)

During the load phase, recreate the nodes and **re-apply their IDs** before adding them to the Tree.

```gdscript
# LevelManager.gd
func _load_persistence(data: Dictionary):
    if not data.has("active_enemies"): return

    for info in data["active_enemies"]:
        var enemy = ENEMY_SCENE.instantiate()

        # CRITICAL: Re-apply the persistence_id BEFORE adding to the tree
        enemy.persistence_id = info["id"]
        enemy.global_position = info["pos"]

        $Enemies.add_child(enemy)

        # Once in the tree, SaveServer automatically injects its
        # @persistent properties (like health/stats) from the snapshot.
```

## 4. Manual Hooks & Custom Data

Use these hooks for complex logic that isn't a simple property.

| Hook | Purpose |
| :--- | :--- |
| `_save_persistence(state: Dictionary)` | Fill the `state` dictionary with custom values. |
| `_load_persistence(data: Dictionary)` | Process custom values after automatic properties are loaded. |

```gdscript
func _save_persistence(state: Dictionary):
    state["custom_logic_data"] = { "power_level": calculate_power() }

func _load_persistence(data: Dictionary):
    var power = data.get("custom_logic_data", {}).get("power_level", 1.0)
    apply_power_modifier(power)
```

## 5. Advanced Evolution: Migrations

When you release a game update that changes your data structure, use **Migrations** to prevent save corruption.

```gdscript
func _init():
    # Upgrade save data from version 1.0 to 1.1
    SaveServer.register_migration("1.0", "1.1", func(data: Dictionary):
        if data.has("old_xp_system"):
            data["new_rank_system"] = convert_xp_to_rank(data["old_xp_system"])
    )
```

## 6. Performance: Incremental vs Full Saves

The system operates like **Git for Gameplay**. It avoids the "Save Stutter" common in large games by using **Incremental Commits**.

### 6.1 Full Snapshot (`save_snapshot`)

Traverses the entire scene tree. Required for the first save of an area or when a full world-state backup is needed.

### 6.2 Incremental Save (`save_incremental`)

The most efficient method. It utilizes **Dirty Tracking**:

1. When a `@persistent` variable is modified via `set()`, the object is automatically marked as "dirty".
2. `save_incremental()` visits **only** these dirty objects.
3. It performs a **Dictionary Merge** between the last loaded state and the new increments.

> [!TIP]
> Use `save_incremental()` for frequent autosaves or background checkpoints to reduce disk I/O by up to 95%.

## 7. Optimization & Best Practices

1. 🎯 **Minimalism:** Only mark variables that *actually* change.
2. 🔒 **Security:** Enable **ZSTD Compression** and **AES Encryption** in `Project Settings > Application > Persistence` for production builds.
3. 🔄 **Backups:** Keep `backup_enabled` on to protect players from crashes during the save process.
4. 🤖 **Automated IDs:** Never set `persistence_id` manually during normal gameplay. Let the engine generate it; you only re-apply it during the load reconstruction.

---
*Professional Grade Persistence Framework.*
