/**************************************************************************/
/*  gameplay_effect.h                                                     */
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
#include "modules/gas/core/gameplay_tag_container.h"

enum GameplayEffectDurationType {
	GE_DURATION_INSTANT,
	GE_DURATION_HAS_DURATION,
	GE_DURATION_INFINITE
};

enum GameplayEffectModifierOp {
	GE_MOD_ADD,
	GE_MOD_MULTIPLY,
	GE_MOD_DIVIDE,
	GE_MOD_OVERRIDE
};

class GameplayEffect : public Resource {
	GDCLASS(GameplayEffect, Resource);

public:
	struct Modifier {
		StringName attribute;
		GameplayEffectModifierOp op = GE_MOD_ADD;
		real_t value = 0.0;
	};

private:
	GameplayEffectDurationType duration_type = GE_DURATION_INSTANT;
	real_t duration = 0.0;
	Vector<Modifier> modifiers;

	Ref<GameplayTagContainer> granted_tags;

protected:
	static void _bind_methods();

public:
	void set_duration_type(GameplayEffectDurationType p_type) { duration_type = p_type; }
	GameplayEffectDurationType get_duration_type() const { return duration_type; }

	void set_duration(real_t p_duration) { duration = p_duration; }
	real_t get_duration() const { return duration; }

	void add_modifier(const StringName &p_attribute, GameplayEffectModifierOp p_op, real_t p_value);
	int get_modifier_count() const { return modifiers.size(); }
	StringName get_modifier_attribute(int p_idx) const { return modifiers[p_idx].attribute; }
	GameplayEffectModifierOp get_modifier_op(int p_idx) const { return modifiers[p_idx].op; }
	real_t get_modifier_value(int p_idx) const { return modifiers[p_idx].value; }

	void set_granted_tags(const Ref<GameplayTagContainer> &p_tags) { granted_tags = p_tags; }
	Ref<GameplayTagContainer> get_granted_tags() const { return granted_tags; }
};

VARIANT_ENUM_CAST(GameplayEffectDurationType);
VARIANT_ENUM_CAST(GameplayEffectModifierOp);
