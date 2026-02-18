# Zyris Engine - Gameplay Ability System (GAS) Module

This module implements a robust, data-driven Gameplay Ability System for the Zyris Engine, inspired by industry standards (like Unreal's GAS) but tailored for Godot's node-based architecture.

## 📂 Architecture & File Structure

The module is organized into logical folders separating core logic, data resources, scene components, and editor tools.

### `scene/` - Scene Components

* **`ability_system_component.h/cpp` (`AbilitySystemComponent`)**:
  * **Purpose**: The heart of the system. It MUST be added to any Actor (Character, Enemy, NPC) that wishes to use abilities, attributes, or effects.
  * **Responsibilities**: Manages active Tags, holds AttributeSets, executes Abilities, processes Effects, and handles Cues. It also integrates with sibling components (`AnimationPlayer`, `AudioStreamPlayer`) if configured.

### `resources/` - Data Definitions

* **`attribute_set.h/cpp` (`AttributeSet`)**:
  * **Purpose**: Defines a set of float values (Health, Mana, Stamina, etc.).
  * **Responsibilities**: Stores `BaseValue` (permanent) and `CurrentValue` (temporary/modified). Emits signals when values change.
* **`gameplay_ability.h/cpp` (`GameplayAbility`)**:
  * **Purpose**: Scriptable resource defining an action.
  * **Responsibilities**: Contains logic (`_activate_ability`), Costs, Cooldowns, and Tag requirements (Activation/Cancel/Block tags).
* **`gameplay_effect.h/cpp` (`GameplayEffect`)**:
  * **Purpose**: Data describing a change to the state.
  * **Responsibilities**: Defines attribute modifiers (Add, Multiply, Override), Duration (Instant, Finite, Infinite), and Granted Tags.
* **`gameplay_cue.h/cpp` (`GameplayCue`)**:
  * **Purpose**: Audio/Visual feedback definition.
  * **Responsibilities**: References VFX (`PackedScene`), SFX (`AudioStream`), and Animation names. Executed by the ASC.
* **`gameplay_tag.h/cpp` (`GameplayTag`)**: Represents a hierarchical tag (e.g., `state.debuff.stun`).
* **`gameplay_tag_container.h/cpp` (`GameplayTagContainer`)**: A collection of tags with fast matching logic (`has_tag`, `has_any`, `has_all`).

### `core/` - Internal Logic & Types

* **`gameplay_ability_spec.h/cpp`**: Runtime instance of an Ability (tracking active state).
* **`gameplay_effect_spec.h/cpp`**: Runtime instance of an Effect (tracking duration).
* **`gameplay_cue_spec.h/cpp`**: Runtime handler for Cues (spawns VFX/SFX or calls sibling nodes).

### `editor/` - Tooling

* **`gameplay_editor_plugin.h/cpp`**: Handles editor integration, inspector customization, and project settings.

---

## 🛠️ Tutorials & Examples

### 1. Setup an Actor

Add an `AbilitySystemComponent` node to your scene. Optionally, link sibling components for automatic handling.

```gdscript
extends CharacterBody3D

@onready var asc: AbilitySystemComponent = $AbilitySystemComponent

func _ready():
    # Setup sibling components (optional, but recommended for Cues)
    asc.set_audio_player_path("AudioStreamPlayer3D")
    asc.set_animation_player_path("AnimationPlayer")

    # Initialize Attributes
    var health_set = AttributeSet.new()
    health_set.register_attribute("health", 100.0)
    health_set.register_attribute("max_health", 100.0)

    # Connect signals
    health_set.attribute_changed.connect(_on_health_changed)

    # Add to ASC
    asc.add_attribute_set(health_set)

func _on_health_changed(attr_name, old_val, new_val):
    if attr_name == "health":
        print("Health changed: ", new_val)
```

### 2. Creating a Gameplay Ability

Create a new script extending `GameplayAbility` and save it as a resource `.tres`.

```gdscript
# my_fireball_ability.gd
extends GameplayAbility

func _activate_ability():
    # 'asc' is available via get_asc() or passed in context if we binding differently,
    # but typically usage in Zyris GAS passes 'asc' to activate.
    # NOTE: In current implementation, 'asc' is stored in the Spec, or passed to the C++ activate.
    # The virtual method _activate_ability assumes valid context.

    print("Fireball Casted!")

    # Logic to spawn projectile would go here

    # Commit ability end
    end_ability()
```

### 3. Applying a Gameplay Effect (Damage, Buff)

Effects are usually data-only Resources, but you can create them via code too.

```gdscript
func take_damage(amount: float):
    # Create an instant damage effect
    var damage_effect = GameplayEffect.new()
    damage_effect.duration_type = GameplayEffect.GE_DURATION_INSTANT

    # Add modifier: Health = Health + (-amount)
    damage_effect.add_modifier("health", GameplayEffect.GE_MOD_ADD, -amount)

    # Apply to self (or target's ASC)
    asc.apply_gameplay_effect_to_self(damage_effect)
```

### 4. Using Gameplay Cues

Cues decouple logic from visuals.

1. Create a `GameplayCue` resource.
2. Assign a `VFX Scene` (Particle) and/or `Sound`.
3. Assign it to your **Ability**'s `Cue Resource` property.

The ASC will automatically execute it when the ability activates.

Or trigger manually:

```gdscript
@export var hit_cue: GameplayCue

func on_hit(location: Vector3):
    asc.execute_gameplay_cue(hit_cue, location)
```

### 5. Using Tags

Tags are primary for state control (e.g., Stunned, Silenced).

```gdscript
# Check if owner has a specific tag
if asc.has_tag("state.debuff.stun"):
    print("I am stunned, cannot move!")

# Add a tag manually (usually done via GameplayEffect)
asc.add_tag("state.buff.speed_boost")
```

---

## ⚠️ Important Notes

* **Networking**: This module is designed with replication in mind, but netcode implementation is handled in the `scene` layer.
* **Performance**: Core logic is C++ for maximum throughput. GDScript interactions are optimized via `GDVIRTUAL` calls.
* **Visuals**: Use `GameplayCue` for ALL visual feedback. Do not hardcode particles in Ability logic if possible.
