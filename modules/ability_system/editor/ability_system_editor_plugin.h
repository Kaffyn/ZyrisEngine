/**************************************************************************/
/*  ability_system_editor_plugin.h                                        */
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

#include "editor/inspector/editor_inspector.h"
#include "editor/plugins/editor_plugin.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/tree.h"
#include "scene/gui/dialogs.h"

/**
 * EditorPropertyGameplayTag
 * Custom property editor for selecting Gameplay Tags from a list.
 */
class EditorPropertyGameplayTag : public EditorProperty {
	GDCLASS(EditorPropertyGameplayTag, EditorProperty);

	HBoxContainer *container = nullptr;
	Button *tag_button = nullptr;
	Button *edit_button = nullptr;

	class TagSearchDialog : public AcceptDialog {
		GDCLASS(TagSearchDialog, AcceptDialog);
		LineEdit *search_box = nullptr;
		Tree *tag_tree = nullptr;
		String selected_tag;

		void _text_changed(const String &p_text);
		void _item_selected();
		void _activated();

	public:
		void setup(const TypedArray<StringName> &p_tags);
		String get_selected_tag() const { return selected_tag; }
		TagSearchDialog();
	};

	TagSearchDialog *search_dialog = nullptr;

	void _tag_selected();
	void _button_pressed();

protected:
	virtual void _set_read_only(bool p_read_only) override;

public:
	virtual void update_property() override;
	void setup(const String &p_base_type);

	EditorPropertyGameplayTag();
};

/**
 * EditorPropertyAbilityAttribute
 * Custom property editor for selecting Ability Attributes from a list.
 */
class EditorPropertyAbilityAttribute : public EditorProperty {
	GDCLASS(EditorPropertyAbilityAttribute, EditorProperty);

	HBoxContainer *container = nullptr;
	Button *attr_button = nullptr;

	class AttributeCreateDialog : public ConfirmationDialog {
		GDCLASS(AttributeCreateDialog, ConfirmationDialog);
		LineEdit *name_edit = nullptr;

	public:
		String get_attribute_name() const { return name_edit->get_text(); }
		AttributeCreateDialog();
	};

	AttributeCreateDialog *create_dialog = nullptr;

	void _attribute_selected(int p_idx, PopupMenu *p_popup);
	void _button_pressed();
	void _create_new_attribute();

protected:
	virtual void _set_read_only(bool p_read_only) override;

public:
	virtual void update_property() override;

	EditorPropertyAbilityAttribute();
};


/**
 * AbilitySystemInspectorPlugin
 * Responsible for intercepting and customizing how Ability System properties
 * are shown in the Inspector.
 */
class AbilitySystemInspectorPlugin : public EditorInspectorPlugin {
	GDCLASS(AbilitySystemInspectorPlugin, EditorInspectorPlugin);

public:
	virtual bool can_handle(Object *p_object) override;
	virtual bool parse_property(Object *p_object, const Variant::Type p_type, const String &p_path, const PropertyHint p_hint, const String &p_hint_text, const BitField<PropertyUsageFlags> p_usage, const bool p_wide = false) override;
};

class AbilitySystemDashboard;
class AbilitySystemArchetypeEditor;

/**
 * AbilitySystemEditorPlugin
 * Entry point for all Editor-side customizations of the Ability System.
 */
class AbilitySystemEditorPlugin : public EditorPlugin {
	GDCLASS(AbilitySystemEditorPlugin, EditorPlugin);

	AbilitySystemDashboard *dashboard = nullptr;
	AbilitySystemArchetypeEditor *archetype_editor = nullptr;

protected:
	void _notification(int p_what);

public:
	virtual bool handles(Object *p_object) const override;
	virtual void edit(Object *p_object) override;
	virtual void make_visible(bool p_visible) override;
	virtual String get_plugin_name() const override { return "AbilitySystem"; }

	AbilitySystemEditorPlugin();
	~AbilitySystemEditorPlugin();
};

/**
 * AbilitySystemDashboard
 * Bottom panel for debugging AbilitySystemComponents in real-time.
 */
class AbilitySystemDashboard : public VBoxContainer {
	GDCLASS(AbilitySystemDashboard, VBoxContainer);

	AbilitySystemComponent *current_asc = nullptr;

	Tree *debug_tree = nullptr;
	Label *header_label = nullptr;

	void _update_debug_view();

public:
	void set_asc(AbilitySystemComponent *p_asc);
	AbilitySystemDashboard();
};

/**
 * AbilitySystemArchetypeEditor
 * Visual editor for AbilitySystemAbilityContainer resources.
 */
class AbilitySystemArchetypeEditor : public AcceptDialog {
	GDCLASS(AbilitySystemArchetypeEditor, AcceptDialog);

	Ref<AbilitySystemAbilityContainer> container;

	VBoxContainer *main_vbox = nullptr;
	Tree *stats_tree = nullptr;

	void _update_view();
	void _add_attribute();

public:
	void set_container(Ref<AbilitySystemAbilityContainer> p_container);
	AbilitySystemArchetypeEditor();
};


