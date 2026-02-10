/**************************************************************************/
/*  save_server.cpp                                                       */
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

#include "save_server.h"

#include "core/config/engine.h"
#include "core/config/project_settings.h"
#include "core/crypto/crypto_core.h"
#include "core/io/dir_access.h"
#include "core/io/file_access.h"
#include "core/io/marshalls.h"
#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/variant/variant_utility.h"
#include "scene/main/node.h"
#include "scene/resources/snapshot.h"

SaveServer *SaveServer::singleton = nullptr;

SaveServer *SaveServer::get_singleton() {
	return singleton;
}

void SaveServer::register_settings() {
	// Settings are now primary registered in project_settings.cpp for better grouping
	GLOBAL_DEF_BASIC("application/persistence/save_format", 0);
	GLOBAL_DEF_BASIC("application/persistence/encryption_key", "");
	GLOBAL_DEF_BASIC("application/persistence/compression_enabled", true);
	GLOBAL_DEF_BASIC("application/persistence/backup_enabled", true);
	GLOBAL_DEF_BASIC("application/persistence/integrity_check_level", 1);
	GLOBAL_DEF_BASIC("application/persistence/save_path", "user://saves/");
}

void SaveServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("save_slot", "slot_name", "data", "async", "metadata", "thumbnail"), &SaveServer::save_slot, DEFVAL(true), DEFVAL(Dictionary()), DEFVAL(Ref<Resource>()));
	ClassDB::bind_method(D_METHOD("load_slot", "slot_name"), &SaveServer::load_slot);
	ClassDB::bind_method(D_METHOD("has_slot", "slot_name"), &SaveServer::has_slot);
	ClassDB::bind_method(D_METHOD("delete_slot", "slot_name"), &SaveServer::delete_slot);

	ClassDB::bind_method(D_METHOD("save_snapshot", "root", "slot_name", "async", "tags", "metadata", "thumbnail"), &SaveServer::save_snapshot, DEFVAL(true), DEFVAL(TypedArray<StringName>()), DEFVAL(Dictionary()), DEFVAL(Ref<Resource>()));
	ClassDB::bind_method(D_METHOD("load_snapshot", "root", "slot_name", "callback"), &SaveServer::load_snapshot, DEFVAL(Callable()));
	ClassDB::bind_method(D_METHOD("has_snapshot", "slot_name"), &SaveServer::has_snapshot);

	ClassDB::bind_method(D_METHOD("_finish_load_async", "node_id", "data", "callback"), &SaveServer::_finish_load_async);

	ClassDB::bind_method(D_METHOD("register_migration", "from", "to", "callback"), &SaveServer::register_migration);

	ClassDB::bind_method(D_METHOD("set_save_format", "format"), &SaveServer::set_save_format);
	ClassDB::bind_method(D_METHOD("get_save_format"), &SaveServer::get_save_format);

	ClassDB::bind_method(D_METHOD("set_encryption_key", "key"), &SaveServer::set_encryption_key);
	ClassDB::bind_method(D_METHOD("get_encryption_key"), &SaveServer::get_encryption_key);

	ClassDB::bind_method(D_METHOD("set_compression_enabled", "enabled"), &SaveServer::set_compression_enabled);
	ClassDB::bind_method(D_METHOD("is_compression_enabled"), &SaveServer::is_compression_enabled);

	ClassDB::bind_method(D_METHOD("set_backup_enabled", "enabled"), &SaveServer::set_backup_enabled);
	ClassDB::bind_method(D_METHOD("is_backup_enabled"), &SaveServer::is_backup_enabled);

	ClassDB::bind_method(D_METHOD("set_integrity_check_level", "level"), &SaveServer::set_integrity_check_level);
	ClassDB::bind_method(D_METHOD("get_integrity_check_level"), &SaveServer::get_integrity_check_level);

	ClassDB::bind_method(D_METHOD("set_save_path", "path"), &SaveServer::set_save_path);
	ClassDB::bind_method(D_METHOD("get_save_path"), &SaveServer::get_save_path);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "save_format", PROPERTY_HINT_ENUM, "Text,Binary"), "set_save_format", "get_save_format");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "encryption_key"), "set_encryption_key", "get_encryption_key");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "compression_enabled"), "set_compression_enabled", "is_compression_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "backup_enabled"), "set_backup_enabled", "is_backup_enabled");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "integrity_check_level", PROPERTY_HINT_ENUM, "None,Signature,Strict"), "set_integrity_check_level", "get_integrity_check_level");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "save_path"), "set_save_path", "get_save_path");

	BIND_ENUM_CONSTANT(FORMAT_TEXT);
	BIND_ENUM_CONSTANT(FORMAT_BINARY);

	BIND_ENUM_CONSTANT(INTEGRITY_NONE);
	BIND_ENUM_CONSTANT(INTEGRITY_SIGNATURE);
	BIND_ENUM_CONSTANT(INTEGRITY_STRICT);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_FILE_LOCKED);
	BIND_ENUM_CONSTANT(ERR_ENCRYPTION);
	BIND_ENUM_CONSTANT(ERR_DISK_FULL);
	BIND_ENUM_CONSTANT(ERR_INVALID_DATA);
	BIND_ENUM_CONSTANT(ERR_CHECKSUM_MISMATCH);
	BIND_ENUM_CONSTANT(ERR_VERSION_MISMATCH);
}

void SaveServer::save_slot(const String &p_slot_name, const Dictionary &p_data, bool p_async, const Dictionary &p_metadata, Ref<Resource> p_thumbnail) {
	ERR_FAIL_COND_MSG(p_slot_name.is_empty(), "Slot name cannot be empty.");

	SaveTask task;
	task.slot_name = p_slot_name;
	task.data = p_data.duplicate(true); // Deep copy to ensure thread safety
	task.metadata = p_metadata.duplicate(true);
	task.thumbnail = p_thumbnail;
	task.format = current_format;
	task.encryption_key = encryption_key;
	task.compression_enabled = compression_enabled;

	if (p_async) {
		MutexLock lock(mutex);
		queue.push_back(task);
		semaphore.post();
	} else {
		_save_to_disk(task);
	}
}

Dictionary SaveServer::load_slot(const String &p_slot_name) {
	return _load_from_disk(p_slot_name);
}

bool SaveServer::has_slot(const String &p_slot_name) const {
	String res_path = save_path.path_join(p_slot_name + ".tres");
	String data_path = save_path.path_join(p_slot_name + ".data");
	return FileAccess::exists(res_path) || FileAccess::exists(data_path);
}

void SaveServer::delete_slot(const String &p_slot_name) {
	String res_path = save_path.path_join(p_slot_name + ".tres");
	String data_path = save_path.path_join(p_slot_name + ".data");
	if (FileAccess::exists(res_path)) {
		DirAccess::remove_absolute(res_path);
	}
	if (FileAccess::exists(data_path)) {
		DirAccess::remove_absolute(data_path);
	}
}

bool SaveServer::save_snapshot(Node *p_root, const String &p_slot_name, bool p_async, const TypedArray<StringName> &p_tags, const Dictionary &p_metadata, Ref<Resource> p_thumbnail) {
	ERR_FAIL_NULL_V(p_root, false);
	p_root->propagate_notification(Node::NOTIFICATION_SAVE_PREPARE);
	Dictionary snapshot = p_root->save_all_persistence(p_tags);
	save_slot(p_slot_name, snapshot, p_async, p_metadata, p_thumbnail);
	p_root->propagate_notification(Node::NOTIFICATION_SAVE_COMPLETED);
	return true;
}

void SaveServer::load_snapshot(Node *p_root, const String &p_slot_name, const Callable &p_callback) {
	ERR_FAIL_NULL(p_root);

	SaveTask task;
	task.type = TASK_LOAD;
	task.slot_name = p_slot_name;
	task.target_node_id = p_root->get_instance_id();
	task.user_callback = p_callback;

	task.format = current_format;
	task.encryption_key = encryption_key;

	MutexLock lock(mutex);
	queue.push_back(task);
	semaphore.post();
}

void SaveServer::_finish_load_async(ObjectID p_node_id, const Dictionary &p_data, const Callable &p_callback) {
	Object *obj = ObjectDB::get_instance(p_node_id);
	Node *root = Object::cast_to<Node>(obj);

	if (root) {
		if (!p_data.is_empty()) {
			root->propagate_notification(Node::NOTIFICATION_LOAD_STARTED);
			root->load_all_persistence(p_data);
			root->propagate_notification(Node::NOTIFICATION_LOAD_COMPLETED);
		} else {
			WARN_PRINT("SaveServer: Async load returned empty data (or file missing).");
		}
	}

	if (p_callback.is_valid()) {
		p_callback.call();
	}
}

Error SaveServer::_save_to_disk(const SaveTask &p_task) {
	// Determine extension and format based on settings
	SaveFormat format = p_task.format;
	String ext = (format == FORMAT_TEXT) ? ".tres" : ".data";
	String full_path = save_path.path_join(p_task.slot_name + ext);

	// 1. Shadow Backup logic
	if (backup_enabled && FileAccess::exists(full_path)) {
		Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_USERDATA);
		String backup_path = full_path + ".bak";
		da->copy(full_path, backup_path);
	}

	// Create directory if it doesn't exist
	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_USERDATA);
	if (!da->dir_exists(save_path)) {
		da->make_dir_recursive(save_path);
	}

	Ref<Snapshot> snapshot_res;
	snapshot_res.instantiate();
	snapshot_res->set_snapshot(p_task.data);
	snapshot_res->set_metadata(p_task.metadata);
	snapshot_res->set_thumbnail(p_task.thumbnail);

	// Inject version
	String project_version = GLOBAL_GET("application/config/version");
	snapshot_res->set_version(project_version);

	// Calculate and set checksum
	snapshot_res->set_checksum(_calculate_checksum(p_task.data));

	if (format == FORMAT_TEXT) {
		return ResourceSaver::save(snapshot_res, full_path);
	} else {
		// Individual task settings for thread safety
		String key = p_task.encryption_key;
		bool compress = p_task.compression_enabled;

		Ref<FileAccess> f;
		if (!key.is_empty()) {
			f = FileAccess::open_encrypted_pass(full_path, FileAccess::WRITE, key);
		} else if (compress) {
			f = FileAccess::open_compressed(full_path, FileAccess::WRITE, FileAccess::COMPRESSION_ZSTD);
		} else {
			f = FileAccess::open(full_path, FileAccess::WRITE);
		}

		ERR_FAIL_COND_V_MSG(f.is_null(), ERR_FILE_CANT_OPEN, "Cannot open save file for writing: " + full_path);
		f->store_var(snapshot_res, true); // Serialize full Snapshot object
		f->close();
		return ::OK;
	}
}

Dictionary SaveServer::_load_from_disk(const String &p_slot_name) {
	Ref<Snapshot> snapshot_res = _read_snapshot_from_disk(p_slot_name);

	// 1. Backup recovery if main save is missing or corrupted
	if (snapshot_res.is_null() && backup_enabled) {
		snapshot_res = _read_snapshot_from_disk(p_slot_name + ".bak");
		if (snapshot_res.is_valid()) {
			print_line("SaveServer: Main save corrupted or missing, restored from backup: " + p_slot_name);
		}
	}

	if (snapshot_res.is_null()) {
		return Dictionary();
	}

	// 2. Integrity check
	if (integrity_level >= INTEGRITY_SIGNATURE) {
		String calculated = _calculate_checksum(snapshot_res->get_snapshot());
		if (calculated != snapshot_res->get_checksum()) {
			WARN_PRINT("SaveServer: Integrity check failed (Signature mismatch) for slot: " + p_slot_name);
			if (integrity_level == INTEGRITY_STRICT) {
				ERR_PRINT("SaveServer: STRICT level active, rejecting save.");
				return Dictionary();
			}
		}
	}

	// 3. Versioning and Migrations
	_apply_migrations(snapshot_res);

	return snapshot_res->get_snapshot();
}

Ref<Snapshot> SaveServer::_read_snapshot_from_disk(const String &p_slot_name) {
	// Try text format first (.tres)
	String res_path = save_path.path_join(p_slot_name);
	if (!res_path.ends_with(".tres") && !res_path.ends_with(".data") && !res_path.ends_with(".bak")) {
		res_path += ".tres";
	}

	if (FileAccess::exists(res_path)) {
		Ref<Snapshot> res = ResourceLoader::load(res_path);
		if (res.is_valid()) {
			return res;
		}
	}

	// Try binary format if not already tried (.data)
	String data_path = save_path.path_join(p_slot_name);
	if (!data_path.ends_with(".data") && !data_path.ends_with(".bak")) {
		data_path += ".data";
	}

	if (FileAccess::exists(data_path)) {
		String key = GLOBAL_GET("application/persistence/encryption_key");
		Ref<FileAccess> f;
		if (!key.is_empty()) {
			f = FileAccess::open_encrypted_pass(data_path, FileAccess::READ, key);
		} else {
			f = FileAccess::open_compressed(data_path, FileAccess::READ, FileAccess::COMPRESSION_ZSTD);
			if (f.is_null()) {
				f = FileAccess::open(data_path, FileAccess::READ);
			}
		}

		if (f.is_valid()) {
			Variant v = f->get_var(true);
			f->close();
			Ref<Snapshot> res = v;
			if (res.is_valid()) {
				return res;
			}
		}
	}

	return Ref<Snapshot>();
}

String SaveServer::_calculate_checksum(const Dictionary &p_data) {
	// For a high-performance AAA checksum, we use SHA-256 over the marshaled dictionary
	int len;
	Error err = encode_variant(p_data, nullptr, len, false);
	ERR_FAIL_COND_V(err != ::OK, "");

	Vector<uint8_t> buffer;
	buffer.resize(len);
	encode_variant(p_data, buffer.ptrw(), len, false);

	unsigned char hash[32];
	CryptoCore::sha256(buffer.ptr(), buffer.size(), hash);

	return String::hex_encode_buffer(hash, 32);
}

void SaveServer::register_migration(const String &p_from, const String &p_to, const Callable &p_callback) {
	Migration m;
	m.from = p_from;
	m.to = p_to;
	m.callback = p_callback;
	migrations.push_back(m);
}

void SaveServer::_apply_migrations(Ref<Snapshot> p_snapshot) {
	String current_ver = p_snapshot->get_version();
	String target_ver = GLOBAL_GET("application/config/version");

	if (current_ver == target_ver) {
		return;
	}

	print_line(vformat("SaveServer: Migrating save from %s to %s", current_ver, target_ver));

	bool changed = true;
	while (changed && current_ver != target_ver) {
		changed = false;
		for (const Migration &m : migrations) {
			if (m.from == current_ver) {
				Dictionary data = p_snapshot->get_snapshot();
				const Variant v_data = data;
				const Variant *args[1] = { &v_data };
				Variant ret;
				Callable::CallError ce;
				m.callback.callp(args, 1, ret, ce);

				if (ce.error == Callable::CallError::CALL_OK) {
					current_ver = m.to;
					p_snapshot->set_version(current_ver);
					changed = true;
					break;
				}
			}
		}
	}
}

void SaveServer::_save_thread_func(void *p_userdata) {
	SaveServer *ss = (SaveServer *)p_userdata;
	while (!ss->exit_thread.is_set()) {
		ss->semaphore.wait();
		if (ss->exit_thread.is_set()) {
			break;
		}
		ss->_process_queue();
	}
}

void SaveServer::_process_queue() {
	while (true) {
		SaveTask task;
		{
			MutexLock lock(mutex);
			if (queue.is_empty()) {
				break;
			}
			task = queue.front()->get();
			queue.pop_front();
		}

		if (task.type == TASK_SAVE) {
			_save_to_disk(task);
		} else if (task.type == TASK_LOAD) {
			Dictionary data = _load_from_disk(task.slot_name);
			// Dispatch back to main thread
			call_deferred("_finish_load_async", task.target_node_id, data, task.user_callback);
		}
	}
}

void SaveServer::set_save_format(SaveFormat p_format) {
	current_format = p_format;
}

SaveServer::SaveFormat SaveServer::get_save_format() const {
	return current_format;
}

void SaveServer::set_encryption_key(const String &p_key) {
	encryption_key = p_key;
}

String SaveServer::get_encryption_key() const {
	return encryption_key;
}

void SaveServer::set_compression_enabled(bool p_enabled) {
	compression_enabled = p_enabled;
}

bool SaveServer::is_compression_enabled() const {
	return compression_enabled;
}

void SaveServer::set_save_path(const String &p_path) {
	save_path = p_path;
}

String SaveServer::get_save_path() const {
	return save_path;
}

void SaveServer::set_backup_enabled(bool p_enabled) {
	backup_enabled = p_enabled;
}

bool SaveServer::is_backup_enabled() const {
	return backup_enabled;
}

void SaveServer::set_integrity_check_level(IntegrityCheckLevel p_level) {
	integrity_level = p_level;
}

SaveServer::IntegrityCheckLevel SaveServer::get_integrity_check_level() const {
	return integrity_level;
}

SaveServer::SaveServer() {
	singleton = this;
	exit_thread.clear();

	// Auto-generate project-specific encryption key if empty (Editor-only)
	if (Engine::get_singleton()->is_editor_hint()) {
		String key = GLOBAL_GET("application/persistence/encryption_key");
		if (key.is_empty()) {
			// Generate a 32-char hex string based on time and random
			String allowed = "abcdef0123456789";
			String new_key = "";
			for (int i = 0; i < 32; i++) {
				int r = Math::rand() % allowed.length();
				new_key += String::chr(allowed[r]);
			}
			ProjectSettings::get_singleton()->set_setting("application/persistence/encryption_key", new_key);
			ProjectSettings::get_singleton()->save();
			print_line("Persistence System: Auto-generated unique encryption key for the project.");
		}
	}

	save_thread.start(_save_thread_func, this);
}

SaveServer::~SaveServer() {
	exit_thread.set();
	semaphore.post();
	save_thread.wait_to_finish();
	singleton = nullptr;
}
