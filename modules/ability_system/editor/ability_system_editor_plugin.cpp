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
#include "core/io/resource_loader.h"
#include "editor/editor_file_system.h"
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/resources/ability_system_ability.h"
#include "modules/ability_system/resources/ability_system_ability_container.h"
#include "modules/ability_system/resources/ability_system_attribute.h"
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

	if (!search_dialog) {
		search_dialog = memnew(TagSearchDialog);
		search_dialog->connect("confirmed", callable_mp(this, &EditorPropertyGameplayTag::_tag_selected));
		add_child(search_dialog);
	}

	search_dialog->setup(AbilitySystem::get_singleton()->get_registered_tags());
	search_dialog->popup_centered(Size2(400, 500));
}

void EditorPropertyGameplayTag::_tag_selected() {
	String tag = search_dialog->get_selected_tag();
	emit_changed(get_edited_property(), tag);
	update_property();
}

// --- TagSearchDialog ---

EditorPropertyGameplayTag::TagSearchDialog::TagSearchDialog() {
	set_title("Select Gameplay Tag");
	VBoxContainer *vbox = memnew(VBoxContainer);
	add_child(vbox);

	search_box = memnew(LineEdit);
	search_box->set_placeholder("Search tags...");
	search_box->connect("text_changed", callable_mp(this, &TagSearchDialog::_text_changed));
	vbox->add_child(search_box);

	tag_tree = memnew(Tree);
	tag_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tag_tree->set_hide_root(true);
	tag_tree->connect("item_activated", callable_mp(this, &TagSearchDialog::_activated));
	tag_tree->connect("cell_selected", callable_mp(this, &TagSearchDialog::_item_selected));
	vbox->add_child(tag_tree);

	get_ok_button()->set_text("Select");
}

void EditorPropertyGameplayTag::TagSearchDialog::setup(const TypedArray<StringName> &p_tags) {
	tag_tree->clear();
	TreeItem *root = tag_tree->create_item();

	TypedArray<StringName> sorted_tags = p_tags;
	sorted_tags.sort();

	String filter = search_box->get_text().to_lower();

	for (int i = 0; i < sorted_tags.size(); i++) {
		String tag = sorted_tags[i];
		if (filter.is_empty() || tag.to_lower().contains(filter)) {
			TreeItem *item = tag_tree->create_item(root);
			item->set_text(0, tag);
			item->set_metadata(0, tag);
		}
	}
}

void EditorPropertyGameplayTag::TagSearchDialog::_text_changed(const String &p_text) {
	setup(AbilitySystem::get_singleton()->get_registered_tags());
}

void EditorPropertyGameplayTag::TagSearchDialog::_item_selected() {
	TreeItem *item = tag_tree->get_selected();
	if (item) {
		selected_tag = item->get_metadata(0);
	}
}

void EditorPropertyGameplayTag::TagSearchDialog::_activated() {
	_item_selected();
	hide();
	emit_signal("confirmed");
}

// --- EditorPropertyAbilityAttribute ---

EditorPropertyAbilityAttribute::EditorPropertyAbilityAttribute() {
	container = memnew(HBoxContainer);
	container->set_h_size_flags(SIZE_EXPAND_FILL);
	add_child(container);

	attr_button = memnew(Button);
	attr_button->set_h_size_flags(SIZE_EXPAND_FILL);
	attr_button->set_clip_text(true);
	attr_button->connect("pressed", callable_mp(this, &EditorPropertyAbilityAttribute::_button_pressed));
	container->add_child(attr_button);
}

void EditorPropertyAbilityAttribute::update_property() {
	StringName val = get_edited_object()->get(get_edited_property());
	if (val == StringName()) {
		attr_button->set_text("[Empty]");
		attr_button->set_tooltip_text("No attribute selected");
	} else {
		attr_button->set_text(val);
		attr_button->set_tooltip_text(val);
	}
}

void EditorPropertyAbilityAttribute::_set_read_only(bool p_read_only) {
	attr_button->set_disabled(p_read_only);
}

void EditorPropertyAbilityAttribute::_button_pressed() {
	if (is_read_only()) {
		return;
	}

	PopupMenu *popup = memnew(PopupMenu);
	popup->connect("index_pressed", callable_mp(this, &EditorPropertyAbilityAttribute::_attribute_selected).bind(popup));
	add_child(popup);

	TypedArray<StringName> attrs = AbilitySystem::get_singleton()->get_registered_attributes();
	attrs.sort();

	popup->add_item("[Empty]", 0);
	popup->set_item_metadata(0, "");

	for (int i = 0; i < attrs.size(); i++) {
		popup->add_item(attrs[i]);
		popup->set_item_metadata(popup->get_item_count() - 1, attrs[i]);
	}

	popup->add_separator();
	popup->add_item("Add New Attribute...", 100);
	popup->set_item_metadata(popup->get_item_count() - 1, "ACTION_CREATE_NEW");

	popup->popup_centered();
}

void EditorPropertyAbilityAttribute::_attribute_selected(int p_idx, PopupMenu *p_popup) {
	if (p_popup) {
		Variant meta = p_popup->get_item_metadata(p_idx);
		if (meta == "ACTION_CREATE_NEW") {
			if (!create_dialog) {
				create_dialog = memnew(AttributeCreateDialog);
				create_dialog->connect("confirmed", callable_mp(this, &EditorPropertyAbilityAttribute::_create_new_attribute));
				add_child(create_dialog);
			}
			create_dialog->popup_centered();
		} else {
			StringName attr = meta;
			emit_changed(get_edited_property(), attr);
			update_property();
		}
		p_popup->queue_free();
	}
}

void EditorPropertyAbilityAttribute::_create_new_attribute() {
	String name = create_dialog->get_attribute_name();
	if (!name.is_empty()) {
		AbilitySystem::get_singleton()->register_attribute(name);
		emit_changed(get_edited_property(), name);
		update_property();
	}
}

// --- AttributeCreateDialog ---

EditorPropertyAbilityAttribute::AttributeCreateDialog::AttributeCreateDialog() {
	set_title("Create New Attribute");
	VBoxContainer *vbox = memnew(VBoxContainer);
	add_child(vbox);

	Label *label = memnew(Label);
	label->set_text("Attribute Name:");
	vbox->add_child(label);

	name_edit = memnew(LineEdit);
	vbox->add_child(name_edit);

	register_text_enter(name_edit);
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

		if (p_path.ends_with("attribute") || p_path == "attribute_name") {
			EditorPropertyAbilityAttribute *editor = memnew(EditorPropertyAbilityAttribute);
			add_property_editor(p_path, editor);
			return true;
		}
	}

	return false;
}

// --- AbilitySystemEditorPlugin ---

void AbilitySystemEditorPlugin::edit(Object *p_object) {
	AbilitySystemComponent *asc = Object::cast_to<AbilitySystemComponent>(p_object);
	if (asc) {
		dashboard->set_asc(asc);
		make_bottom_panel_item_visible(dashboard);
	}
}

bool AbilitySystemEditorPlugin::handles(Object *p_object) const {
	return Object::cast_to<AbilitySystemComponent>(p_object) != nullptr;
}

AbilitySystemEditorPlugin::AbilitySystemEditorPlugin() {
	Ref<AbilitySystemInspectorPlugin> inspector_plugin;
	inspector_plugin.instantiate();
	add_inspector_plugin(inspector_plugin);

	dashboard = memnew(AbilitySystemDashboard);
	add_control_to_bottom_panel(dashboard, "AbilitySystem");

	archetype_editor = memnew(AbilitySystemArchetypeEditor);
	get_editor_interface()->get_base_control()->add_child(archetype_editor);

	EditorFileSystem::get_singleton()->connect("filesystem_changed", callable_mp(this, &AbilitySystemEditorPlugin::_rescan_resources));
	EditorFileSystem::get_singleton()->connect("resources_reload_finished", callable_mp(this, &AbilitySystemEditorPlugin::_rescan_resources));

	_rescan_resources();
}

void AbilitySystemEditorPlugin::_rescan_resources() {
	HashSet<StringName> tags;
	HashSet<StringName> attrs;

	_scan_dir(EditorFileSystem::get_singleton()->get_filesystem(), tags, attrs);

	// Sync with singleton
	AbilitySystem *as = AbilitySystem::get_singleton();
	if (as) {
		// Update tags
		for (const StringName &E : tags) {
			as->register_tag(E);
		}
		// Update attributes
		for (const StringName &E : attrs) {
			as->register_attribute(E);
		}
	}
}

void AbilitySystemEditorPlugin::_scan_dir(EditorFileSystemDirectory *p_dir, HashSet<StringName> &r_tags, HashSet<StringName> &r_attrs) {
	if (!p_dir) {
		return;
	}

	for (int i = 0; i < p_dir->get_subdir_count(); i++) {
		_scan_dir(p_dir->get_subdir(i), r_tags, r_attrs);
	}

	for (int i = 0; i < p_dir->get_file_count(); i++) {
		String type = p_dir->get_file_type(i);
		if (type == "AbilitySystemAttribute" || type == "AbilitySystemCue") {
			Ref<Resource> res = ResourceLoader::load(p_dir->get_file_path(i));
			if (res.is_valid()) {
				if (type == "AbilitySystemAttribute") {
					Ref<AbilitySystemAttribute> attr = res;
					if (attr->get_attribute_name() != StringName()) {
						r_attrs.insert(attr->get_attribute_name());
					}
				} else if (type == "AbilitySystemCue") {
					Ref<AbilitySystemCue> cue = res;
					if (cue->get_cue_tag() != StringName()) {
						r_tags.insert(cue->get_cue_tag());
					}
				}
			}
		}
	}
}

AbilitySystemEditorPlugin::~AbilitySystemEditorPlugin() {
}

// --- AbilitySystemDashboard ---

AbilitySystemDashboard::AbilitySystemDashboard() {
	set_custom_minimum_size(Size2(0, 200));

	HBoxContainer *header = memnew(HBoxContainer);
	add_child(header);

	header_label = memnew(Label);
	header_label->set_text("Ability System Debugger [No ASC Selected]");
	header->add_child(header_label);

	debug_tree = memnew(Tree);
	debug_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	debug_tree->set_columns(2);
	debug_tree->set_column_title(0, "Property");
	debug_tree->set_column_title(1, "Value");
	debug_tree->set_column_titles_visible(true);
	add_child(debug_tree);

	set_process(true);
}

void AbilitySystemDashboard::set_asc(AbilitySystemComponent *p_asc) {
	current_asc = p_asc;
	if (current_asc) {
		header_label->set_text(vformat("Ability System Debugger [%s]", current_asc->get_name()));
	} else {
		header_label->set_text("Ability System Debugger [No ASC Selected]");
	}
	_update_debug_view();
}

void AbilitySystemDashboard::_notification(int p_what) {
	if (p_what == NOTIFICATION_PROCESS) {
		_update_debug_view();
	}
}

void AbilitySystemDashboard::_update_debug_view() {
	if (!current_asc || !is_visible_in_tree()) {
		return;
	}

	debug_tree->clear();
	TreeItem *root = debug_tree->create_item();

	// 1. Attributes
	TreeItem *attr_root = debug_tree->create_item(root);
	attr_root->set_text(0, "Attributes");
	attr_root->set_selectable(0, false);

	// Use unified net state for attributes and tags.
	Dictionary state = current_asc->get_net_state();
	if (state.has("attributes")) {
		Dictionary attrs = state["attributes"];
		Array keys = attrs.keys();
		for (int i = 0; i < keys.size(); i++) {
			TreeItem *item = debug_tree->create_item(attr_root);
			item->set_text(0, keys[i]);
			item->set_text(1, String::num(current_asc->get_attribute_value(keys[i]), 2));
		}
	}

	// 2. Tags
	TreeItem *tag_root = debug_tree->create_item(root);
	tag_root->set_text(0, "Tags");
	tag_root->set_selectable(0, false);

	TypedArray<StringName> tags = current_asc->get_owned_tags()->get_all_tags();
	for (int i = 0; i < tags.size(); i++) {
		TreeItem *item = debug_tree->create_item(tag_root);
		item->set_text(0, tags[i]);
	}

	// 3. Active Effects
	TreeItem *effect_root = debug_tree->create_item(root);
	effect_root->set_text(0, "Active Effects");
	effect_root->set_selectable(0, false);

	TypedArray<AbilitySystemEffectSpec> effects = current_asc->get_active_effects_debug();
	for (int i = 0; i < effects.size(); i++) {
		Ref<AbilitySystemEffectSpec> spec = effects[i];
		TreeItem *item = debug_tree->create_item(effect_root);
		item->set_text(0, spec->get_effect()->get_path().get_file());
		item->set_text(1, vformat("Duration: %.2f", spec->get_duration_remaining()));
	}
}

// --- AbilitySystemArchetypeEditor ---

AbilitySystemArchetypeEditor::AbilitySystemArchetypeEditor() {
	set_title("Visual Archetype Editor");
	set_custom_minimum_size(Size2(600, 400));

	main_vbox = memnew(VBoxContainer);
	add_child(main_vbox);

	HBoxContainer *toolbar = memnew(HBoxContainer);
	main_vbox->add_child(toolbar);

	Button *add_attr = memnew(Button);
	add_attr->set_text("Add Attribute");
	add_attr->connect("pressed", callable_mp(this, &AbilitySystemArchetypeEditor::_add_attribute));
	toolbar->add_child(add_attr);

	stats_tree = memnew(Tree);
	stats_tree->set_v_size_flags(SIZE_EXPAND_FILL);
	stats_tree->set_columns(2);
	stats_tree->set_column_title(0, "Attribute");
	stats_tree->set_column_title(1, "Base Value");
	stats_tree->set_column_titles_visible(true);
	main_vbox->add_child(stats_tree);
}

void AbilitySystemArchetypeEditor::set_container(Ref<AbilitySystemAbilityContainer> p_container) {
	container = p_container;
	_update_view();
}

void AbilitySystemArchetypeEditor::_update_view() {
	if (container.is_null()) {
		return;
	}

	stats_tree->clear();
	TreeItem *root = stats_tree->create_item();

	Dictionary attrs = container->get_initial_attributes();
	Array keys = attrs.keys();
	for (int i = 0; i < keys.size(); i++) {
		TreeItem *item = stats_tree->create_item(root);
		item->set_text(0, keys[i]);
		item->set_cell_mode(1, TreeItem::CELL_MODE_RANGE);
		item->set_range_config(1, 0, 9999, 1);
		item->set_range(1, attrs[keys[i]]);
		item->set_editable(1, true);
		item->set_metadata(0, keys[i]);
	}
}

void AbilitySystemArchetypeEditor::_add_attribute() {
	PopupMenu *popup = memnew(PopupMenu);
	TypedArray<StringName> attrs = AbilitySystem::get_singleton()->get_registered_attributes();
	for (int i = 0; i < attrs.size(); i++) {
		popup->add_item(attrs[i]);
	}
	add_child(popup);
	popup->popup_centered();
}
