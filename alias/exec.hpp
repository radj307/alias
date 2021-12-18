#pragma once
#include <sysarch.h>
#include <make_exception.hpp>

#include <iostream>
#include <array>

#ifdef OS_WIN
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace alias {
	struct Exec {
		const std::string& command;

		Exec(const std::string& command) : command{ command } {}

		friend std::ostream& operator<<(std::ostream& os, const Exec& obj)
		{
			auto pipe(POPEN(obj.command.c_str(), "r"));
			if (!pipe) throw make_exception("Exec failed to open the pipe!");
			for (std::array<char, 128> buffer{}; fread(&buffer, 1, buffer.size() - 1, pipe) != 0u; os << buffer.data()) {}
			if (*(Global.return_code = std::make_unique<int>(PCLOSE(pipe))).get() == -1) throw make_exception("Exec failed to close the process!");
			return os;
		}
	};

	/**
	 * @brief			Execute a command, discard the output from it, and return the code it returned, or -1.
	 * @param command	The command to execute.
	 * @returns			int
	 *					This is the return value of the process.
	 */
	inline int exec(const char* command) noexcept(false)
	{
		auto pipe(POPEN(command, "r"));
		if (!pipe) throw make_exception("popen() failed!");
		for (std::array<char, 128> buffer{}; fread(&buffer, 1, buffer.size() - 1, pipe) != 0u; ) {}
		const auto rc{ PCLOSE(pipe) };
		if (rc == -1) throw make_exception("pclose() failed!");
		Global.return_code = std::make_unique<int>(rc);
		return rc;
	}
}