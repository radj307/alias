#pragma once
#include <xlog.hpp>
using namespace xlog;

#include <iostream>

namespace alias {
	static struct {
		xLogs<std::ostream> log{ std::cerr };

		// header: target
		std::string command;
		// header: output
		bool allow_output{ true };
		bool append_newline{ false };
		std::string out_file;
		// header: misc
		bool pass_args{ true };
	} Global;
}