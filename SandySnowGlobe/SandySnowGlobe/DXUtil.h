#pragma once
#include <d3d11.h>
#include <string>
#include <time.h>
#include <iostream>
#include <fstream>

#pragma comment(lib, "d3d11.lib")

namespace Memory
{
	template <typename T> void SafeRelease(T *t);
	template <typename T> void SafeDelete(T *t);
	template <typename T> void SafeDeleteArr(T *t);
}

namespace Validation
{
	bool ErrCheck(HRESULT result, char const *file, const int &line, const std::string &desc);
}

namespace Logger
{
	void Log(const std::string &msg);
	void InitLogFile(const std::string &folderPath = "Logs");
}

#include "DXUtil.inl"


