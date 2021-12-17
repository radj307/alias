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
 *# SYNTAX LEGEND
  <>		Variable
  []		Condition
  ?(...)	Condition Outcome when True
  ?{...}	Condition Block when True
  !(...)	Condition Outcome when False
  !{...}	Condition Block when False
  //...		Comment
  {...}		Block
 *
 *#	PROGRAM FLOW
 *	[<config> exists]
 *		!{
 *			{write config file}
 *			{return 1}
 *		}
 *		?{
 *			[<pass_args>]
 *				?(Append all arguments to the outgoing command)
 *			[<allow_output>]
 *				!(Execute the command without saving the output)
 *				?{
 *					[<out_file> is NOT empty]
 *						?(Direct command output to the file specified by <out_file>)
 *						!(Direct command output to STDOUT)
 *				}
 *			{return 0}
 *		}
 *
 *
 */
 /**
  * @brief			Main.
  * @param argc		Argument Count
  * @param argv		Argument Array
  * @returns		int
  *\n
  *					| Value | Description			|
  *					| ----- | --------------------- |
  *					| 1		| Wrote default INI		|
  *					| 0		| Successful execution	|
  *					| -1	| std::exception		|
  *					| -2	| undefined exception	|
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
				write_log(level::MESSAGE, "Successfully created ", cfg_path);
			else
				write_log(level::ERROR, "Failed to create file: ", cfg_path);
			exit(1);
		}

		auto cfg{ read_config(cfg_path) };

		write_log(level::INFO, "Successfully read config file ", cfg_path);
		write_log(level::DEBUG, "Config File Contents:\n", cfg);
		write_log(level::DEBUG, "Parsed Values:",
			"\ncommand:        ", Global.command,
			"\nallow_output:   ", str::bool_to_string(Global.allow_output),
			"\nappend_newline: ", str::bool_to_string(Global.append_newline),
			"\nout_file:       ", Global.out_file,
			"\nlog_level:      ", Global.log.getLevel().as_string_id(),
			"\npass_args:      ", str::bool_to_string(Global.pass_args)
		);

		if (Global.pass_args && argc > 1) {
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
				write_log(level::DEBUG, "Directing output to STDOUT");
				std::cout << Exec(Global.command);
			}
			else {
				write_log(level::INFO, "Directing output to \"", Global.out_file, '\"');
				if (std::ofstream ofs{ Global.out_file }; ofs.is_open())
					ofs << Exec(Global.command);
				else
					throw make_exception("Failed to open output file \"", Global.out_file, "\"!");
			}
		}

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::crit << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}