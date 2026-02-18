/**************************************************************************/
/*  attribute_set.cpp                                                     */
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

#include "modules/gas/resources/attribute_set.h"

void AttributeSet::register_attribute(const StringName &p_name, real_t p_initial_value) {
	AttributeValue val;
	val.base_value = p_initial_value;
	val.current_value = p_initial_value;
	attributes[p_name] = val;
}

real_t AttributeSet::get_base_value(const StringName &p_name) const {
	ERR_FAIL_COND_V(!attributes.has(p_name), 0.0);
	return attributes[p_name].base_value;
}

void AttributeSet::set_base_value(const StringName &p_name, real_t p_value) {
	ERR_FAIL_COND(!attributes.has(p_name));
	attributes[p_name].base_value = p_value;
	// Recalculate current if needed, or just set it for now
	attributes[p_name].current_value = p_value;
}

real_t AttributeSet::get_current_value(const StringName &p_name) const {
	ERR_FAIL_COND_V(!attributes.has(p_name), 0.0);
	return attributes[p_name].current_value;
}

void AttributeSet::set_current_value(const StringName &p_name, real_t p_value) {
	ERR_FAIL_COND(!attributes.has(p_name));

	real_t old_value = attributes[p_name].current_value;
	if (old_value == p_value) return;

	if (!emit_attribute_pre_change(p_name, old_value, p_value)) {
		return; // Change cancelled
	}

	attributes[p_name].current_value = p_value;
	emit_attribute_changed(p_name, old_value, p_value);
}

bool AttributeSet::has_attribute(const StringName &p_name) const {
	return attributes.has(p_name);
}

void AttributeSet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_attribute", "name", "initial_value"), &AttributeSet::register_attribute);
	ClassDB::bind_method(D_METHOD("get_base_value", "name"), &AttributeSet::get_base_value);
	ClassDB::bind_method(D_METHOD("set_base_value", "name", "value"), &AttributeSet::set_base_value);
	ClassDB::bind_method(D_METHOD("get_current_value", "name"), &AttributeSet::get_current_value);
	ClassDB::bind_method(D_METHOD("set_current_value", "name", "value"), &AttributeSet::set_current_value);
	ClassDB::bind_method(D_METHOD("has_attribute", "name"), &AttributeSet::has_attribute);

	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	// attribute_pre_change is internal/C++ primarily, but we can expose it if needed. For now let's keep it simple.
	ADD_SIGNAL(MethodInfo("attribute_pre_change", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
}

void AttributeSet::emit_attribute_changed(const StringName &p_name, real_t p_old_value, real_t p_new_value) {
	emit_signal("attribute_changed", p_name, p_old_value, p_new_value);
}

bool AttributeSet::emit_attribute_pre_change(const StringName &p_name, real_t p_old_value, real_t p_new_value) {
	// In the future this could be a virtual method or involve more complex logic
	// For now, it just emits a signal. Note that GDScript signals don't return values directly like this.
	// So "cancelling" via signal return isn't straightforward in standard Godot signals without a reference object.
	// However, C++ logic can use this.
	emit_signal("attribute_pre_change", p_name, p_old_value, p_new_value);
	return true;
}
