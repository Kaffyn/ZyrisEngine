/**************************************************************************/
/*  gameplay_ability.cpp                                                  */
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

#include "modules/gas/resources/gameplay_ability.h"
#include "modules/gas/scene/ability_system_component.h"
#include "modules/gas/resources/gameplay_effect.h"
#include "modules/gas/resources/gameplay_cue.h"
#include "scene/3d/node_3d.h"

bool GameplayAbility::can_activate(AbilitySystemComponent *p_asc) const {
	ERR_FAIL_COND_V(p_asc == nullptr, false);

	// Check required tags
	if (activation_required_tags.is_valid() && !p_asc->has_all_tags(activation_required_tags)) {
		return false;
	}

	// Check blocked tags
	if (activation_blocked_tags.is_valid() && p_asc->has_any_tags(activation_blocked_tags)) {
		return false;
	}

	// Check Cooldown
	if (cooldown_tag != StringName() && p_asc->has_tag(cooldown_tag)) {
		return false;
	}

	// Check Costs
	if (!costs.is_empty()) {
		Array keys = costs.keys();
		for (int i = 0; i < keys.size(); i++) {
			StringName attr = keys[i];
			real_t cost = costs[attr];
			if (p_asc->get_attribute_value(attr) < cost) {
				return false;
			}
		}
	}

	bool script_ret = true;
	if (GDVIRTUAL_CALL(_can_activate, p_asc, script_ret)) {
		return script_ret;
	}

	return true;
}

void GameplayAbility::activate_ability(AbilitySystemComponent *p_asc) {
	asc = p_asc;

	// Apply Costs
	if (!costs.is_empty()) {
		Ref<GameplayEffect> cost_ge;
		cost_ge.instantiate();
		cost_ge->set_duration_type(GE_DURATION_INSTANT);

		Array keys = costs.keys();
		for (int i = 0; i < keys.size(); i++) {
			StringName attr = keys[i];
			real_t cost = costs[attr];
			// Cost reduces attribute, so subtract
			cost_ge->add_modifier(attr, GE_MOD_ADD, -cost); // Assuming cost is positive
		}
		p_asc->apply_gameplay_effect_to_self(cost_ge);
	}

	// Apply Cooldown
	if (cooldown_duration > 0.0 && cooldown_tag != StringName()) {
		Ref<GameplayEffect> cd_ge;
		cd_ge.instantiate();
		cd_ge->set_duration_type(GE_DURATION_HAS_DURATION);
		cd_ge->set_duration(cooldown_duration);

		Ref<GameplayTagContainer> granted;
		granted.instantiate();
		granted->add_tag(cooldown_tag);
		cd_ge->set_granted_tags(granted);

		p_asc->apply_gameplay_effect_to_self(cd_ge);
	}

	// Execute Cue
	if (cue_resource.is_valid()) {
		Node *owner = p_asc->get_parent(); // ASC is typically child of Actor
		Vector3 loc;
		Node3D *owner_3d = Object::cast_to<Node3D>(owner);
		if (owner_3d) {
			loc = owner_3d->get_global_position();
		}

		p_asc->execute_gameplay_cue(cue_resource, loc);
	}

	GDVIRTUAL_CALL(_activate_ability);
}

void GameplayAbility::end_ability() {
	if (asc) {
		asc->notify_ability_ended(this);
		asc = nullptr;
	}
}

void GameplayAbility::_bind_methods() {
	ClassDB::bind_method(D_METHOD("can_activate", "asc"), &GameplayAbility::can_activate);
	ClassDB::bind_method(D_METHOD("activate_ability", "asc"), &GameplayAbility::activate_ability);
	ClassDB::bind_method(D_METHOD("end_ability"), &GameplayAbility::end_ability);

	ClassDB::bind_method(D_METHOD("set_ability_tags", "tags"), &GameplayAbility::set_ability_tags);
	ClassDB::bind_method(D_METHOD("get_ability_tags"), &GameplayAbility::get_ability_tags);
	ClassDB::bind_method(D_METHOD("set_cancel_tags", "tags"), &GameplayAbility::set_cancel_tags);
	ClassDB::bind_method(D_METHOD("get_cancel_tags"), &GameplayAbility::get_cancel_tags);
	ClassDB::bind_method(D_METHOD("set_block_tags", "tags"), &GameplayAbility::set_block_tags);
	ClassDB::bind_method(D_METHOD("get_block_tags"), &GameplayAbility::get_block_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_tags"), &GameplayAbility::get_activation_required_tags);
	ClassDB::bind_method(D_METHOD("set_activation_blocked_tags", "tags"), &GameplayAbility::set_activation_blocked_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_tags"), &GameplayAbility::get_activation_blocked_tags);

	ClassDB::bind_method(D_METHOD("set_costs", "costs"), &GameplayAbility::set_costs);
	ClassDB::bind_method(D_METHOD("get_costs"), &GameplayAbility::get_costs);
	ClassDB::bind_method(D_METHOD("set_cooldown_duration", "duration"), &GameplayAbility::set_cooldown_duration);
	ClassDB::bind_method(D_METHOD("get_cooldown_duration"), &GameplayAbility::get_cooldown_duration);
	ClassDB::bind_method(D_METHOD("set_cooldown_tag", "tag"), &GameplayAbility::set_cooldown_tag);
	ClassDB::bind_method(D_METHOD("get_cooldown_tag"), &GameplayAbility::get_cooldown_tag);

	ClassDB::bind_method(D_METHOD("set_cue_resource", "cue"), &GameplayAbility::set_cue_resource);
	ClassDB::bind_method(D_METHOD("get_cue_resource"), &GameplayAbility::get_cue_resource);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ability_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_ability_tags", "get_ability_tags");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cancel_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_cancel_tags", "get_cancel_tags");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "block_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_block_tags", "get_block_tags");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "activation_required_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_activation_required_tags", "get_activation_required_tags");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "activation_blocked_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_activation_blocked_tags", "get_activation_blocked_tags");


	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "costs"), "set_costs", "get_costs");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "cooldown_duration"), "set_cooldown_duration", "get_cooldown_duration");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "cooldown_tag"), "set_cooldown_tag", "get_cooldown_tag");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cue_resource", PROPERTY_HINT_RESOURCE_TYPE, "GameplayCue"), "set_cue_resource", "get_cue_resource");

	GDVIRTUAL_BIND(_can_activate, "asc");
	GDVIRTUAL_BIND(_activate_ability);
}
