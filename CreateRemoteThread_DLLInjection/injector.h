// ファイル名：injector.h
// 説明：インジェクターで必要な関数などを宣言しています

#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>

// 関数宣言
bool GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId);
std::string PrintAddress(LPVOID address);
