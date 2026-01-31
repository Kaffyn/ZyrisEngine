extends Node
# zyris-teste/hub/pet_data.gd

@persistent var coins: int = 0
@persistent var hunger: float = 100.0
@persistent var energy: float = 100.0
@persistent var pet_name: String = "Zyris"

# Instance identification for the SaveServer
func _ready():
	persistence_id = "PetData_Main"

func add_coins(amount: int):
	coins += amount
	# Trigger async save after significant change
	SaveServer.save_snapshot(get_node("/root/LSSRoot"), "zyris_save", true)
