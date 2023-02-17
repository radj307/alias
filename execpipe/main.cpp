/**
 * This code opens a process pipe and live-prints the received STDOUT data.
 */
#include <sysarch.h>
#include <TermAPI.hpp>
#include <make_exception.hpp>
#include <process.hpp>

#include <cstdio>
#include <iostream>
#include <array>

#ifdef OS_WIN
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

inline static WINCONSTEXPR std::string merge_args(const int& sz, char** arr, const int& off = 1)
{
	std::string str;
	str.reserve(static_cast<size_t>(sz) * 10ull);
	for (auto i{ off }; i < sz; ++i) {
		str += arr[i];
		if (i + 1 < sz) // add a space between each argument
			str += ' ';
	}
	str.shrink_to_fit();
	return str;
}

int main(const int argc, char** argv)
{
	try {
		process::Proc p(merge_args(argc, argv));
		std::cout << term::EnableANSI << p << std::endl;
		return p.close();
	} catch (const std::exception& ex) {
		std::cerr << term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << term::crit << "An unknown exception occurred!" << std::endl;
		return -1;
	}
}