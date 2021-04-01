#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <string>
#include <random>

namespace Utils
{
	void* FindPattern(const char* szModule, const char* pattern, const char* mask);
	std::string RandomString(size_t len);
}