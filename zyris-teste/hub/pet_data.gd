extends Node
# zyris-teste/hub/pet_data.gd

@persistent var coins: int = 0:
	set(value):
		coins = value
		SaveServer.stage_change(get_instance_id())

@persistent var hunger: float = 100.0:
	set(value):
		hunger = value
		SaveServer.stage_change(get_instance_id())

@persistent var energy: float = 100.0:
	set(value):
		energy = value
		SaveServer.stage_change(get_instance_id())

@persistent var pet_name: String = "Zyris":
	set(value):
		pet_name = value
		SaveServer.stage_change(get_instance_id())

@persistent var snake_high_score: int = 0:
	set(value):
		snake_high_score = value
		SaveServer.stage_change(get_instance_id())

# Instance identification for the SaveServer
func _ready():
	persistence_id = "PetData_Main"

func add_coins(amount: int):
	self.coins += amount
	# Trigger async incremental save after significant change
	SaveServer.amend_save(get_node("/root/LSSRoot"), "zyris_save")

func check_snake_record(current_score: int) -> bool:
	if current_score > snake_high_score:
		self.snake_high_score = current_score
		add_coins(50) # Bonus for breaking record
		return true
	return false
