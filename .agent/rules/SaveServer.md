---
trigger: always_on
---

# Core Persistence System (v2)

## 1. Engine Context

The persistence system is built on three fundamental engine pillars:

1. **Hierarchy and Behavior Inheritance:** The engine works as a behavior ladder. The base `Object` provides life to objects; `Node` allows them to exist in the scene; and `Resource` serves to store data. When you use something like `CanvasItem` (for 2D and Control) or `Node3D`, they are simply extending these bases and adding position logic. This allows any functionality injected into the base (like persistence) to be inherited by everything that comes after.

2. **Server-Based Architecture (Microservices):** To avoid overloading the game, heavy tasks are delegated to "specialists". The `AudioServer` focuses only on playing sound and managing volumes, while the `PhysicsServer` exclusively handles collisions. The `SaveServer` follows this same model: it is a dedicated engine for saving data in the background, without freezing your game.

3. **Abstraction via Integrated Language:** GDScript is not just for making the character walk; it is a tool that "intertwines" with the heart of the engine. It allows us to use "tags" (annotations like `@persistent`) and special methods that send complex orders to the system very simply, hiding all the math and heavy processing behind a single line of code.

## 2. Script Interface (GDScript)

Persistence is primarily declarative, minimizing boilerplate code.

### 2.1 Declarative Annotations

- **`@persistent`**: Marks a variable for automatic inclusion in the save system.
  - **Note:** For performance and security reasons, variables marked as `@persistent` **must** have explicit typing. The compiler will emit a warning if typing is omitted.

    ```gdscript
    @persistent var health: int = 100
    @persistent var current_inventory: Array = []
    ```

### 2.2 The `_save_persistence` Hook

For complex cases (like custom transformations or states that are not simple properties), the developer can implement this virtual hook.

- **Signature:** `func _save_persistence(state: Dictionary, tags: Array)`
- **Usage:** The system passes a `state` dictionary that should be filled with the additional data you want to persist.

  ```gdscript
  func _save_persistence(state: Dictionary, tags: Array):
      state["last_position"] = global_position
  ```

### 2.3 The `_load_persistence` Hook

- **Signature:** `func _load_persistence(data: Dictionary)`
- **Usage:** Called after automatic property restoration to process custom data received from the snapshot.

## 3. Node API (C++)

The `SaveServer` extends the base `Node` class to provide granular control over what is persisted.

### 3.1 Identification and Policy

- **`persistence_id` (StringName):** Global unique ID. Allows the node to be restored even if it changes name or position in the hierarchy. This ID is automatically registered in the `SaveServer` when set, enabling cross-hierarchy lookups.

- **`save_policy` (Enum):**
  - `SAVE_POLICY_INHERIT`: Follows the parent node's policy.
  - `SAVE_POLICY_ALWAYS`: Always tries to save this node.
  - `SAVE_POLICY_NEVER`: Ignores this node and all its children (ideal for static decoration).
  - `SAVE_POLICY_CUSTOM`: Reserved for future custom implementations.

**Key Behavior:** When a node's `save_policy` is set to `NEVER`, its `persistence_id` is automatically unregistered from the `SaveServer`. When changed back to a permissive policy, it is re-registered.

### 3.2 Dynamic Management

- `set_persistence_for_property(property, enabled)`: Allows toggling persistence of specific properties at runtime.

## 4. Orchestration by SaveServer

The `SaveServer` is not just a file writer; it is the choreographer of data collection in the scene.

### 4.1 Recursive Snapshots

- **`save_snapshot(root, slot_name, async, tags, metadata, thumbnail)`**: Initiates recursive collection from the `root` node. The entire tree is traversed and states are captured.
- **`load_snapshot(root, slot_name, callback)`**: Restores the state of the entire branch from the file on disk.

**Snapshot Structure:**

- `snapshot`: Automatic properties and return from `_save_persistence`.
- `.id`: The persistence ID, if present.
- `.children`: Recursive dictionary with the state of children.

#### 4.1.1 Partial Persistence (Tags)

The system allows saving only subsets of data using tags.

- **`save_snapshot(root, slot, async, tags: Array[String])`**: If provided, only properties marked with `@persistent(tag_name)` or included in `_save_persistence` dictionaries that match the tags will be processed.

### 4.2 The `Snapshot` Resource

Internally, the system uses a dedicated resource to encapsulate saved data:

- **`Snapshot` (Resource)**: This resource has the following main properties:
  - `snapshot` (Dictionary): The serialized state of the node tree.
  - `version` (String): The game version (`application/config/version`) captured at save time.
  - `metadata` (Dictionary): Lightweight data for quick visualization (e.g., playtime, coins, area name).
  - `thumbnail` (Texture2D): Optional screen capture at save time.
  - `checksum` (String): SHA-256 hash automatically generated to validate data integrity.

### 4.3 Global Settings (Project Settings)

The `SaveServer` behavior is controlled via `Project Settings > Application > Persistence`:

- **`save_format`**:
  - `Text (0)`: Generates `.tres` files (readable). Ideal for development and debugging.
  - `Binary (1)`: Generates `.data` files (binary). Ideal for production, supporting encryption and compression.

- **`encryption_key`**: 32-character key used to protect `.data` files.
  - **Auto-generation**: If the field is empty or reset in the Editor, the engine automatically generates a unique random key for the project.
  - **Critical Security**: If the key changes after release, old saves become unreadable. Use version control systems (Git) to back up `project.godot`.

- **`compression_enabled`**: When active, uses **ZSTD** compression on `.data` files, reducing disk size with minimal CPU cost.

- **`backup_enabled`**: When active, the system maintains a `.bak` file of the previous save. If loading the main save fails (corruption or crash during write), the backup is automatically restored.

- **`integrity_check_level`**:
  - `NONE`: Fast loading without validation.
  - `SIGNATURE`: Verifies if the Checksum matches the content (prevents simple manual editing).
  - `STRICT`: Validates Checksum and Versioning before any data injection.

### 4.4 Disk Persistence

Location and extension depend on the chosen format:

- **Location**: Generally stored in `user://saves/`.
- **Extensions**: `.tres` for text mode and `.data` for binary mode.

## 5. Technical Integration (Core Flags)

The system uses the global flag **`PROPERTY_USAGE_PERSISTENCE`** (1 << 30) defined in `object.h`. Any engine system (C++ or GDScript) can mark properties with this bit so that `Node` automatically includes them in the save state.

## 6. Data Evolution (Migrations)

To manage changes in data structure between game versions:

1. **`register_migration(from, to, callable)`**: Allows defining data transformations at runtime.
2. At `load_snapshot` time, if the captured version is lower than the current one, the `SaveServer` executes the registered migration chain before calling `_load_persistence` on nodes.

```gdscript
func _init():
    SaveServer.register_migration("1.0", "1.1", func(data):
        data["new_health_system"] = data.get("old_hp", 100)
    )
```

## 7. Incremental Persistence (Commit System)

The engine supports an incremental save model that avoids full scene traversals:

### 7.1 Dirty Object Tracking

When a persistent property changes via `set()`, the object automatically registers itself in the `SaveServer`'s `dirty_objects` list.

### 7.2 Incremental Save

- **`save_incremental(root, slot_name)`**: Instead of traversing the entire tree, only visits objects marked as dirty.
- If no base snapshot exists, performs a full save.
- Consolidates changes with the base snapshot and saves the merged state.

This approach is analogous to version control systems: the engine maintains a "base commit" (loaded state) and only saves "increments" (modified objects), significantly reducing I/O overhead in large scenes.

---

_This protocol standardizes persistence in the engine, focusing on simplicity for the user and raw performance in the backend._
