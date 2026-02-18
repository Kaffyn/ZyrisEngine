/**************************************************************************/
/*  gameplay_effect_spec.cpp                                              */
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

#include "modules/gas/core/gameplay_effect_spec.h"

void GameplayEffectSpec::set_effect(const Ref<GameplayEffect> &p_effect) {
	effect = p_effect;
	if (effect.is_valid()) {
		duration_remaining = effect->get_duration();
	}
}

bool GameplayEffectSpec::is_expired() const {
	if (effect.is_null()) {
		return true;
	}
	if (effect->get_duration_type() == GE_DURATION_INSTANT) {
		return true;
	}
	if (effect->get_duration_type() == GE_DURATION_INFINITE) {
		return false;
	}
	return duration_remaining <= 0.0;
}

void GameplayEffectSpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_effect", "effect"), &GameplayEffectSpec::set_effect);
	ClassDB::bind_method(D_METHOD("get_effect"), &GameplayEffectSpec::get_effect);
	ClassDB::bind_method(D_METHOD("set_duration_remaining", "duration"), &GameplayEffectSpec::set_duration_remaining);
	ClassDB::bind_method(D_METHOD("get_duration_remaining"), &GameplayEffectSpec::get_duration_remaining);
	ClassDB::bind_method(D_METHOD("is_expired"), &GameplayEffectSpec::is_expired);
}
