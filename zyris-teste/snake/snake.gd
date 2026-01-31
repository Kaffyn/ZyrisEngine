extends Node2D

@export var grid_size: int = 20
@export var tick_speed: float = 0.15

var snake: Array[Vector2i] = [Vector2i(10, 10), Vector2i(9, 10), Vector2i(8, 10)]
var direction: Vector2i = Vector2i.RIGHT
var food_pos: Vector2i
var score: int = 0
var game_over: bool = false
var tick_timer: float = 0.0

@onready var pet_data = get_node("/root/LSSRoot/Hub/PetData") # Example path

func _ready():
	spawn_food()

func _process(delta):
	if game_over:
		if Input.is_action_just_pressed("ui_accept"):
			reset_game()
		return

	handle_input()
	
	tick_timer += delta
	if tick_timer >= tick_speed:
		tick_timer = 0.0
		move_snake()

func handle_input():
	if Input.is_action_just_pressed("ui_up") and direction != Vector2i.DOWN:
		direction = Vector2i.UP
	elif Input.is_action_just_pressed("ui_down") and direction != Vector2i.UP:
		direction = Vector2i.DOWN
	elif Input.is_action_just_pressed("ui_left") and direction != Vector2i.RIGHT:
		direction = Vector2i.LEFT
	elif Input.is_action_just_pressed("ui_right") and direction != Vector2i.LEFT:
		direction = Vector2i.RIGHT

func move_snake():
	var new_head = snake[0] + direction
	
	# Check collisions
	if new_head.x < 0 or new_head.x >= grid_size or new_head.y < 0 or new_head.y >= grid_size or new_head in snake:
		end_game()
		return
		
	snake.insert(0, new_head)
	
	if new_head == food_pos:
		score += 10
		spawn_food()
		# Potentially increase speed
		tick_speed = max(0.05, tick_speed - 0.002)
	else:
		snake.pop_back()
	
	queue_redraw()

func spawn_food():
	while true:
		food_pos = Vector2i(randi() % grid_size, randi() % grid_size)
		if not food_pos in snake:
			break

func end_game():
	game_over = true
	print("Game Over! Score: ", score)
	if pet_data:
		pet_data.add_coins(score / 2) # Reward half the score as coins

func reset_game():
	snake = [Vector2i(10, 10), Vector2i(9, 10), Vector2i(8, 10)]
	direction = Vector2i.RIGHT
	score = 0
	game_over = false
	tick_speed = 0.15
	spawn_food()

func _draw():
	# Draw background
	draw_rect(Rect2(0, 0, grid_size * 20, grid_size * 20), Color(0.1, 0.1, 0.1), true)
	
	# Draw food
	draw_rect(Rect2(food_pos.x * 20, food_pos.y * 20, 20, 20), Color.RED, true)
	
	# Draw snake
	for i in range(snake.size()):
		var color = Color.GREEN if i == 0 else Color.DARK_GREEN
		draw_rect(Rect2(snake[i].x * 20, snake[i].y * 20, 20, 20), color, true)
