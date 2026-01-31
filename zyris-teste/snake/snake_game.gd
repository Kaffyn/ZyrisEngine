extends Node2D
# zyris-teste/snake/snake_game.gd

@onready var snake = $SnakePlayer
@onready var hud = $SnakeHUD
@onready var food = $Food
@onready var timer = $GameTimer

var score: int = 0
var window_size: Vector2

func _ready():
	window_size = get_viewport_rect().size
	snake.grid_size = window_size
	snake.collided.connect(_on_snake_collided)
	snake.food_eaten.connect(_on_food_eaten)
	start_game()

func start_game():
	score = 0
	hud.update_score(score)
	snake.reset()
	spawn_food()
	timer.start()

func _on_game_timer_timeout():
	snake.update_movement()

func _on_food_eaten():
	score += 10
	hud.update_score(score)
	spawn_food()

func _on_snake_collided(_type):
	end_game()

func spawn_food():
	# Simple random spawn within window
	food.position = Vector2(
		randi_range(40, window_size.x - 40),
		randi_range(40, window_size.y - 40)
	)

func end_game():
	timer.stop()
	hud.show_game_over(score)
	# Pay the player
	var root = get_node_or_null("/root/LSSRoot")
	if root:
		var pet_data = root.get_node_or_null("PetData")
		if pet_data:
			pet_data.add_coins(score / 5)
