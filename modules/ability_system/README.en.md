# Zyris Engine - Ability System Module

This module implements a robust, data-driven Ability System for the Zyris Engine, inspired by industry standards but tailored for Godot's node-based architecture.

## 📂 Architecture & File Structure

The module is organized into logical folders separating core logic, data resources, scene components, and editor tools.

### Module Root - Registration and Configuration

- **`register_types.h/cpp`**: Defines the module entry points, registering all classes to `ClassDB` and initializing the `AbilitySystem` singleton.
- **`config.py`**: SCons configuration and definition of classes that have XML documentation.
- **`SCsub`**: Build script that organizes the inclusion of all subfolders.

### `scene/` - Scene Components

- **`ability_system_component.h/cpp` (`AbilitySystemComponent`)**:
  - **Purpose**: The Actor's brain. Manages abilities, attributes, and states.
  - **Features**:
    - **Sibling Cache**: Automatically detects and caches sibling nodes (`CharacterBody2D/3D`, `AnimationPlayer`, `AudioStreamPlayer2D/3D`, `RayCast2D/3D`, `GPUParticles2D/3D`, `Marker2D/3D`).
    - **Reactive Signals**: Emits native signals like `attribute_changed`, `tag_changed`, and `ability_activated`.
    - **Performance**: Pure C++ processing of hierarchical tags and attribute modifiers.
    - **Netcode**: `get_net_state` and `apply_net_state` methods for multiplayer synchronization.

### `resources/` - Data Definitions

- **`ability_system_attribute_set.h/cpp` (`AbilitySystemAttributeSet`)**: Defines sets of attributes (Health, Mana, etc.) with support for Base and Current values.
- **`ability_system_ability.h/cpp` (`AbilitySystemAbility`)**: Resource that defines an ability's logic, including activation and blocking tags.
- **`ability_system_effect.h/cpp` (`AbilitySystemEffect`)**: Defines attribute modifiers, durations, and granted tags.
- **`ability_system_cue.h/cpp` (`AbilitySystemCue`)**: Defines audio and visual feedback triggered by events.
- **`ability_system_tag.h/cpp` (`AbilitySystemTag`)**: Resource representing a single hierarchical Gameplay Tag.
- **`ability_system_tag_container.h/cpp` (`AbilitySystemTagContainer`)**: Container for managing tag collections with optimized searching.

### `core/` - Internal Logic & Control

- **`ability_system.h/cpp` (`AbilitySystem`)**: Central singleton (Server) that acts as the project's Gameplay Tag master registry.
- **`ability_system_ability_spec.h/cpp`**: Represents an active instance of an ability in an ASC.
- **`ability_system_effect_spec.h/cpp`**: Represents an applied and active effect, tracking its remaining duration.
- **`ability_system_cue_spec.h/cpp`**: Runtime manager for Cue execution.
- **`ability_system_magnitude_calculation.h/cpp` (MMC)**: Base class for creating complex magnitude calculation formulas (e.g., damage based on stats).
- **`ability_system_task.h/cpp`**: Base for asynchronous tasks within abilities (e.g., waiting for a delay or input).
- **`ability_system_target_data.h/cpp`**: Serializable structure for transporting target information between client and server.

### `editor/` - Tooling

- **`ability_system_editor_plugin.h/cpp`**: Main editor plugin. Integrates specialized Tag and Attribute selectors into Godot's Inspector.

---

## 🛠️ Tutorials & Examples

### 1. Combat System (Net State and MMC)

```gdscript
extends CharacterBody3D

@onready var asc = $AbilitySystemComponent

func perform_attack(target_asc: AbilitySystemComponent):
    var spec = asc.make_outgoing_spec(load("res://effects/damage_fire.tres"))
    spec.set_magnitude("base_damage", 50.0 * asc.get_attribute_value("stats.intellect"))
    asc.apply_gameplay_effect_spec_to_target(spec, target_asc)
```

### 2. Asynchronous Abilities (Tasks)

```gdscript
extends AbilitySystemAbility

func _activate_ability():
    asc.play_montage("attack_sequence_a")
    # Conceptual example of an asynchronous Task
    var event = await asc.wait_for_gameplay_event("event.combat.impact").completed
    _spawn_hitbox(event.position)
    end_ability()
```

### 3. Reactive UI (Signals)

```gdscript
extends ProgressBar

func _ready():
    var asc = get_owner().get_node("AbilitySystemComponent")
    asc.attribute_changed.connect(_on_health_changed)

func _on_health_changed(attr_name: StringName, old: float, new: float):
    if attr_name == &"health":
        create_tween().tween_property(self, "value", new, 0.2)
```

---

## ⚠️ Important Notes

- **Networking**: Netcode uses **Net State** synchronization (do not confuse with save snapshots).
- **Performance**: All heavy processing of tags and modifiers is done in C++.
- **Visuals**: Use `AbilitySystemCue` to keep logic decoupled from visual effects.

_This document defines the official technical specification for the GAS module in the Zyris Engine._
