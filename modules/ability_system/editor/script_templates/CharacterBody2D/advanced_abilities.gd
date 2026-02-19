# meta-description: Advanced character controller with full Ability System and SaveServer integration

extends CharacterBody2D

# ============================================================================
# CORE COMPONENTS
# ============================================================================

@onready var ability_system: AbilitySystemComponent = $AbilitySystemComponent
@onready var animation_player: AnimationPlayer = $AnimationPlayer
@onready var sprite: AnimatedSprite2D = $AnimatedSprite2D

# ============================================================================
# PERSISTENCE (SaveServer Integration)
# ============================================================================

@persistent var character_name: String = "Player"
@persistent_group("attributes")
@persistent var level: int = 1
@persistent var experience: int = 0
@persistent_group("combat")
@persistent var current_health: float = 100.0
@persistent var max_health: float = 100.0
@persistent_group("movement")
@persistent var base_speed: float = 300.0
@persistent var jump_velocity: float = -400.0

# ============================================================================
# ABILITY SYSTEM CONFIGURATION
# ============================================================================

@export var ability_container: AbilitySystemAbilityContainer
@export var starting_abilities: Array[AbilitySystemAbility] = []

# Movement attributes managed by Ability System
var movement_speed: float
var jump_power: float
var can_move: bool = true
var can_jump: bool = true

# ============================================================================
# INITIALIZATION
# ============================================================================

func _ready() -> void:
	_setup_persistence_id()
	_setup_ability_system()
	_setup_initial_attributes()
	_connect_signals()

func _setup_persistence_id() -> void:
	if persistence_id.is_empty():
		persistence_id = &"character_" + str(get_instance_id())

func _setup_ability_system() -> void:
	if not ability_system:
		push_error("AbilitySystemComponent required!")
		return

	# Apply ability container if provided
	if ability_container:
		ability_system.apply_ability_container(ability_container)

	# Grant starting abilities
	for ability in starting_abilities:
		ability_system.give_ability(ability)

func _setup_initial_attributes() -> void:
	if not ability_system:
		return

	# Initialize base attributes
	ability_system.set_base_attribute(&"movement.speed", base_speed)
	ability_system.set_base_attribute(&"movement.jump_power", jump_velocity)
	ability_system.set_base_attribute(&"combat.health", current_health)
	ability_system.set_base_attribute(&"combat.max_health", max_health)

	# Cache current values
	movement_speed = ability_system.get_attribute_value(&"movement.speed")
	jump_power = ability_system.get_attribute_value(&"movement.jump_power")

func _connect_signals() -> void:
	if ability_system:
		ability_system.attribute_changed.connect(_on_attribute_changed)
		ability_system.tag_changed.connect(_on_tag_changed)

# ============================================================================
# GAMEPLAY LOOP
# ============================================================================

func _physics_process(delta: float) -> void:
	if not can_move:
		return

	_handle_gravity(delta)
	_handle_movement(delta)
	_handle_jump()
	_apply_movement()

func _handle_gravity(delta: float) -> void:
	if not is_on_floor():
		velocity += get_gravity() * delta

func _handle_movement(delta: float) -> void:
	var direction := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")

	if direction:
		velocity.x = direction.x * movement_speed
		_play_movement_animation(direction)
	else:
		velocity.x = move_toward(velocity.x, 0, movement_speed)
		_play_idle_animation()

func _handle_jump() -> void:
	if Input.is_action_just_pressed("ui_accept") and is_on_floor() and can_jump:
		velocity.y = jump_power

		# Try to activate jump ability
		if ability_system:
			ability_system.try_activate_ability_by_tag(&"ability.jump")

func _apply_movement() -> void:
	move_and_slide()

# ============================================================================
# ANIMATION SYSTEM
# ============================================================================

func _play_movement_animation(direction: Vector2) -> void:
	if not animation_player:
		return

	if direction.x > 0:
		sprite.flip_h = false
		animation_player.play("run")
	elif direction.x < 0:
		sprite.flip_h = true
		animation_player.play("run")

func _play_idle_animation() -> void:
	if animation_player and animation_player.has_animation("idle"):
		animation_player.play("idle")

# ============================================================================
# ABILITY SYSTEM EVENTS
# ============================================================================

func _on_attribute_changed(attribute_name: StringName, old_value: float, new_value: float) -> void:
	match attribute_name:
		&"movement.speed":
			movement_speed = new_value
		&"movement.jump_power":
			jump_power = new_value
		&"combat.health":
			current_health = new_value
			_handle_health_change(new_value)
		&"combat.max_health":
			max_health = new_value

func _on_tag_changed(tag_name: StringName, is_present: bool) -> void:
	match tag_name:
		&"state.stun":
			can_move = not is_present
		&"state.cant_jump":
			can_jump = not is_present
		&"state.dead":
			if is_present:
				_handle_death()

func _handle_health_change(health: float) -> void:
	# Update UI, play hurt animations, etc.
	if health <= 0 and not ability_system.has_tag(&"state.dead"):
		ability_system.add_tag(&"state.dead")

func _handle_death() -> void:
	can_move = false
	set_physics_process(false)

	if animation_player and animation_player.has_animation("death"):
		animation_player.play("death")

# ============================================================================
# PERSISTENCE HOOKS
# ============================================================================

func _save_persistence(state: Dictionary) -> void:
	super._save_persistence(state)

	# Save current ability system state
	if ability_system and state.has(&"combat"):
		state[&"combat"]["current_tags"] = ability_system.get_all_tags()
		state[&"combat"]["active_abilities"] = _get_active_ability_names()

func _load_persistence(data: Dictionary) -> void:
	super._load_persistence(data)

	# Restore ability system state
	if ability_system and data.has(&"combat"):
		var combat_data = data[&"combat"]

		if combat_data.has("current_tags"):
			for tag in combat_data["current_tags"]:
				ability_system.add_tag(tag)

		# Refresh cached values
		_setup_initial_attributes()

# ============================================================================
# UTILITY METHODS
# ============================================================================

func _get_active_ability_names() -> Array[String]:
	var names: Array[String] = []
	if ability_system:
		# This would need to be implemented in the AbilitySystemComponent
		pass
	return names

func take_damage(amount: float, damage_type: StringName = &"physical") -> void:
	if not ability_system:
		current_health = maxf(0, current_health - amount)
		return

	# Create damage effect spec
	var damage_effect = AbilitySystemEffect.new()
	damage_effect.modifiers = [{
		"attribute": &"combat.health",
		"operation": "ADD",
		"value": -amount
	}]

	var spec = ability_system.make_outgoing_spec(damage_effect)
	ability_system.apply_gameplay_effect_spec_to_self(spec)

func heal(amount: float) -> void:
	if not ability_system:
		current_health = minf(max_health, current_health + amount)
		return

	# Create healing effect spec
	var heal_effect = AbilitySystemEffect.new()
	heal_effect.modifiers = [{
		"attribute": &"combat.health",
		"operation": "ADD",
		"value": amount
	}]

	var spec = ability_system.make_outgoing_spec(heal_effect)
	ability_system.apply_gameplay_effect_spec_to_self(spec)
