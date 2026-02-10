extends Node
# zyris-teste/hub/pet_data.gd

@persistent var coins: int = 0
@persistent var hunger: float = 100.0
@persistent var energy: float = 100.0
@persistent var pet_name: String = "Zyris"
@persistent var snake_high_score: int = 0

# Instance identification for the SaveServer
func _ready():
	persistence_id = "PetData_Main"

func add_coins(amount: int):
	coins += amount
	# Trigger async save after significant change
	SaveServer.save_snapshot(get_node("/root/LSSRoot"), "zyris_save", true)

func check_snake_record(current_score: int) -> bool:
	if current_score > snake_high_score:
		snake_high_score = current_score
		add_coins(50) # Bonus for breaking record
		return true
	return false
