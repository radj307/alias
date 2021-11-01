#include <array>
#include <TermAPI.hpp>
#include <ParamsAPI.hpp>
#include <resolve-path.hpp>
#include <fileutil.hpp>
#include "Setting.hpp"

using namespace commandalias;

namespace commandalias {
	inline int exec(const char* cmd)
	{
		auto pipe(_popen(cmd, "r"));
		if (!pipe)
			throw std::runtime_error("_popen() failed to open a pipe!");
		for (std::array<char, 128> buffer{}; fread(&buffer, 1, buffer.size() - 1, pipe) != 0u; std::cout << buffer.data()) {}
		return _pclose(pipe);
	}
}

constexpr const bool INI_NAME_MUST_MATCH_PROGRAM_NAME{ true };

int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI;

		opt::ParamsAPI args{
			argc,
			argv,
			Arg::WRITE_INI.opt(),
			Arg::WRITE_INI.flag()
		};

		const auto [program_path, program_name] { opt::resolve_split_path(envp, args.arg0().value_or(std::string(argv[0]))) };

		if (args.check_flag(Arg::HELP.flag()) || args.check_opt(Arg::HELP.opt()))
			help::print(program_path, program_name);

		// Read all local INI files
		file::ini::INI ini;
		if (!INI_NAME_MUST_MATCH_PROGRAM_NAME)
			for (auto& file : file::getAllFilesWithExtension(program_path, ".ini"))
				ini.read(file.path().generic_string());
		else
			ini.read(file::replace_extension(program_name, ".ini").generic_string());

		const auto // get settings/args
			use_log{ str::tolower(ini.getv(Setting::USE_LOG).value_or("")) == "true" },
			dry_run{ args.check_flag(Arg::DRY.flag()) || args.check_opt(Arg::DRY.opt()) };

		if (dry_run)
			std::cout << sys::term::log << "Dry Run Enabled.\n";

		// Check if --write-ini was included
		if (args.check_flag(Arg::WRITE_INI.flag()) || args.check_opt(Arg::WRITE_INI.opt())) {
			// Get target INI filename
			const auto target_filename{ args.getv("write-ini").value_or(file::replace_extension(program_name, ".ini").generic_string()) };

			// If the dry run argument was included, don't actually write to file, just output to console.
			if (dry_run)
				std::cout << ini::getTemplate().rdbuf() << '\n';
			else
				if (!ini::writeTemplate(target_filename))
					throw std::exception(std::string("Failed to write default INI to \"" + target_filename + "\"").c_str());

			if (use_log)
				std::cout << sys::term::log << "Successfully wrote default INI to \"" << target_filename << "\"\n";
		}

		// if no valid INI settings were found, throw an exception
		if (ini.empty())
			throw std::exception(std::string("Couldn't find any valid INI files located in directory: \"" + program_path + "\"\n").c_str());

		if (const auto command{ ini.getv(Setting::COMMANDLINE) }; command.has_value()) {
			if (use_log)
				std::cout << sys::term::log << "Executing command \"" << command.value() << "\"\n";

			if (dry_run)
				std::cout << command.value() << '\n';
			else {
				const auto retcode{ exec(command.value().c_str()) };
				if (use_log)
					std::cout << sys::term::log << "\"" << command.value() << "\" returned " << retcode << "\n";
			}
		}
		else // INI container is NOT empty, but does not contain the required keys
			throw std::exception("Found a valid INI file, but it did not contain a valid command!");

		if (const auto pause{ ini.getv(Setting::PAUSE) }; pause.has_value() && str::tolower(pause.value()) == "true") {
			std::cout << "Press " << color::f::red << "<ENTER>" << color::reset << " to exit." << std::endl;
			std::string tmp{};
			std::getline(std::cin, tmp, '\n'); // block until enter key is pressed
		}

		return 0;
	} catch (std::exception& ex) {
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}