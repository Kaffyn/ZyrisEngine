extends Node2D
# zyris-teste/snake/snake_player.gd

signal collided(type: String)
signal food_eaten

@export var step_size: int = 20
@export var grid_size: Vector2 = Vector2(1280, 720)

var body_nodes: Array[Node2D] = []
var direction: Vector2 = Vector2.RIGHT
var next_direction: Vector2 = Vector2.RIGHT

@onready var segment_scene = preload("res://snake/snake_segment.tscn")
@onready var head_area = $Head

func reset():
	# Clear previous segments
	for node in body_nodes:
		node.queue_free()
	body_nodes.clear()

	position = Vector2(100, 100)
	direction = Vector2.RIGHT
	next_direction = Vector2.RIGHT

	# Initial segments (start from behind the head)
	for i in range(1, 4):
		add_segment(position - Vector2(i * step_size, 0))

func add_segment(pos: Vector2):
	var seg = segment_scene.instantiate()
	get_parent().add_child.call_deferred(seg)
	seg.position = pos
	body_nodes.append(seg)

func update_movement():
	direction = next_direction
	var old_head_pos = position
	var new_head_pos = position + (direction * step_size)

	# Loop Logic
	if new_head_pos.x < 0: new_head_pos.x = grid_size.x - step_size/2
	elif new_head_pos.x > grid_size.x: new_head_pos.x = 0

	if new_head_pos.y < 0: new_head_pos.y = grid_size.y - step_size/2
	elif new_head_pos.y > grid_size.y: new_head_pos.y = 0

	position = new_head_pos

	# Move segments: move the last to where the head was
	var tail = body_nodes.pop_back()
	tail.position = old_head_pos
	body_nodes.insert(0, tail)

func grow():
	# Add a segment at current tail position
	var tail_pos = body_nodes.back().position
	add_segment(tail_pos)

func _input(_event):
	if Input.is_action_just_pressed("ui_up") and direction != Vector2.DOWN:
		next_direction = Vector2.UP
	elif Input.is_action_just_pressed("ui_down") and direction != Vector2.UP:
		next_direction = Vector2.DOWN
	elif Input.is_action_just_pressed("ui_left") and direction != Vector2.RIGHT:
		next_direction = Vector2.LEFT
	elif Input.is_action_just_pressed("ui_right") and direction != Vector2.LEFT:
		next_direction = Vector2.RIGHT

func _on_head_area_entered(area: Area2D):
	# Using match as requested by Machi
	var groups = area.get_groups()
	for g in groups:
		match g:
			"snake_food":
				food_eaten.emit()
				grow()
			"snake_snake":
				collided.emit("self")
