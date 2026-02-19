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
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/resources/ability_system_ability.h"
#include "modules/ability_system/resources/ability_system_ability_container.h"
#include "modules/ability_system/resources/ability_system_cue.h"
#include "modules/ability_system/resources/ability_system_effect.h"
#include "modules/ability_system/scene/ability_system_component.h"
#include "scene/gui/popup_menu.h"

// --- EditorPropertyGameplayTag ---

EditorPropertyGameplayTag::EditorPropertyGameplayTag() {
	container = memnew(HBoxContainer);
	container->set_h_size_flags(SIZE_EXPAND_FILL);
	add_child(container);

	tag_button = memnew(Button);
	tag_button->set_h_size_flags(SIZE_EXPAND_FILL);
	tag_button->set_clip_text(true);
	tag_button->connect("pressed", callable_mp(this, &EditorPropertyGameplayTag::_button_pressed));
	container->add_child(tag_button);
}

void EditorPropertyGameplayTag::update_property() {
	StringName val = get_edited_object()->get(get_edited_property());
	if (val == StringName()) {
		tag_button->set_text("[Empty]");
		tag_button->set_tooltip_text("No tag selected");
	} else {
		tag_button->set_text(val);
		tag_button->set_tooltip_text(val);
	}
}

void EditorPropertyGameplayTag::setup(const String &p_base_type) {
}

void EditorPropertyGameplayTag::_set_read_only(bool p_read_only) {
	tag_button->set_disabled(p_read_only);
}

void EditorPropertyGameplayTag::_button_pressed() {
	if (is_read_only()) {
		return;
	}

	PopupMenu *popup = memnew(PopupMenu);
	popup->connect("index_pressed", callable_mp(this, &EditorPropertyGameplayTag::_tag_selected).bind(popup));
	add_child(popup);

	TypedArray<StringName> tags = AbilitySystem::get_singleton()->get_registered_tags();
	tags.sort();

	popup->add_item("[Empty]", 0);
	popup->set_item_metadata(0, "");

	for (int i = 0; i < tags.size(); i++) {
		popup->add_item(tags[i]);
		popup->set_item_metadata(popup->get_item_count() - 1, tags[i]);
	}

	popup->popup_centered();
}

void EditorPropertyGameplayTag::_tag_selected(int p_idx, PopupMenu *p_popup) {
	if (p_popup) {
		StringName tag = p_popup->get_item_metadata(p_idx);
		emit_changed(get_edited_property(), tag);
		update_property();
		p_popup->queue_free();
	}
}

// --- AbilitySystemInspectorPlugin ---

bool AbilitySystemInspectorPlugin::can_handle(Object *p_object) {
	return Object::cast_to<AbilitySystemComponent>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemAbility>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemEffect>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemCue>(p_object) != nullptr ||
			Object::cast_to<AbilitySystemAbilityContainer>(p_object) != nullptr;
}

bool AbilitySystemInspectorPlugin::parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide) {
	if (p_type == Variant::STRING_NAME || p_type == Variant::STRING) {
		if (p_path.ends_with("_tag") || p_path == "tag") {
			EditorPropertyGameplayTag *editor = memnew(EditorPropertyGameplayTag);
			add_property_editor(p_path, editor);
			return true;
		}
	}

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
