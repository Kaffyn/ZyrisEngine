/**************************************************************************/
/*  ability_system_editor_plugin.cpp                                      */
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

#include "ability_system_editor_plugin.h"
#include "modules/ability_system/resources/ability_system_ability.h"
#include "modules/ability_system/resources/ability_system_effect.h"
#include "modules/ability_system/scene/ability_system_component.h"

// --- AbilitySystemInspectorPlugin ---

bool AbilitySystemInspectorPlugin::can_handle(Object *p_object) {
	// The inspector plugin handles the component and the resource-based specs
	return Object::cast_to<AbilitySystemComponent>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemAbility>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemEffect>(p_object) != nullptr;
}

bool AbilitySystemInspectorPlugin::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	// The logic here will be to identify tags and attributes names
	// and instantiate specialized EditorProperty implementations.

	// Example scope detection for future implementation:
	// if (property_is_tag(p_path)) { add_property_editor(p_path, memnew(EditorPropertyAbilitySystemTag)); return true; }

	return false;
}

// --- AbilitySystemEditorPlugin ---

AbilitySystemEditorPlugin::AbilitySystemEditorPlugin() {
	Ref<AbilitySystemInspectorPlugin> inspector_plugin;
	inspector_plugin.instantiate();
	add_inspector_plugin(inspector_plugin);
}

AbilitySystemEditorPlugin::~AbilitySystemEditorPlugin() {
}
