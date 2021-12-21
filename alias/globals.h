#pragma once
#include "version.h"

#include <xlog.hpp>
using namespace xlog;

#include <iostream>

namespace alias {
	static struct {

		inline void pauseBeforeExit()
		{
			if (pause_before_exit) {
				std::cout << "Press " << color::setcolor(color::red) << "<Enter>" << color::reset_f << " to exit." << std::endl;
				(void)std::cin.get();
			}
		}

		inline std::optional<int> getReturnCode() const noexcept
		{
			if (const auto p{ return_code.get() }; p != nullptr)
				return *p;
			return std::nullopt;
		}

		// Program Variables:
		std::unique_ptr<int> return_code{ nullptr };

		// Config Variables (& Object):
		std::string file_version{ ALIAS_VERSION };
		// header: target
		std::string command;
		// header: output
		bool allow_output{ true };
		bool pause_before_exit{ false };
		bool append_newline{ false };
		std::string out_file;
		// header: misc
		bool forward_args{ true };
		xLogs<std::ostream> log{ std::cerr, level::OnlyErrorsAndWarnings | level::MESSAGE };
	} Global;

	inline std::ostream& newline_if_enabled(std::ostream& os)
	{
		if (Global.append_newline)
			os << std::endl;
		return os;
	}
}