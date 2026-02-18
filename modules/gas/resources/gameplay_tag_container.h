/**************************************************************************/
/*  gameplay_tag_container.h                                              */
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

#include "core/object/ref_counted.h"
#include "core/templates/rb_set.h"
#include "core/variant/typed_array.h"
#include "modules/gas/resources/gameplay_tag_container.h"

class GameplayTagContainer : public RefCounted {
	GDCLASS(GameplayTagContainer, RefCounted);

	RBSet<StringName> tags;

protected:
	static void _bind_methods();

public:
	void add_tag(const StringName &p_tag);
	void remove_tag(const StringName &p_tag);
	bool has_tag(const StringName &p_tag) const;
	bool has_tag_hierarchical(const StringName &p_tag) const;

	void add_container(const Ref<GameplayTagContainer> &p_container);
	bool has_any(const Ref<GameplayTagContainer> &p_container) const;
	bool has_all(const Ref<GameplayTagContainer> &p_container) const;

	TypedArray<StringName> get_tags() const;

	GameplayTagContainer() {}
};
