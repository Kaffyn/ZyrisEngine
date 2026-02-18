/**************************************************************************/
/*  ability_system_component.h                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "modules/gas/core/gameplay_ability_spec.h"
#include "modules/gas/core/gameplay_effect_spec.h"
#include "modules/gas/core/gameplay_tag_container.h"
#include "modules/gas/resources/attribute_set.h"
#include "scene/main/node.h"

class AnimationPlayer;
class GameplayCue;

class AbilitySystemComponent : public Node {
	GDCLASS(AbilitySystemComponent, Node);

	Ref<GameplayTagContainer> active_tags;

	Vector<Ref<GameplayAbilitySpec>> active_abilities;
	Vector<Ref<GameplayEffectSpec>> active_effects;
	Vector<Ref<AttributeSet>> attribute_sets;

	// Sibling Components
	NodePath audio_player_path;
	NodePath animation_player_path;
	Node *cached_audio_player = nullptr;
	AnimationPlayer *cached_animation_player = nullptr;

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	// Tags
	bool has_tag(const StringName &p_tag) const { return active_tags->has_tag_hierarchical(p_tag); }
	bool has_any_tags(const Ref<GameplayTagContainer> &p_tags) const { return active_tags->has_any(p_tags); }
	bool has_all_tags(const Ref<GameplayTagContainer> &p_tags) const { return active_tags->has_all(p_tags); }
	void add_tag(const StringName &p_tag);
	void remove_tag(const StringName &p_tag);

	// Attributes
	void add_attribute_set(const Ref<AttributeSet> &p_set);
	Ref<AttributeSet> get_attribute_set(const StringName &p_class_name) const;
	real_t get_attribute_value(const StringName &p_name) const;

	// Effects
	void apply_gameplay_effect_to_self(const Ref<GameplayEffect> &p_effect);
	void remove_active_effect(const Ref<GameplayEffect> &p_effect);

	void execute_gameplay_cue(const Ref<GameplayCue> &p_cue, const Vector3 &p_location);

	// Sibling Configuration
	void set_audio_player_path(const NodePath &p_path) { audio_player_path = p_path; }
	NodePath get_audio_player_path() const { return audio_player_path; }

	void set_animation_player_path(const NodePath &p_path) { animation_player_path = p_path; }
	NodePath get_animation_player_path() const { return animation_player_path; }

	Node *get_audio_player_node() const { return cached_audio_player; }
	AnimationPlayer *get_animation_player_node() const { return cached_animation_player; }

	// Abilities
	void give_ability(const Ref<GameplayAbility> &p_ability);
	bool try_activate_ability(const Ref<GameplayAbility> &p_ability);
	void notify_ability_ended(GameplayAbility *p_ability);

	AbilitySystemComponent();

	// Signals emission
	void emit_tag_added(const StringName &p_name);
	void emit_tag_removed(const StringName &p_name);
	void emit_ability_activated(const Ref<GameplayAbility> &p_ability);
	void emit_ability_ended(const Ref<GameplayAbility> &p_ability);
	void emit_effect_applied(const Ref<GameplayEffect> &p_effect);
};
