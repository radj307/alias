#pragma once
#include <make_exception.hpp>

#include <iostream>
#include <array>

namespace alias {
	struct Exec {
		const std::string& command;

		Exec(const std::string& command) : command{ command } {}

		friend std::ostream& operator<<(std::ostream& os, const Exec& obj)
		{
			auto pipe(_popen(obj.command.c_str(), "r"));
			if (!pipe) throw make_exception("Exec failed to open the pipe!");
			for (std::array<char, 128> buffer{}; fread(&buffer, 1, buffer.size() - 1, pipe) != 0u; os << buffer.data()) {}
			if (_pclose(pipe) == -1) throw make_exception("Exec failed to close the pipe!");
			return os;
		}
	};

	/// @brief	No output command
	inline bool exec(const char* command)
	{
		auto pipe(_popen(command, "r"));
		if (!pipe) throw make_exception("Exec failed to open the pipe!");
		for (std::array<char, 128> buffer{}; fread(&buffer, 1, buffer.size() - 1, pipe) != 0u; ) {}
		return _pclose(pipe) != -1;
	}
}