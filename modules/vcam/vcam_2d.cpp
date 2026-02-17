/**************************************************************************/
/*  vcam_2d.cpp                                                           */
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

#include "vcam_2d.h"
#include "shake_profile.h"
#include "shot_profile.h"
#include "vcam_server.h"

void vCam2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (vCamServer::get_singleton()) {
				vCamServer::get_singleton()->register_vcam_2d(this);
			}
			set_process_internal(true);
		} break;
		case NOTIFICATION_INTERNAL_PROCESS: {
			if (vCamServer::get_singleton()) {
				vCamServer::get_singleton()->update(get_process_delta_time());
			}
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (vCamServer::get_singleton()) {
				vCamServer::get_singleton()->unregister_vcam_2d(this);
			}
		} break;
	}
}

void vCam2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_priority", "priority"), &vCam2D::set_priority);
	ClassDB::bind_method(D_METHOD("get_priority"), &vCam2D::get_priority);

	ClassDB::bind_method(D_METHOD("set_shot_profile", "profile"), &vCam2D::set_shot_profile);
	ClassDB::bind_method(D_METHOD("get_shot_profile"), &vCam2D::get_shot_profile);

	ClassDB::bind_method(D_METHOD("set_shake_profile", "profile"), &vCam2D::set_shake_profile);
	ClassDB::bind_method(D_METHOD("get_shake_profile"), &vCam2D::get_shake_profile);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "priority"), "set_priority", "get_priority");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shot_profile", PROPERTY_HINT_RESOURCE_TYPE, "ShotProfile"), "set_shot_profile", "get_shot_profile");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shake_profile", PROPERTY_HINT_RESOURCE_TYPE, "ShakeProfile"), "set_shake_profile", "get_shake_profile");
}

void vCam2D::set_priority(int p_priority) {
	priority = p_priority;
}

int vCam2D::get_priority() const {
	return priority;
}

void vCam2D::set_shot_profile(const Ref<ShotProfile> &p_profile) {
	shot_profile = p_profile;
}

Ref<ShotProfile> vCam2D::get_shot_profile() const {
	return shot_profile;
}

void vCam2D::set_shake_profile(const Ref<ShakeProfile> &p_profile) {
	shake_profile = p_profile;
}

Ref<ShakeProfile> vCam2D::get_shake_profile() const {
	return shake_profile;
}

vCam2D::vCam2D() {
}

vCam2D::~vCam2D() {
}
