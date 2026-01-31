extends Camera3D

# Flight Settings
@export var move_speed: float = 10.0
@export var look_sensitivity: float = 2.0

func _ready() -> void:
    make_current()

func _input(event: InputEvent) -> void:
    # Mouse fallback for PC testing
    if event is InputEventMouseMotion and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
        _rotate_camera(event.relative * 0.005)

func _process(delta: float) -> void:
    _process_rotation(delta)
    _process_movement(delta)

func _process_rotation(delta: float) -> void:
    # Analog reading from Input Map ("look_*" mapped to Right Stick / TouchPad)
    var look_vec = Input.get_vector("look_left", "look_right", "look_up", "look_down")

    if look_vec.length_squared() > 0.01:
        # Sensitivity adjustment for continuous look
        var rot_velocity = look_vec * look_sensitivity * delta * 60.0
        _rotate_camera(rot_velocity * 0.05)

func _rotate_camera(relative: Vector2) -> void:
    rotate_y(-relative.x)
    rotation.x = clamp(rotation.x - relative.y, deg_to_rad(-90), deg_to_rad(90))
    rotation.z = 0

func _process_movement(delta: float) -> void:
    # Analog reading from Input Map ("move_*" mapped to Left Stick / WASD)
    var move_vec = Input.get_vector("move_left", "move_right", "move_up", "move_down")

    if move_vec.length_squared() > 0.01:
        var forward = -global_transform.basis.z
        var right = global_transform.basis.x

        var direction = (forward * -move_vec.y + right * move_vec.x).normalized()
        global_position += direction * move_speed * delta
