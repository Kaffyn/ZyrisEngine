extends Node2D
# zyris-teste/snake/snake_game.gd

@onready var snake = $SnakePlayer
@onready var hud = $SnakeHUD
@onready var food = $Food
@onready var timer = $GameTimer

var score: int = 0
var window_size: Vector2
var is_game_over: bool = false

func _ready():
	window_size = get_viewport_rect().size
	snake.grid_size = window_size
	snake.collided.connect(_on_snake_collided)
	snake.food_eaten.connect(_on_food_eaten)
	start_game()

func _input(event):
	if is_game_over:
		if event.is_action_pressed("ui_accept"):
			start_game()
		elif event.is_action_pressed("pause"):
			go_to_hub()

func start_game():
	score = 0
	is_game_over = false
	hud.update_score(score)
	# Assuming snake_hud has a way to hide the panel
	hud.get_node("Control/GameOverPanel").hide()
	hud.get_node("Control/GameOverPanel/VBox/RestartHint").text = "Press Action to Restart"
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
	is_game_over = true
	timer.stop()
	hud.show_game_over(score)
	# Pay the player
	var root = get_node_or_null("/root/LSSRoot")
	if root:
		var pet_data = root.get_node_or_null("PetData")
		if pet_data:
			var base_coins = score / 5
			pet_data.add_coins(base_coins)

			if pet_data.check_snake_record(score):
				hud.get_node("Control/GameOverPanel/VBox/RestartHint").text = "NEW RECORD! +50 Bonus Coins"

func go_to_hub():
	var root = get_node_or_null("/root/LSSRoot")
	if root and root.has_method("transition_to_scene"):
		root.transition_to_scene("res://hub/hub.tscn", root.GameState.MENU)

func _draw():
	# Draw Chessboard Grid
	var tile_size = 20
	var cols = int(window_size.x / tile_size) + 1
	var rows = int(window_size.y / tile_size) + 1

	for x in range(cols):
		for y in range(rows):
			if (x + y) % 2 == 1:
				draw_rect(Rect2(x * tile_size, y * tile_size, tile_size, tile_size), Color(0.12, 0.12, 0.12, 1.0))
			else:
				draw_rect(Rect2(x * tile_size, y * tile_size, tile_size, tile_size), Color(0.08, 0.08, 0.08, 1.0))
