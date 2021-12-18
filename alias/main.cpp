#include "config.hpp"
using namespace alias;

#include <make_exception.hpp>
#include <env.hpp>
#include <str.hpp>
#include <TermAPI.hpp>
#include <exec.hpp>

template<typename... T>
inline void write_log(T&&... msg)
{
	((Global.log << std::forward<T>(msg)), ...) << xlog::endm;
}

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

#undef ERROR

/**
 *	# SYNTAX LEGEND
 *	$>			Program Entry Point
 *	<$			Program Exit Point
 *	{}			Block
 *	<>			Variable Name
 *	()			Operation
 *	-[]			Condition
 *	?...		When True
 *	!...		When False
 *	^...		Identifier
 *	>>...		Jump to Next
 *	<<...		Jump to Previous
 *	&			AND
 *	|			OR
 *
 *	# PROGRAM FLOW
 *	$>
		-[config file exists]
		?{
			(Read Config File)
			-[<forward_args>]
			?(Append <argv> to <command>)
			-[<allow_output>]
			?{
				-[<out_file> is empty]
				?(Execute <command> & direct output to STDOUT)
				!(Execute <command> & direct output to <out_file>)
			}
			!(Execute <command> & discard output)
		}
		!(Write Config File)
	<$
 */
 /** @def RETURN_CODE_EXCEPTION @brief Return value when an exception occurred and interrupted the program. */
#define RETURN_CODE_EXCEPTION -2
/** @def RETURN_CODE_PROCFAILURE @brief Return value when an error occurred during command execution. */
#define RETURN_CODE_PROCFAILURE -3
/** @def RETURN_CODE_INITIALIZE @brief Return value when no config file was detected, and a new one was created. */
#define RETURN_CODE_INITIALIZE -4
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
		env::PATH path{ {argv[0]} };
		const auto& [program_path, program_name] { path.resolve_split(argv[0]) };
		const auto cfg_path{ program_path / std::filesystem::path(program_name).replace_extension("ini") };

		write_log(level::DEBUG, "Config Path: ", cfg_path);

		if (!file::exists(cfg_path)) {
			write_log(level::INFO, "Missing Config File.");
			if (write_config(cfg_path))
				Global.log.msg("Successfully created ", cfg_path);
			else
				Global.log.error("Failed to create file ", cfg_path);
			exit(RETURN_CODE_INITIALIZE);
		}

		auto cfg{ read_config(cfg_path) };

		Global.log.info("Successfully read config file ", cfg_path);

		if (Global.forward_args && argc > 1) {
			Global.command += ' ';
			Global.command += merge_args(argc, argv);
		}

		write_log(level::INFO, "Command:\t\"", Global.command, '\"');

		if (!Global.allow_output) {
			if (exec(Global.command.c_str()))
				write_log(level::INFO, "Silent execution successful.");
			else throw make_exception("Failed to execute command \"", Global.command, '\"');
		}
		else {
			if (Global.out_file.empty()) {
				Global.log.debug("Directing output to STDOUT");
				std::cout << Exec(Global.command) << newline_if_enabled;
			}
			else {
				write_log(level::INFO, "Directing output to \"", Global.out_file, '\"');
				if (std::ofstream ofs{ Global.out_file }; ofs.is_open())
					ofs << Exec(Global.command) << newline_if_enabled;
				else
					throw make_exception("Failed to open output file \"", Global.out_file, "\"!");
			}
		}

		Global.pauseBeforeExit();
		return Global.getReturnCode().value_or(RETURN_CODE_PROCFAILURE);
	} catch (const std::exception& ex) {
		std::cerr << term::crit << ex.what() << std::endl;
		return RETURN_CODE_EXCEPTION;
	} catch (...) {
		std::cerr << term::crit << "An unknown exception occurred!" << std::endl;
		return RETURN_CODE_EXCEPTION;
	}
}