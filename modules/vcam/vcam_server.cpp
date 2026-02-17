/**************************************************************************/
/*  vcam_server.cpp                                                       */
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

#include "vcam_server.h"
#include "core/math/math_funcs.h"
#include "vcam_2d.h"
#include "vcam_3d.h"

vCamServer *vCamServer::singleton = nullptr;

void vCamServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_active_vcam_3d"), &vCamServer::get_active_vcam_3d);
	ClassDB::bind_method(D_METHOD("get_active_vcam_2d"), &vCamServer::get_active_vcam_2d);

	ClassDB::bind_method(D_METHOD("add_trauma", "amount"), &vCamServer::add_trauma);
	ClassDB::bind_method(D_METHOD("set_trauma", "amount"), &vCamServer::set_trauma);
	ClassDB::bind_method(D_METHOD("get_trauma"), &vCamServer::get_trauma);

	ClassDB::bind_method(D_METHOD("update", "delta"), &vCamServer::update);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "trauma"), "set_trauma", "get_trauma");
}

void vCamServer::update(float p_delta) {
	time_passed += p_delta;
	_update_trauma(p_delta);
	_update_arbitration();
}

void vCamServer::_update_trauma(float p_delta) {
	if (trauma > 0.0f) {
		trauma = MAX(0.0f, trauma - trauma_decay * p_delta);
	}
}

void vCamServer::add_trauma(float p_amount) {
	trauma = MIN(1.0f, trauma + p_amount);
}

void vCamServer::set_trauma(float p_amount) {
	trauma = CLAMP(p_amount, 0.0f, 1.0f);
}

float vCamServer::get_trauma() const {
	return trauma;
}

Vector3 vCamServer::get_shake_offset_3d() const {
	if (trauma <= 0.0f) {
		return Vector3();
	}

	float amount = trauma * trauma; // Exponential feel
	// Simple pseudo-noise for demonstration (Zyris should use Perlin if available)
	float x = (Math::random(-1.0f, 1.0f)) * amount;
	float y = (Math::random(-1.0f, 1.0f)) * amount;
	float z = (Math::random(-1.0f, 1.0f)) * amount;

	return Vector3(x, y, z);
}

float vCamServer::get_shake_offset_2d() const {
	if (trauma <= 0.0f) {
		return 0.0f;
	}

	float amount = trauma * trauma;
	return Math::random(-1.0f, 1.0f) * amount;
}

void vCamServer::_update_arbitration() {
	// 3D Arbitration
	vCam3D *best_3d = nullptr;
	int max_priority_3d = -1;

	for (int i = 0; i < vcams_3d.size(); i++) {
		if (vcams_3d[i]->get_priority() > max_priority_3d) {
			max_priority_3d = vcams_3d[i]->get_priority();
			best_3d = vcams_3d[i];
		}
	}

	if (active_vcam_3d != best_3d) {
		active_vcam_3d = best_3d;
		// Signal change or start blend
	}

	// 2D Arbitration
	vCam2D *best_2d = nullptr;
	int max_priority_2d = -1;

	for (int i = 0; i < vcams_2d.size(); i++) {
		if (vcams_2d[i]->get_priority() > max_priority_2d) {
			max_priority_2d = vcams_2d[i]->get_priority();
			best_2d = vcams_2d[i];
		}
	}

	if (active_vcam_2d != best_2d) {
		active_vcam_2d = best_2d;
		// Signal change or start blend
	}
}

void vCamServer::register_vcam_3d(vCam3D *p_vcam) {
	if (!vcams_3d.has(p_vcam)) {
		vcams_3d.push_back(p_vcam);
	}
}

void vCamServer::unregister_vcam_3d(vCam3D *p_vcam) {
	vcams_3d.erase(p_vcam);
}

void vCamServer::register_vcam_2d(vCam2D *p_vcam) {
	if (!vcams_2d.has(p_vcam)) {
		vcams_2d.push_back(p_vcam);
	}
}

void vCamServer::unregister_vcam_2d(vCam2D *p_vcam) {
	vcams_2d.erase(p_vcam);
}

vCamServer::vCamServer() {
	singleton = this;
}

vCamServer::~vCamServer() {
	singleton = nullptr;
}
