// �t�@�C�����Finjector.h
// �����F�C���W�F�N�^�[�ŕK�v�Ȋ֐��Ȃǂ�錾���Ă��܂�

#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>

bool GetProcessIdToProcessName(std::wstring processName, DWORD& processId);
LPVOID ReadFileBytes(std::wstring filePath, DWORD& fileSize);
std::string PrintAddress(LPVOID address);