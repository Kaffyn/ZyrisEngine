/**************************************************************************/
/*  ability_system_ability.cpp                                            */
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

#include "ability_system_ability.h"
#include "modules/ability_system/core/ability_system_effect_spec.h"
#include "modules/ability_system/resources/ability_system_effect.h"
#include "modules/ability_system/resources/ability_system_tag_container.h"
#include "modules/ability_system/scene/ability_system_component.h"

void AbilitySystemAbility::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ability_tag", "tag"), &AbilitySystemAbility::set_ability_tag);
	ClassDB::bind_method(D_METHOD("get_ability_tag"), &AbilitySystemAbility::get_ability_tag);

	ClassDB::bind_method(D_METHOD("set_activation_owned_tags", "tags"), &AbilitySystemAbility::set_activation_owned_tags);
	ClassDB::bind_method(D_METHOD("get_activation_owned_tags"), &AbilitySystemAbility::get_activation_owned_tags);

	ClassDB::bind_method(D_METHOD("set_activation_required_tags", "tags"), &AbilitySystemAbility::set_activation_required_tags);
	ClassDB::bind_method(D_METHOD("get_activation_required_tags"), &AbilitySystemAbility::get_activation_required_tags);

	ClassDB::bind_method(D_METHOD("set_activation_blocked_tags", "tags"), &AbilitySystemAbility::set_activation_blocked_tags);
	ClassDB::bind_method(D_METHOD("get_activation_blocked_tags"), &AbilitySystemAbility::get_activation_blocked_tags);

	ClassDB::bind_method(D_METHOD("set_cost_effect", "effect"), &AbilitySystemAbility::set_cost_effect);
	ClassDB::bind_method(D_METHOD("get_cost_effect"), &AbilitySystemAbility::get_cost_effect);

	ClassDB::bind_method(D_METHOD("set_cooldown_effect", "effect"), &AbilitySystemAbility::set_cooldown_effect);
	ClassDB::bind_method(D_METHOD("get_cooldown_effect"), &AbilitySystemAbility::get_cooldown_effect);

	ClassDB::bind_method(D_METHOD("can_activate_ability", "owner"), &AbilitySystemAbility::can_activate_ability);
	ClassDB::bind_method(D_METHOD("activate_ability", "owner"), &AbilitySystemAbility::activate_ability);
	ClassDB::bind_method(D_METHOD("end_ability", "owner"), &AbilitySystemAbility::end_ability);

	GDVIRTUAL_BIND(_activate_ability, "owner");
	GDVIRTUAL_BIND(_can_activate_ability, "owner");
	GDVIRTUAL_BIND(_on_end_ability, "owner");

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag"), "set_ability_tag", "get_ability_tag");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_owned_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_owned_tags", "get_activation_owned_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_required_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_required_tags", "get_activation_required_tags");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "activation_blocked_tags", PROPERTY_HINT_ARRAY_TYPE, "StringName"), "set_activation_blocked_tags", "get_activation_blocked_tags");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cost_effect", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemEffect"), "set_cost_effect", "get_cost_effect");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cooldown_effect", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemEffect"), "set_cooldown_effect", "get_cooldown_effect");
}

bool AbilitySystemAbility::can_activate_ability(AbilitySystemComponent *p_owner) const {
	ERR_FAIL_NULL_V(p_owner, false);

	// Tag requirements
	if (!p_owner->get_owned_tags()->has_all_tags(activation_required_tags)) {
		return false;
	}

	if (p_owner->get_owned_tags()->has_any_tags(activation_blocked_tags)) {
		return false;
	}

	// Cost check
	if (cost_effect.is_valid()) {
		// In a real system we'd check if we CAN afford it without applying.
		// For now let's assume if it's instant modifiers we can check them.
		// Simplified for this implementation.
	}

	// Cooldown check
	if (cooldown_effect.is_valid()) {
		// Check if any active effect on owner has the cooldown tags
		TypedArray<StringName> cd_tags = cooldown_effect->get_granted_tags();
		if (p_owner->get_owned_tags()->has_any_tags(cd_tags)) {
			return false;
		}
	}

	// Script override
	bool script_ret = true;
	if (GDVIRTUAL_CALL(_can_activate_ability, p_owner, script_ret)) {
		if (!script_ret) {
			return false;
		}
	}

	return true;
}

void AbilitySystemAbility::activate_ability(AbilitySystemComponent *p_owner) {
	ERR_FAIL_NULL(p_owner);

	// Apply Cost
	if (cost_effect.is_valid()) {
		Ref<AbilitySystemEffectSpec> cost_spec = p_owner->make_outgoing_spec(cost_effect);
		p_owner->apply_gameplay_effect_spec_to_self(cost_spec);
	}

	// Apply Cooldown
	if (cooldown_effect.is_valid()) {
		Ref<AbilitySystemEffectSpec> cd_spec = p_owner->make_outgoing_spec(cooldown_effect);
		p_owner->apply_gameplay_effect_spec_to_self(cd_spec);
	}

	// Apply owned tags via ASC so tag_changed signal is emitted
	for (int i = 0; i < activation_owned_tags.size(); i++) {
		p_owner->add_tag(activation_owned_tags[i]);
	}

	GDVIRTUAL_CALL(_activate_ability, p_owner);
}

void AbilitySystemAbility::end_ability(AbilitySystemComponent *p_owner) {
	ERR_FAIL_NULL(p_owner);

	// Remove owned tags via ASC so tag_changed signal is emitted
	for (int i = 0; i < activation_owned_tags.size(); i++) {
		p_owner->remove_tag(activation_owned_tags[i]);
	}

	GDVIRTUAL_CALL(_on_end_ability, p_owner);
}

void AbilitySystemAbility::set_cost_effect(Ref<AbilitySystemEffect> p_effect) {
	cost_effect = p_effect;
}

Ref<AbilitySystemEffect> AbilitySystemAbility::get_cost_effect() const {
	return cost_effect;
}

void AbilitySystemAbility::set_cooldown_effect(Ref<AbilitySystemEffect> p_effect) {
	cooldown_effect = p_effect;
}

Ref<AbilitySystemEffect> AbilitySystemAbility::get_cooldown_effect() const {
	return cooldown_effect;
}

AbilitySystemAbility::AbilitySystemAbility() {
}

AbilitySystemAbility::~AbilitySystemAbility() {
}
