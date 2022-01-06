#include "config.hpp"
using namespace alias;

#include <make_exception.hpp>
#include <env.hpp>
#include <str.hpp>
#include <TermAPI.hpp>
#include <process.hpp>

/** @def RETURN_CODE_EXCEPTION @brief Return value when an exception occurred and interrupted the program. */
#define RETURN_CODE_EXCEPTION -2
/** @def RETURN_CODE_PROCFAILURE @brief Return value when an error occurred during command execution. */
#define RETURN_CODE_PROCFAILURE -3
/** @def RETURN_CODE_INITIALIZE @brief Return value when no config file was detected, and a new one was created. */
#define RETURN_CODE_INITIALIZE -4
/** @def RETURN_CODE_BADCONFIG @brief Return value when the config's version is incompatible. */
#define RETURN_CODE_OLDCONFIG -5

inline std::string merge_args(const int argc, char** argv, const int off = 1)
{
	std::string buffer{};
	buffer.reserve(256);
	for (int i{ off }; i < argc; ++i) {
		buffer += argv[i];
		if (i + 1 < argc)
			buffer += ' ';
	}
	buffer.shrink_to_fit();
	return buffer;
}

inline void version_check()
{
	if (ALIAS_VERSION_MAJOR > std::get<0>(Global.file_version)) {
		Global.log.error("Major Version Mismatch: (", color::setcolor::red, version_to_string(Global.file_version), color::reset_f, " < ", ALIAS_VERSION, ')');
		throw make_exception("Config file was generated with an incompatible version of alias, delete it and regenerate.");
	}
	else if (ALIAS_VERSION_MINOR > std::get<1>(Global.file_version))
		Global.log.log("Minor Version Mismatch: (", color::setcolor::yellow, version_to_string(Global.file_version), color::reset_f, " < ", ALIAS_VERSION, ')');
	else if (ALIAS_VERSION_PATCH > std::get<2>(Global.file_version))
		Global.log.log("Patch Version Mismatch: (", version_to_string(Global.file_version), " < ", ALIAS_VERSION, ')');
}

 /**
  * @brief			Main.
  * @param argc		Argument Count
  * @param argv		Argument Array
  * @returns		int
  *\n
  *					| Value					 | Description				|
  *					| ---------------------- | ------------------------ |
  *					| <any>					 | Process Return Code		|
  *					| RETURN_CODE_EXCEPTION	 | An Exception Occurred	|
  *					| RETURN_CODE_INITIALIZE | Wrote default INI		|
  */
int main(const int argc, char** argv)
{
	try {
		std::cout << term::EnableANSI;

		Global.log.info("Alias version ", ALIAS_VERSION);

		// Locate the config file
		env::PATH path{ {argv[0]} };
		const auto& [program_path, program_name] { path.resolve_split(argv[0]) };
		const auto cfg_path{ program_path / std::filesystem::path(program_name).replace_extension("ini") };

		Global.log.debug("Config Path: ", cfg_path.generic_string());

		// Check if the config file exists
		if (!file::exists(cfg_path)) {
			Global.log.debug("Missing Config File.");
			if (write_config(cfg_path, Version{ ALIAS_VERSION_MAJOR, ALIAS_VERSION_MINOR, ALIAS_VERSION_PATCH }))
				Global.log.msg("Successfully created ", cfg_path.generic_string());
			else
				Global.log.error("Failed to create file ", cfg_path.generic_string());
			exit(RETURN_CODE_INITIALIZE);
		}

		// Read the config
		auto cfg{ read_config(cfg_path) };

		// Check the config version
		version_check();
		
		// Check if the command field is blank
		if (Global.command.empty())
			throw make_exception("Invalid config: Command is blank!");

		Global.log.info("Successfully read config file ", cfg_path.generic_string());

		// Concatenate arguments to the command if forward_args is enabled
		if (Global.forward_args && argc > 1) {
			if (const auto margs{ merge_args(argc, argv) }; !margs.empty()) {
				Global.command.reserve(Global.command.size() + 1ull + margs.size());
				Global.command += ' ';
				Global.command += margs;
			}
		}

		Global.log.info("Command:\t\"", Global.command, '\"');
		
		int rc{ RETURN_CODE_PROCFAILURE };

		// Select output method
		if (!Global.allow_output) { // no output
			if (process::Proc(rc, Global.command).close())
				Global.log.info("Silent execution successful.");
			else throw make_exception("Failed to execute command \"", Global.command, '\"');
		}
		else {
			if (!Global.out_file.empty()) { // File
				Global.log.info("Directing output to \"", Global.out_file, '\"');
				if (std::ofstream ofs{ Global.out_file }; ofs.is_open())
					ofs << process::Proc(rc, Global.command) << newline_if_enabled;
				else throw make_exception("Failed to open output file \"", Global.out_file, "\"!");
			}
			else { // STDOUT
				Global.log.info("Directing output to STDOUT");
				std::cout << process::Proc(rc, Global.command) << newline_if_enabled;
			}
		}

		// check if process pipe set the return code correctly
		if (rc == RETURN_CODE_PROCFAILURE) {
			Global.log.debug("Return code pointer wasn't set, the process pipe failed to correctly set a return value!");
			throw make_exception('\"', color::setcolor::yellow, Global.command, color::reset_f, "\" failed!");
		}

		// pause before exiting if enabled in the config
		if (Global.pause_before_exit) {
			std::cout << "Press " << color::setcolor::red << "<Enter>" << color::reset_f << " to exit." << std::endl;
			(void)std::cin.get();
		}

		// log the return value
		Global.log.log("Command returned: ", color::setcolor::green, rc, color::setcolor::reset);

		// forward the command return value
		return rc;
	} catch (const std::exception& ex) { // catch std::exceptions
		Global.log.error(ex.what());
	} catch (...) { // catch other exceptions
		Global.log.crit("An unknown exception occurred!");
	}
	return RETURN_CODE_EXCEPTION;
}