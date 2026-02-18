/**************************************************************************/
/*  ability_system_attribute_set.cpp                                      */
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

#include "ability_system_attribute_set.h"

void AbilitySystemAttributeSet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_attribute_base_value", "name", "value"), &AbilitySystemAttributeSet::set_attribute_base_value);
	ClassDB::bind_method(D_METHOD("get_attribute_base_value", "name"), &AbilitySystemAttributeSet::get_attribute_base_value);
	ClassDB::bind_method(D_METHOD("set_attribute_current_value", "name", "value"), &AbilitySystemAttributeSet::set_attribute_current_value);
	ClassDB::bind_method(D_METHOD("get_attribute_current_value", "name"), &AbilitySystemAttributeSet::get_attribute_current_value);
	ClassDB::bind_method(D_METHOD("has_attribute", "name"), &AbilitySystemAttributeSet::has_attribute);
	ClassDB::bind_method(D_METHOD("get_attribute_list"), &AbilitySystemAttributeSet::get_attribute_list);
	ClassDB::bind_method(D_METHOD("reset_current_values"), &AbilitySystemAttributeSet::reset_current_values);

	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	ADD_SIGNAL(MethodInfo("attribute_pre_change", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
}

void AbilitySystemAttributeSet::set_attribute_base_value(const StringName &p_name, float p_value) {
	float old_val = 0.0f;
	if (attributes.has(p_name)) {
		old_val = attributes[p_name].base_value;
		emit_signal("attribute_pre_change", p_name, old_val, p_value);
		attributes[p_name].base_value = p_value;
	} else {
		AttributeValue av;
		av.base_value = p_value;
		av.current_value = p_value;
		attributes[p_name] = av;
	}
	emit_signal("attribute_changed", p_name, old_val, p_value);
}

float AbilitySystemAttributeSet::get_attribute_base_value(const StringName &p_name) const {
	if (attributes.has(p_name)) {
		return attributes[p_name].base_value;
	}
	return 0.0f;
}

void AbilitySystemAttributeSet::set_attribute_current_value(const StringName &p_name, float p_value) {
	float old_val = 0.0f;
	if (attributes.has(p_name)) {
		old_val = attributes[p_name].current_value;
		emit_signal("attribute_pre_change", p_name, old_val, p_value);
		attributes[p_name].current_value = p_value;
	} else {
		AttributeValue av;
		av.base_value = p_value;
		av.current_value = p_value;
		attributes[p_name] = av;
	}
	emit_signal("attribute_changed", p_name, old_val, p_value);
}

float AbilitySystemAttributeSet::get_attribute_current_value(const StringName &p_name) const {
	if (attributes.has(p_name)) {
		return attributes[p_name].current_value;
	}
	return 0.0f;
}

bool AbilitySystemAttributeSet::has_attribute(const StringName &p_name) const {
	return attributes.has(p_name);
}

TypedArray<StringName> AbilitySystemAttributeSet::get_attribute_list() const {
	TypedArray<StringName> res;
	for (const KeyValue<StringName, AttributeValue> &E : attributes) {
		res.push_back(E.key);
	}
	return res;
}

void AbilitySystemAttributeSet::reset_current_values() {
	for (KeyValue<StringName, AttributeValue> &E : attributes) {
		E.value.current_value = E.value.base_value;
	}
}

AbilitySystemAttributeSet::AbilitySystemAttributeSet() {
}

AbilitySystemAttributeSet::~AbilitySystemAttributeSet() {
}
