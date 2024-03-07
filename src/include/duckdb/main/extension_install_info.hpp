//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/main/extension_install_info.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb.hpp"

namespace duckdb {

enum class ExtensionInstallMode : uint8_t {
	//! Extension was installed using a url deduced from a repository base url
	REPOSITORY = 1,
	//! Extension was installed from DuckDB's LocalFileSystem
	LOCAL_FILE = 2,
	//! Extension was install from a custom path, this could be either local or remote
	CUSTOM_PATH = 3
};

class ExtensionInstallInfo {
public:
	//! How the extension was installed
	ExtensionInstallMode mode;
	//! Full path where the extension was generated from
	string full_path;
	//! (optional) Repository where the extension came from
	string repository;

	void Serialize(Serializer &serializer) const;
	static unique_ptr<ExtensionInstallInfo> Deserialize(Deserializer &deserializer);
};

} // namespace duckdb
