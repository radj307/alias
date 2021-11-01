#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <strmanip.hpp>
#include <INI.hpp>
#include <fileio.hpp>

namespace commandalias {
	/**
	 * @brief Default maximum width value used when printing help.
	 */
	inline constexpr std::streamsize DEFAULT_MARGIN_WIDTH{ 30ll };

	/**
	 * @brief Wrapper object for a commandline argument. Used to standardize argument names & access methods.
	 */
	struct Arg {
		const std::string _name_opt;
		const char _name_flag;

		constexpr Arg(std::string opt, char flag) : _name_opt{ std::move(opt) }, _name_flag{ std::move(flag) } {}

		constexpr operator std::string() const { return _name_opt; }
		constexpr operator char() const { return _name_flag; }

		constexpr std::string opt() const { return this->operator std::string(); }
		constexpr char flag() const { return this->operator char(); }

		friend std::ostream& operator<<(std::ostream& os, const Arg& obj)
		{
			os << obj._name_flag << "  " << obj._name_opt << std::setw(DEFAULT_MARGIN_WIDTH - obj._name_opt.size() - 3ll) << ' ';
			return os;
		}

		static const Arg HELP, DRY, WRITE_INI;
	};
	inline const Arg
		Arg::HELP{ "help", 'h' },
		Arg::DRY{ "dry", 'd' },
		Arg::WRITE_INI{ "write-ini", 'i' };


	struct Setting : file::ini::KeyHelper {
		constexpr Setting(std::string header, std::string key) : file::ini::KeyHelper(std::forward<std::string>(header), std::forward<std::string>(key)) {}
		// Declare Settings
		static const Setting COMMANDLINE, USE_LOG, PAUSE;
	};
	// Define Settings
	inline const Setting
		Setting::COMMANDLINE{ "target", "commandline" },
		Setting::USE_LOG{ "console", "use_log" },
		Setting::PAUSE{ "console", "pause" };

	namespace help {
		inline void print(const std::string& program_path, const std::string& program_name)
		{
			std::cout
				<< "USAGE:\n"
				<< program_name << " [OPTIONS]\n"
				<< '\n'
				<< "OPTIONS:\n"
				<< Arg::HELP << "Shows this help display.\n"
				<< Arg::DRY << "Dry run, does not execute or write anything, instead outputs commands/files to STDOUT.\n"
				<< Arg::WRITE_INI << "Write a default INI config file to the given filename. If no filename is included, uses \"" << file::replace_extension(program_name, ".ini").generic_string() << "\"\n"
				<< '\n'
				<< "INI:\n"
				<< "The program will search for INI files located in the same directory as the executable.\n"
				<< "Currently: \"" << program_path << "\"\n";
		}
	}

	namespace ini {
		inline std::stringstream getTemplate()
		{
			std::stringstream buffer;
			buffer
				<< "[target]\n"
				<< Setting::COMMANDLINE.key << " = \"\"\n"
				<< '\n'
				<< "[console]\n"
				<< Setting::USE_LOG.key << " = true\n"
				<< Setting::PAUSE.key << " = false\n";
			return buffer;
		}

		inline bool writeTemplate(const std::string& filename)
		{
			return file::write(filename, std::move(getTemplate()), false);
		}
	}
}