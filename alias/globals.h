#pragma once
#include "version.h"

#include <str.hpp>
#include <xlog.hpp>
using namespace xlog;

#include <iostream>

namespace alias {
	using Version = std::tuple<short, short, short>;

	inline std::string version_to_string(const Version& v) noexcept { return str::stringify(std::get<0>(v), '.', std::get<1>(v), '.', std::get<2>(v)); }
	inline Version string_to_version(const std::string& strver)
	{
		if (std::all_of(strver.begin(), strver.end(), [](auto&& c) { return isdigit(c) || c == '.'; })) {
			auto& [major, v] {str::split(strver, '.')};
			auto& [minor, patch] {str::split(v, '.') };
			return{ str::stos(major), str::stos(minor), str::stos(patch) };
		}
		return{ 0, 0, 0 };
	}

	static struct {
		template<short i>
		inline bool check_file_version(const short& v) const noexcept
		{
			static_assert(i >= 0 && i <= 2, "Cannot check an out-of-bounds version index.");
			return std::get<i>(file_version) == v;
		}

		// Program Variables:
		// Config Variables (& Object):
		Version file_version{ 0, 0, 0 };
		// header: target
		std::string command;
		// header: output
		bool allow_output{ true };
		bool pause_before_exit{ false };
		bool append_newline{ false };
		std::string out_file;
		// header: misc
		bool forward_args{ true };

		// log manager
		xLogs<std::ostream> log{ std::cout, level::OnlyErrorsAndWarnings | level::MESSAGE };
	} Global;

	inline std::ostream& newline_if_enabled(std::ostream& os)
	{
		if (Global.append_newline)
			os << std::endl;
		return os;
	}
}