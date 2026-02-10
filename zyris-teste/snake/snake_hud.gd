extends CanvasLayer
# zyris-teste/snake/snake_hud.gd

@onready var score_label = $Control/ScoreLabel

func update_score(value: int):
	score_label.text = "Score: " + str(value)

func show_game_over(final_score: int):
	$Control/GameOverPanel.show()
	$Control/GameOverPanel/VBox/FinalScoreLabel.text = "Final: " + str(final_score)
