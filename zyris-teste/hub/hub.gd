extends Node2D
# zyris-teste/hub/hub.gd

@onready var pet_data = $PetData
@onready var coins_label = $CanvasLayer/HUD/CoinsLabel

func _process(_delta):
	if pet_data:
		coins_label.text = "Coins: " + str(pet_data.coins)

func _on_play_snake_pressed():
	# Transition to snake game via LSS Super Node
	var root = get_parent()
	if root and root.has_method("transition_to_scene"):
		root.transition_to_scene("res://snake/snake_game.tscn")
	else:
		print("Error: LSSRoot not found or transition_to_scene missing")
