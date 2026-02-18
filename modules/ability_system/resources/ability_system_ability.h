/**************************************************************************/
/*  ability_system_ability.h                                              */
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

#include "core/io/resource.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/variant/typed_array.h"

class AbilitySystemComponent;
class AbilitySystemEffect;

class AbilitySystemAbility : public Resource {
	GDCLASS(AbilitySystemAbility, Resource);

private:
	StringName ability_tag;
	TypedArray<StringName> activation_owned_tags;
	TypedArray<StringName> activation_required_tags;
	TypedArray<StringName> activation_blocked_tags;

	Ref<AbilitySystemEffect> cost_effect;
	Ref<AbilitySystemEffect> cooldown_effect;

protected:
	static void _bind_methods();

	// Lifecycle - Script-overridable
	GDVIRTUAL1(_activate_ability, Object *)
	GDVIRTUAL1RC(bool, _can_activate_ability, Object *)
	GDVIRTUAL1(_on_end_ability, Object *)

public:
	void set_ability_tag(const StringName &p_tag) { ability_tag = p_tag; }
	StringName get_ability_tag() const { return ability_tag; }

	void set_activation_owned_tags(const TypedArray<StringName> &p_tags) { activation_owned_tags = p_tags; }
	TypedArray<StringName> get_activation_owned_tags() const { return activation_owned_tags; }

	void set_activation_required_tags(const TypedArray<StringName> &p_tags) { activation_required_tags = p_tags; }
	TypedArray<StringName> get_activation_required_tags() const { return activation_required_tags; }

	void set_activation_blocked_tags(const TypedArray<StringName> &p_tags) { activation_blocked_tags = p_tags; }
	TypedArray<StringName> get_activation_blocked_tags() const { return activation_blocked_tags; }

	void set_cost_effect(Ref<AbilitySystemEffect> p_effect);
	Ref<AbilitySystemEffect> get_cost_effect() const;

	void set_cooldown_effect(Ref<AbilitySystemEffect> p_effect);
	Ref<AbilitySystemEffect> get_cooldown_effect() const;

	bool can_activate_ability(AbilitySystemComponent *p_owner) const;
	void activate_ability(AbilitySystemComponent *p_owner);
	void end_ability(AbilitySystemComponent *p_owner);

	AbilitySystemAbility();
	~AbilitySystemAbility();
};
