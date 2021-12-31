#include "config.hpp"
using namespace alias;

#include <make_exception.hpp>
#include <env.hpp>
#include <str.hpp>
#include <TermAPI.hpp>
#include <process.hpp>

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

 /** @def RETURN_CODE_EXCEPTION @brief Return value when an exception occurred and interrupted the program. */
#define RETURN_CODE_EXCEPTION -2
/** @def RETURN_CODE_PROCFAILURE @brief Return value when an error occurred during command execution. */
#define RETURN_CODE_PROCFAILURE -3
/** @def RETURN_CODE_INITIALIZE @brief Return value when no config file was detected, and a new one was created. */
#define RETURN_CODE_INITIALIZE -4
/** @def RETURN_CODE_BADCONFIG @brief Return value when the config's version is incompatible. */
#define RETURN_CODE_OLDCONFIG -5
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
		if (!Global.check_file_version<0>(ALIAS_VERSION_MAJOR)) { // major versions don't match
			Global.log.error("Config was created with an incompatible version of alias, regenerating the config to update is required.");
			Global.log.error("Config Version:\t", version_to_string(Global.file_version));
			Global.log.error("Alias Version:\t", ALIAS_VERSION);
			return RETURN_CODE_OLDCONFIG;
		}
		else if (!Global.check_file_version<1>(ALIAS_VERSION_MINOR)) { // minor versions don't match
			Global.log.warn("Config was created with an older version of alias, and might not work. Regenerating the config is recommended.");
			Global.log.info("Config Version:\t", version_to_string(Global.file_version));
			Global.log.info("Alias Version:\t", ALIAS_VERSION);
		}
		else if (!Global.check_file_version<2>(ALIAS_VERSION_PATCH)) { // patch versions don't match
			Global.log.log("Config version is outdated, but is compatible.");
			Global.log.info("Config Version:\t", version_to_string(Global.file_version));
			Global.log.info("Alias Version:\t", ALIAS_VERSION);
		}

		Global.log.log("Successfully read config file ", cfg_path.generic_string());

		// Concatenate arguments to the command if forward_args is enabled
		if (Global.forward_args && argc > 1) {
			Global.command += ' ';
			Global.command += merge_args(argc, argv);
		}

		Global.log.info("Command:\t\"", Global.command, '\"');

		// Select output method
		if (!Global.allow_output) {
			if (process::exec(Global.command))
				Global.log.info("Silent execution successful.");
			else throw make_exception("Failed to execute command \"", Global.command, '\"');
		}
		else {
			if (Global.out_file.empty()) {
				Global.log.debug("Directing output to STDOUT");
				std::cout << process::exec(Global.command) << newline_if_enabled;
			}
			else {
				Global.log.info("Directing output to \"", Global.out_file, '\"');
				if (std::ofstream ofs{ Global.out_file }; ofs.is_open())
					ofs << process::exec(Global.command) << newline_if_enabled;
				else
					throw make_exception("Failed to open output file \"", Global.out_file, "\"!");
			}
		}

		// Return the received result code
		Global.pauseBeforeExit();
		return Global.getReturnCode().value_or(RETURN_CODE_PROCFAILURE);
	} catch (const std::exception& ex) { // catch std::exceptions
		std::cerr << term::crit << ex.what() << std::endl;
		return RETURN_CODE_EXCEPTION;
	} catch (...) { // catch other exceptions
		std::cerr << term::crit << "An unknown exception occurred!" << std::endl;
		return RETURN_CODE_EXCEPTION;
	}
}