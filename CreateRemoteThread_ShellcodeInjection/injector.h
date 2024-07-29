// ファイル名：injector.h
// 説明：インジェクターで必要な関数などを宣言しています

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>

BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId);
LPVOID ReadFileBytes(LPCWSTR filePath, DWORD& fileSize);
std::string PrintAddress(LPVOID address);