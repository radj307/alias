#pragma once
#include "globals.h"

#define INI_USE_EXPERIMENTAL
#include <INI.hpp>

#include <filesystem>

namespace alias {
	inline constexpr const auto HEADER_TARGET{ "target" }, HEADER_OUTPUT{ "output" }, HEADER_MISC{ "misc" };

	/// @brief	Read the specified config from disk
	[[nodiscard]] inline file::INI read_config(const std::filesystem::path& path)
	{
		file::INI cfg{ path };
		// target
		Global.command = cfg.getvs(HEADER_TARGET, "command").value_or("");
		Global.pass_args = cfg.checkv(HEADER_TARGET, "pass_args", true);
		// output
		Global.allow_output = cfg.checkv(HEADER_OUTPUT, "allow_output", true);
		Global.append_newline = cfg.checkv(HEADER_OUTPUT, "append_newline", true);
		Global.out_file = cfg.getvs(HEADER_OUTPUT, "out_file").value_or("");
		// misc
		Global.log.setLevel(xlog::include_all_below(xlog::string_to_level(cfg.getvs(HEADER_MISC, "log_level").value_or("error"))));
		return cfg;
	}

	/// @brief	Write the given config to disk
	[[nodiscard]] inline bool write_config(const std::filesystem::path& path, const file::INI& ini) { return ini.write(path); }

	/// @brief	Write the default config to disk
	[[nodiscard]] inline bool write_config(const std::filesystem::path& path)
	{
		std::stringstream buffer;
		buffer
			<< '[' << HEADER_TARGET << "]\n"
			<< "command = \"" << Global.command << "\" ; Put the command you want to execute here\n"
			<< "pass_args = " << str::bool_to_string(Global.pass_args) << " ; When true, passes any arguments received to the target by appending them to the command string."
			<< '\n'
			<< '[' << HEADER_OUTPUT << "]\n"
			<< "allow_output = " << str::bool_to_string(Global.allow_output) << " ; When false, the command produces no output.\n"
			<< "append_newline = " << str::bool_to_string(Global.append_newline) << " ; When true, appends a newline character to the output.\n"
			<< "out_file = \"" << Global.out_file << "\" ; If this is empty, STDOUT is used instead.\n"
			<< '\n'
			<< '[' << HEADER_MISC << "]\n"
			<< "log_level = " << Global.log.getLevel().as_string_id() << '\n'
			;
		return file::write(path, buffer.rdbuf(), false);
	}
}