#ifndef GAMEPLAY_EDITOR_PLUGIN_H
#define GAMEPLAY_EDITOR_PLUGIN_H

#include "editor/editor_plugin.h"

class GameplayEditorPlugin : public EditorPlugin {
	GDCLASS(GameplayEditorPlugin, EditorPlugin);

public:
	virtual String get_name() const override { return "GameplayAbilitySystem"; }

	virtual void _enter_tree() override;
	virtual void _exit_tree() override;

	GameplayEditorPlugin();
	~GameplayEditorPlugin();
};

#endif // GAMEPLAY_EDITOR_PLUGIN_H
