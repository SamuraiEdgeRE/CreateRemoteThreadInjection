// �t�@�C�����Finjector.h
// �����F�C���W�F�N�^�[�ŕK�v�Ȋ֐��Ȃǂ�錾���Ă��܂�

#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sstream>

// �֐��錾
bool GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId);
std::string PrintAddress(LPVOID address);
