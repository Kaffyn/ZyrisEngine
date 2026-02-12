extends Node
# zyris-teste/core/lss_root.gd

# LSSRoot acts as the Super Node managing multiple worlds (Hub vs Games)
# and orchestrating the SaveServer with a Game State Machine (GSM).

enum GameState {
	BOOT,
	MENU,
	LOADING,
	GAMEPLAY,
	TRANSITION
}

var current_state: GameState = GameState.BOOT

func _ready():
	print("LSS: Initializing Super Node (GSM: BOOT)")
	current_state = GameState.BOOT
	load_game()
	# Typically boot leads to menu or hub
	current_state = GameState.MENU

func save_game():
	SaveServer.save_snapshot(self, "zyris_save", true)

func save_incremental():
	SaveServer.amend_save(self, "zyris_save")

func load_game():
	SaveServer.load_snapshot(self, "zyris_save")

func transition_to_scene(scene_path: String, next_state: GameState = GameState.GAMEPLAY):
	print("LSS: Transitioning (State: ", current_state, " -> ", next_state, ") to ", scene_path)
	current_state = GameState.TRANSITION

	# Save current state before switching
	save_incremental()

	# Clear current world (Hub or Game) - keeping LSSRoot-governed nodes like PetData
	for child in get_children():
		if child.name != "PetData": # Protect global data
			child.queue_free()

	# Load new scene
	current_state = GameState.LOADING
	var new_scene = load(scene_path).instantiate()
	add_child(new_scene)

	current_state = next_state

func _notification(what):
	if what == NOTIFICATION_WM_CLOSE_REQUEST:
		save_incremental()
