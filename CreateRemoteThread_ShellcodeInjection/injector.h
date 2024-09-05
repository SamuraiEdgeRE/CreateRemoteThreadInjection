// ファイル名：injector.h
// 説明：インジェクターで必要な関数などを宣言しています

#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>

bool GetProcessIdToProcessName(std::wstring processName, DWORD& processId);
LPVOID ReadFileBytes(std::wstring filePath, DWORD& fileSize);
std::string PrintAddress(LPVOID address);