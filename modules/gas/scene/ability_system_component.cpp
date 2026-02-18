/**************************************************************************/
/*  ability_system_component.cpp                                          */
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

#include "modules/gas/scene/ability_system_component.h"
#include "modules/gas/resources/gameplay_cue.h"
#include "modules/gas/core/gameplay_cue_spec.h"
#include "scene/animation/animation_player.h"

void AbilitySystemComponent::add_attribute_set(const Ref<AttributeSet> &p_set) {
	ERR_FAIL_COND(p_set.is_null());
	attribute_sets.push_back(p_set);
	p_set->set_owner(this);
}

Ref<AttributeSet> AbilitySystemComponent::get_attribute_set(const StringName &p_class_name) const {
	for (const Ref<AttributeSet> &set : attribute_sets) {
		if (set->get_class_name() == p_class_name) {
			return set;
		}
	}
	return Ref<AttributeSet>();
}

real_t AbilitySystemComponent::get_attribute_value(const StringName &p_name) const {
	for (const Ref<AttributeSet> &set : attribute_sets) {
		if (set->has_attribute(p_name)) {
			return set->get_current_value(p_name);
		}
	}
	return 0.0;
}

void AbilitySystemComponent::apply_gameplay_effect_to_self(const Ref<GameplayEffect> &p_effect) {
	ERR_FAIL_COND(p_effect.is_null());

	emit_effect_applied(p_effect);

	if (p_effect->get_duration_type() == GE_DURATION_INSTANT) {
		for (int i = 0; i < p_effect->get_modifier_count(); i++) {
			StringName attr = p_effect->get_modifier_attribute(i);
			GameplayEffectModifierOp op = p_effect->get_modifier_op(i);
			real_t val = p_effect->get_modifier_value(i);

			for (Ref<AttributeSet> &set : attribute_sets) {
				if (set->has_attribute(attr)) {
					real_t current = set->get_base_value(attr);
					switch (op) {
						case GE_MOD_ADD:
							set->set_base_value(attr, current + val);
							break;
						case GE_MOD_MULTIPLY:
							set->set_base_value(attr, current * val);
							break;
						case GE_MOD_DIVIDE:
							set->set_base_value(attr, current / val);
							break;
						case GE_MOD_OVERRIDE:
							set->set_base_value(attr, val);
							break;
					}
				}
			}
		}
	} else {
		Ref<GameplayEffectSpec> spec;
		spec.instantiate();
		spec->set_effect(p_effect);
		active_effects.push_back(spec);

		// Apply granted tags
		if (p_effect->get_granted_tags().is_valid()) {
			active_tags->add_container(p_effect->get_granted_tags());
		}
	}
}

void AbilitySystemComponent::give_ability(const Ref<GameplayAbility> &p_ability) {
	ERR_FAIL_COND(p_ability.is_null());
	Ref<GameplayAbilitySpec> spec;
	spec.instantiate();
	spec->set_ability(p_ability);
}

bool AbilitySystemComponent::try_activate_ability(const Ref<GameplayAbility> &p_ability) {
	ERR_FAIL_COND_V(p_ability.is_null(), false);

	if (!p_ability->can_activate(this)) {
		return false;
	}

	if (p_ability->get_activation_blocked_tags().is_valid() && active_tags->has_any(p_ability->get_activation_blocked_tags())) {
		return false;
	}

	Ref<GameplayAbilitySpec> spec;
	spec.instantiate();
	spec->set_ability(p_ability);
	spec->set_active(true);
	active_abilities.push_back(spec);

	emit_ability_activated(p_ability);
	p_ability->activate_ability(this);

	return true;
}

void AbilitySystemComponent::notify_ability_ended(GameplayAbility *p_ability) {
	for (int i = 0; i < active_abilities.size(); i++) {
		if (active_abilities[i]->get_ability().ptr() == p_ability) {
			active_abilities.remove_at(i);
			break;
		}
	}
	emit_ability_ended(Ref<GameplayAbility>(p_ability)); // Create Ref to pass as signal argument, assumes it's managed elsewhere or stays valid
}


void AbilitySystemComponent::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (!audio_player_path.is_empty()) {
				cached_audio_player = get_node_or_null(audio_player_path);
			}
			if (!animation_player_path.is_empty()) {
				Node *node = get_node_or_null(animation_player_path);
				cached_animation_player = Object::cast_to<AnimationPlayer>(node);
			}
		} break;

		case NOTIFICATION_INTERNAL_PROCESS: {
			real_t delta = get_process_delta_time();
			for (int i = active_effects.size() - 1; i >= 0; i--) {
				Ref<GameplayEffectSpec> spec = active_effects[i];
				if (spec->get_effect()->get_duration_type() == GE_DURATION_HAS_DURATION) {
					spec->set_duration_remaining(spec->get_duration_remaining() - delta);
					if (spec->is_expired()) {
						active_effects.remove_at(i);
					}
				}
			}
		} break;
	}
}

void AbilitySystemComponent::execute_gameplay_cue(const Ref<GameplayCue> &p_cue, const Vector3 &p_location) {
	if (p_cue.is_null()) {
		return;
	}

	Ref<GameplayCueSpec> spec;
	spec.instantiate();
	spec->set_cue(p_cue);
	spec->invoke(this, p_location);
}

void AbilitySystemComponent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("execute_gameplay_cue", "cue", "location"), &AbilitySystemComponent::execute_gameplay_cue);

	ClassDB::bind_method(D_METHOD("try_activate_ability", "ability"), &AbilitySystemComponent::try_activate_ability);

	ClassDB::bind_method(D_METHOD("add_tag", "tag"), &AbilitySystemComponent::add_tag);
	ClassDB::bind_method(D_METHOD("remove_tag", "tag"), &AbilitySystemComponent::remove_tag);

	ADD_SIGNAL(MethodInfo("tag_added", PropertyInfo(Variant::STRING_NAME, "tag")));
	ADD_SIGNAL(MethodInfo("tag_removed", PropertyInfo(Variant::STRING_NAME, "tag")));
	ADD_SIGNAL(MethodInfo("ability_activated", PropertyInfo(Variant::OBJECT, "ability", PROPERTY_HINT_RESOURCE_TYPE, "GameplayAbility")));
	ADD_SIGNAL(MethodInfo("ability_ended", PropertyInfo(Variant::OBJECT, "ability", PROPERTY_HINT_RESOURCE_TYPE, "GameplayAbility")));
	ADD_SIGNAL(MethodInfo("effect_applied", PropertyInfo(Variant::OBJECT, "effect", PROPERTY_HINT_RESOURCE_TYPE, "GameplayEffect")));

	ClassDB::bind_method(D_METHOD("set_audio_player_path", "path"), &AbilitySystemComponent::set_audio_player_path);
	ClassDB::bind_method(D_METHOD("get_audio_player_path"), &AbilitySystemComponent::get_audio_player_path);
	ClassDB::bind_method(D_METHOD("set_animation_player_path", "path"), &AbilitySystemComponent::set_animation_player_path);
	ClassDB::bind_method(D_METHOD("get_animation_player_path"), &AbilitySystemComponent::get_animation_player_path);

	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "audio_player_path"), "set_audio_player_path", "get_audio_player_path");
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "animation_player_path"), "set_animation_player_path", "get_animation_player_path");
}

void AbilitySystemComponent::add_tag(const StringName &p_tag) {
	active_tags->add_tag(p_tag);
	emit_tag_added(p_tag);
}

void AbilitySystemComponent::remove_tag(const StringName &p_tag) {
	active_tags->remove_tag(p_tag);
	emit_tag_removed(p_tag);
}

void AbilitySystemComponent::emit_tag_added(const StringName &p_name) {
	emit_signal("tag_added", p_name);
}

void AbilitySystemComponent::emit_tag_removed(const StringName &p_name) {
	emit_signal("tag_removed", p_name);
}

void AbilitySystemComponent::emit_ability_activated(const Ref<GameplayAbility> &p_ability) {
	emit_signal("ability_activated", p_ability);
}

void AbilitySystemComponent::emit_ability_ended(const Ref<GameplayAbility> &p_ability) {
	emit_signal("ability_ended", p_ability);
}

void AbilitySystemComponent::emit_effect_applied(const Ref<GameplayEffect> &p_effect) {
	emit_signal("effect_applied", p_effect);
}

AbilitySystemComponent::AbilitySystemComponent() {
	active_tags.instantiate();
	set_process_internal(true);
}
