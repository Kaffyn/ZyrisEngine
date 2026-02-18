/**************************************************************************/
/*  gameplay_effect.cpp                                                   */
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

#include "modules/gas/resources/gameplay_effect.h"

void GameplayEffect::add_modifier(const StringName &p_attribute, GameplayEffectModifierOp p_op, real_t p_value) {
	Modifier mod;
	mod.attribute = p_attribute;
	mod.op = p_op;
	mod.value = p_value;
	modifiers.push_back(mod);
}

void GameplayEffect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_duration_type", "type"), &GameplayEffect::set_duration_type);
	ClassDB::bind_method(D_METHOD("get_duration_type"), &GameplayEffect::get_duration_type);
	ClassDB::bind_method(D_METHOD("set_duration", "duration"), &GameplayEffect::set_duration);
	ClassDB::bind_method(D_METHOD("get_duration"), &GameplayEffect::get_duration);
	ClassDB::bind_method(D_METHOD("add_modifier", "attribute", "op", "value"), &GameplayEffect::add_modifier);
	ClassDB::bind_method(D_METHOD("set_granted_tags", "tags"), &GameplayEffect::set_granted_tags);
	ClassDB::bind_method(D_METHOD("get_granted_tags"), &GameplayEffect::get_granted_tags);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "duration_type", PROPERTY_HINT_ENUM, "Instant,Has Duration,Infinite"), "set_duration_type", "get_duration_type");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "granted_tags", PROPERTY_HINT_RESOURCE_TYPE, "GameplayTagContainer"), "set_granted_tags", "get_granted_tags");

	BIND_ENUM_CONSTANT(GE_DURATION_INSTANT);
	BIND_ENUM_CONSTANT(GE_DURATION_HAS_DURATION);
	BIND_ENUM_CONSTANT(GE_DURATION_INFINITE);

	BIND_ENUM_CONSTANT(GE_MOD_ADD);
	BIND_ENUM_CONSTANT(GE_MOD_MULTIPLY);
	BIND_ENUM_CONSTANT(GE_MOD_DIVIDE);
	BIND_ENUM_CONSTANT(GE_MOD_OVERRIDE);
}
