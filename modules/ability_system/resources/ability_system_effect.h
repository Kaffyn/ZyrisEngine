/**************************************************************************/
/*  ability_system_effect.h                                               */
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
#include "core/templates/vector.h"
#include "core/variant/typed_array.h"

class AbilitySystemEffect : public Resource {
	GDCLASS(AbilitySystemEffect, Resource);

public:
	enum DurationPolicy {
		INSTANT,
		DURATION,
		INFINITE
	};

	enum ModifierOp {
		ADD,
		MULTIPLY,
		DIVIDE,
		OVERRIDE
	};

	struct Modifier {
		StringName attribute;
		ModifierOp operation = ADD;
		float magnitude = 0.0f;
	};

protected:
	static void _bind_methods();
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

private:
	DurationPolicy duration_policy = INSTANT;
	float duration_magnitude = 0.0f;

	struct ModifierData {
		StringName attribute;
		ModifierOp operation;
		float magnitude;
		Ref<class AbilitySystemMagnitudeCalculation> custom_magnitude;
	};
	Vector<ModifierData> modifiers;

	TypedArray<StringName> granted_tags;
	TypedArray<StringName> blocked_tags;
	TypedArray<StringName> removed_tags;
	TypedArray<StringName> cue_tags;

public:
	void set_duration_policy(DurationPolicy p_policy) { duration_policy = p_policy; }
	DurationPolicy get_duration_policy() const { return duration_policy; }

	void set_duration_magnitude(float p_mag) { duration_magnitude = p_mag; }
	float get_duration_magnitude() const { return duration_magnitude; }

	void add_modifier(const StringName &p_attr, ModifierOp p_op, float p_mag, Ref<class AbilitySystemMagnitudeCalculation> p_custom_mag = nullptr);
	int get_modifier_count() const { return modifiers.size(); }
	StringName get_modifier_attribute(int p_idx) const;
	ModifierOp get_modifier_operation(int p_idx) const;
	float get_modifier_magnitude(int p_idx) const;
	Ref<class AbilitySystemMagnitudeCalculation> get_modifier_custom_magnitude(int p_idx) const;

	void set_granted_tags(const TypedArray<StringName> &p_tags) { granted_tags = p_tags; }
	TypedArray<StringName> get_granted_tags() const { return granted_tags; }

	void set_blocked_tags(const TypedArray<StringName> &p_tags) { blocked_tags = p_tags; }
	TypedArray<StringName> get_blocked_tags() const { return blocked_tags; }

	void set_removed_tags(const TypedArray<StringName> &p_tags) { removed_tags = p_tags; }
	TypedArray<StringName> get_removed_tags() const { return removed_tags; }

	void set_cue_tags(const TypedArray<StringName> &p_tags) { cue_tags = p_tags; }
	TypedArray<StringName> get_cue_tags() const { return cue_tags; }

	void set_modifiers_count(int p_count);
	int get_modifiers_count() const;

	AbilitySystemEffect();
	~AbilitySystemEffect();
};

VARIANT_ENUM_CAST(AbilitySystemEffect::DurationPolicy);
VARIANT_ENUM_CAST(AbilitySystemEffect::ModifierOp);
