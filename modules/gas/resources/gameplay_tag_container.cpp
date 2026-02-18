/**************************************************************************/
/*  gameplay_tag_container.cpp                                            */
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

#include "modules/gas/core/gameplay_tag_container.h"
#include "core/variant/typed_array.h"
#include "core/variant/variant.h"

void GameplayTagContainer::add_tag(const StringName &p_tag) {
	if (p_tag == StringName()) {
		return;
	}
	tags.insert(p_tag);
}

void GameplayTagContainer::remove_tag(const StringName &p_tag) {
	tags.erase(p_tag);
}

bool GameplayTagContainer::has_tag(const StringName &p_tag) const {
	return tags.has(p_tag);
}

bool GameplayTagContainer::has_tag_hierarchical(const StringName &p_tag) const {
	if (tags.has(p_tag)) {
		return true;
	}

	Ref<GameplayTag> other;
	other.instantiate();
	other->set_name(p_tag);

	for (const StringName &tag_name : tags) {
		Ref<GameplayTag> current;
		current.instantiate();
		current->set_name(tag_name);
		if (current->matches(other)) {
			return true;
		}
	}

	return false;
}

void GameplayTagContainer::add_container(const Ref<GameplayTagContainer> &p_container) {
	ERR_FAIL_COND(p_container.is_null());
	for (const StringName &tag_name : p_container->tags) {
		tags.insert(tag_name);
	}
}

bool GameplayTagContainer::has_any(const Ref<GameplayTagContainer> &p_container) const {
	ERR_FAIL_COND_V(p_container.is_null(), false);
	for (const StringName &tag_name : p_container->tags) {
		if (has_tag_hierarchical(tag_name)) {
			return true;
		}
	}
	return false;
}

bool GameplayTagContainer::has_all(const Ref<GameplayTagContainer> &p_container) const {
	ERR_FAIL_COND_V(p_container.is_null(), false);
	for (const StringName &tag_name : p_container->tags) {
		if (!has_tag_hierarchical(tag_name)) {
			return false;
		}
	}
	return true;
}

TypedArray<StringName> GameplayTagContainer::get_tags() const {
	TypedArray<StringName> res;
	for (const StringName &tag_name : tags) {
		res.push_back(tag_name);
	}
	return res;
}

void GameplayTagContainer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_tag", "tag"), &GameplayTagContainer::add_tag);
	ClassDB::bind_method(D_METHOD("remove_tag", "tag"), &GameplayTagContainer::remove_tag);
	ClassDB::bind_method(D_METHOD("has_tag", "tag"), &GameplayTagContainer::has_tag);
	ClassDB::bind_method(D_METHOD("has_tag_hierarchical", "tag"), &GameplayTagContainer::has_tag_hierarchical);
	ClassDB::bind_method(D_METHOD("add_container", "container"), &GameplayTagContainer::add_container);
	ClassDB::bind_method(D_METHOD("has_any", "container"), &GameplayTagContainer::has_any);
	ClassDB::bind_method(D_METHOD("has_all", "container"), &GameplayTagContainer::has_all);
	ClassDB::bind_method(D_METHOD("get_tags"), &GameplayTagContainer::get_tags);
}
