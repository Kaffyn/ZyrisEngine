extends Node3D

func _ready() -> void:
    print("Level 3D Loaded. Use Virtual Controls to fly.")

func _input(event: InputEvent) -> void:
    if event.is_action_pressed("ui_cancel"):
        # In a real game it would go back to the menu, here it exits or logs
        print("Exit requested")
        get_tree().quit()
