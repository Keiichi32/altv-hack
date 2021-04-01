#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "minhook/include/MinHook.h"
#include "cpp-sdk/IPackage.h"
#include "cpp-sdk/ICore.h"

namespace Hacks
{
	void Initialize(LPVOID, LPVOID, LPVOID, LPVOID, LPVOID, LPVOID, LPVOID, LPVOID);
	void LoadFile(const std::string&);
	void ChangeSerial(const std::string&);
	void Remove(const std::string&);
	void BlockWeaponCheck();
	void Dump();
}