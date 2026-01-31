extends Node2D

func _ready() -> void:
	print("Level 2D Loaded.")

func _input(event: InputEvent) -> void:
	if event.is_action_pressed("ui_cancel"):
		get_tree().quit()
