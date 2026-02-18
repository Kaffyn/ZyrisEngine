/**************************************************************************/
/*  ability_system_cue.cpp                                                */
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

#include "ability_system_cue.h"

void AbilitySystemCue::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_event_type", "type"), &AbilitySystemCue::set_event_type);
	ClassDB::bind_method(D_METHOD("get_event_type"), &AbilitySystemCue::get_event_type);
	ClassDB::bind_method(D_METHOD("set_cue_tag", "tag"), &AbilitySystemCue::set_cue_tag);
	ClassDB::bind_method(D_METHOD("get_cue_tag"), &AbilitySystemCue::get_cue_tag);

	GDVIRTUAL_BIND(_on_execute, "owner", "data");
	GDVIRTUAL_BIND(_on_active, "owner", "data");
	GDVIRTUAL_BIND(_on_remove, "owner", "data");

	ADD_PROPERTY(PropertyInfo(Variant::INT, "event_type", PROPERTY_HINT_ENUM, "OnExecute,OnActive,OnRemove"), "set_event_type", "get_event_type");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "cue_tag"), "set_cue_tag", "get_cue_tag");

	BIND_ENUM_CONSTANT(ON_EXECUTE);
	BIND_ENUM_CONSTANT(ON_ACTIVE);
	BIND_ENUM_CONSTANT(ON_REMOVE);
}

AbilitySystemCue::AbilitySystemCue() {
}

AbilitySystemCue::~AbilitySystemCue() {
}
