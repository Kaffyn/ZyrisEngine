/**************************************************************************/
/*  ability_system.cpp                                                    */
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

#include "ability_system.h"
#include "core/config/project_settings.h"

AbilitySystem *AbilitySystem::singleton = nullptr;

void AbilitySystem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("register_tag", "tag"), &AbilitySystem::register_tag);
	ClassDB::bind_method(D_METHOD("is_tag_registered", "tag"), &AbilitySystem::is_tag_registered);
	ClassDB::bind_method(D_METHOD("unregister_tag", "tag"), &AbilitySystem::unregister_tag);
	ClassDB::bind_method(D_METHOD("get_registered_tags"), &AbilitySystem::get_registered_tags);

	ClassDB::bind_method(D_METHOD("register_attribute", "attribute"), &AbilitySystem::register_attribute);
	ClassDB::bind_method(D_METHOD("is_attribute_registered", "attribute"), &AbilitySystem::is_attribute_registered);
	ClassDB::bind_method(D_METHOD("unregister_attribute", "attribute"), &AbilitySystem::unregister_attribute);
	ClassDB::bind_method(D_METHOD("get_registered_attributes"), &AbilitySystem::get_registered_attributes);

	ClassDB::bind_static_method("AbilitySystem", D_METHOD("tag_matches", "tag", "match_against", "exact"), &AbilitySystem::tag_matches, DEFVAL(false));
}

void AbilitySystem::register_tag(const StringName &p_tag) {
	if (!registered_tags.has(p_tag)) {
		registered_tags.insert(p_tag);
		_save_to_settings();
	}
}

bool AbilitySystem::is_tag_registered(const StringName &p_tag) const {
	return registered_tags.has(p_tag);
}

void AbilitySystem::unregister_tag(const StringName &p_tag) {
	if (registered_tags.has(p_tag)) {
		registered_tags.erase(p_tag);
		_save_to_settings();
	}
}

void AbilitySystem::register_attribute(const StringName &p_attr) {
	if (!registered_attributes.has(p_attr)) {
		registered_attributes.insert(p_attr);
		_save_to_settings();
	}
}

bool AbilitySystem::is_attribute_registered(const StringName &p_attr) const {
	return registered_attributes.has(p_attr);
}

void AbilitySystem::unregister_attribute(const StringName &p_attr) {
	if (registered_attributes.has(p_attr)) {
		registered_attributes.erase(p_attr);
		_save_to_settings();
	}
}

TypedArray<StringName> AbilitySystem::get_registered_attributes() const {
	TypedArray<StringName> res;
	for (const StringName &E : registered_attributes) {
		res.push_back(E);
	}
	return res;
}

TypedArray<StringName> AbilitySystem::get_registered_tags() const {
	TypedArray<StringName> res;
	for (const StringName &E : registered_tags) {
		res.push_back(E);
	}
	return res;
}

bool AbilitySystem::tag_matches(const StringName &p_tag, const StringName &p_match_against, bool p_exact) {
	if (p_tag == p_match_against) {
		return true;
	}

	if (p_exact) {
		return false;
	}

	String s_tag = p_tag;
	String s_match = p_match_against;

	if (s_tag.begins_with(s_match + ".")) {
		return true;
	}

	return false;
}

void AbilitySystem::_save_to_settings() {
	TypedArray<StringName> tags = get_registered_tags();
	TypedArray<StringName> attributes = get_registered_attributes();

	ProjectSettings::get_singleton()->set_setting("ability_system/config/tags", tags);
	ProjectSettings::get_singleton()->set_setting("ability_system/config/attributes", attributes);
	ProjectSettings::get_singleton()->save();
}

void AbilitySystem::_load_from_settings() {
	if (ProjectSettings::get_singleton()->has_setting("ability_system/config/tags")) {
		TypedArray<StringName> tags = ProjectSettings::get_singleton()->get_setting("ability_system/config/tags");
		for (int i = 0; i < tags.size(); i++) {
			registered_tags.insert(tags[i]);
		}
	}

	if (ProjectSettings::get_singleton()->has_setting("ability_system/config/attributes")) {
		TypedArray<StringName> attributes = ProjectSettings::get_singleton()->get_setting("ability_system/config/attributes");
		for (int i = 0; i < attributes.size(); i++) {
			registered_attributes.insert(attributes[i]);
		}
	}
}

AbilitySystem::AbilitySystem() {
	singleton = this;
	_load_from_settings();
}

AbilitySystem::~AbilitySystem() {
	singleton = nullptr;
}
