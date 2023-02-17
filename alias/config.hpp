#pragma once
#include "globals.h"

#include <simpleINI.hpp>
#include <str.hpp>
#include <strcore.hpp>

#include <utility>
#include <filesystem>

namespace alias {
	inline CONSTEXPR const auto HEADER_TARGET{ "target" }, HEADER_OUTPUT{ "output" }, HEADER_MISC{ "misc" };

	/// @brief	Read the specified config from disk
	[[nodiscard]] inline ini::INI read_config(const std::filesystem::path& path)
	{
		ini::INI cfg{ path };
		// file_version
		Global.file_version = string_to_version(cfg.get_or("", "file_version", "0.0.0"));
		// target
		Global.command = cfg.get_or(HEADER_TARGET, "command", "");
		Global.forward_args = cfg.checkv(HEADER_TARGET, "forward_args", true);
		// output
		Global.allow_output = cfg.checkv(HEADER_OUTPUT, "allow_output", true);
		Global.pause_before_exit = cfg.checkv(HEADER_OUTPUT, "pause_before_exit", true);
		Global.append_newline = cfg.checkv(HEADER_OUTPUT, "append_newline", true);
		Global.out_file = cfg.get_or(HEADER_OUTPUT, "out_file", "");
		// misc
		Global.log.setLevel(str::tonumber<int>(cfg.get_or(HEADER_MISC, "log_level", "error").as<std::string>().value()));
		return cfg;
	}

	/// @brief	Write the given config to disk
	[[nodiscard]] inline bool write_config(const std::filesystem::path& path, ini::INI& ini, const bool& update_version = false)
	{
		if (update_version)
			ini.insert_or_assign("", "file_version", std::string(alias_VERSION));
		return ini.write(path);
	}

	/// @brief	Write the default config to disk
	[[nodiscard]] inline bool write_config(const std::filesystem::path& path)
	{
		std::stringstream buffer;
		buffer
			<< "file_version = " << alias_VERSION << '\n'
			<< '[' << HEADER_TARGET << "]\n"
			<< "command = \"" << Global.command << "\" ; Put the command you want to execute here\n"
			<< "forward_args = " << str::bool_to_string(Global.forward_args) << " ; When true, passes any arguments received to the target by appending them to the command string.\n"
			<< '\n'
			<< '[' << HEADER_OUTPUT << "]\n"
			<< "allow_output = " << str::bool_to_string(Global.allow_output) << " ; When false, the command produces no output.\n"
			<< "pause_before_exit = " << str::bool_to_string(Global.pause_before_exit) << " ; Prompts for a key press before exiting. Only works if the command executed successfully.\n"
			<< "append_newline = " << str::bool_to_string(Global.append_newline) << " ; When true, appends a newline character to the output.\n"
			<< "out_file = \"" << Global.out_file << "\" ; Put the name of a file to direct output to. If empty, STDOUT is used.\n"
			<< '\n'
			<< '[' << HEADER_MISC << "]\n"
			<< "log_level = " << Global.log.getLevel().as_string_id() << " ; (Range: 0 - 127) See below table for flag values. Default is 15.\n"
			<< "; | Flag | Log Level |\n"
			<< "; |------|-----------|\n"
			<< "; | `0`  | None      |\n"
			<< "; | `1`  | Critical  |\n"
			<< "; | `2`  | Error     |\n"
			<< "; | `4`  | Warning   |\n"
			<< "; | `8`  | Message   |\n"
			<< "; | `16` | Log       |\n"
			<< "; | `32` | Info      |\n"
			<< "; | `64` | Debug     |\n"
			;
		return file::write(path, buffer.rdbuf());
	}


}