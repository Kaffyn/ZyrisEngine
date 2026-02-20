/**************************************************************************/
/*  ability_system_attribute.cpp                                          */
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

#include "ability_system_attribute.h"

void AbilitySystemAttribute::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_attribute_name", "name"), &AbilitySystemAttribute::set_attribute_name);
	ClassDB::bind_method(D_METHOD("get_attribute_name"), &AbilitySystemAttribute::get_attribute_name);

	ClassDB::bind_method(D_METHOD("set_display_name", "name"), &AbilitySystemAttribute::set_display_name);
	ClassDB::bind_method(D_METHOD("get_display_name"), &AbilitySystemAttribute::get_display_name);

	ClassDB::bind_method(D_METHOD("set_description", "description"), &AbilitySystemAttribute::set_description);
	ClassDB::bind_method(D_METHOD("get_description"), &AbilitySystemAttribute::get_description);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "attribute_name"), "set_attribute_name", "get_attribute_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "display_name"), "set_display_name", "get_display_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "description", PROPERTY_HINT_MULTILINE_TEXT), "set_description", "get_description");
}

AbilitySystemAttribute::AbilitySystemAttribute() {
}
