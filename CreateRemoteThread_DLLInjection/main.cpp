// �t�@�C�����Fmain.cpp
// �����F�C�ӂ̃v���Z�X�Ɏw�肵��DLL���������[�h����v���O�����ł�
// �g�p���@�F[�C���W�F�N�^�[��] [�^�[�Q�b�g�v���Z�X��] [DLL�p�X]

#include"injector.h"

// �G���g���|�C���g
// argv[1]�F�^�[�Q�b�g�v���Z�X��
// argv[2]�FDLL�̃p�X
// �߂�l�F���������ꍇ��0�A���s�����ꍇ��1
int wmain(int argc, wchar_t* argv[]) {

	// �����̐����`�F�b�N
	if (argc != 3) {
		std::cout << "�g�p���@�F[�C���W�F�N�^�[��] [�v���Z�X��] [DLL�p�X]" << std::endl;
		return 1;
	}

	LPCWSTR processName = argv[1];
	LPCWSTR dllPath = argv[2];

	// �����̓��e���`�F�b�N
	if (wcslen(processName) == 0) {
		std::cerr << "[-] �v���Z�X������ł�" << std::endl;
		return 1;
	}

	if (wcslen(dllPath) == 0) {
		std::cerr << "[-] DLL�p�X����ł�" << std::endl;
		return 1;
	}

	// DLL�p�X�̃T�C�Y���擾
	SIZE_T dllPathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);

	// �v���Z�X������v���Z�XID���擾
	DWORD processId = 0;
	BOOL successGetPid = GetProcessIdToProcessName(processName, processId);
	if (!successGetPid) {
		std::cerr << "[-] �v���Z�X������v���Z�XID���擾�ł��܂���ł���" << std::endl;
		return 1;
	}

	// �e�����\��
	std::wcout
		<< "[DLL�C���W�F�N�^�[]\n"
		<< "�v���Z�X���F" << processName << "\n"
		<< "�v���Z�XID�F" << processId << "\n"
		<< "DLL�p�X�F" << dllPath << std::endl;

	// �v���Z�X�n���h�����擾
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (processHandle == NULL) {
		std::cerr << "[-] OpenProcess�����s���܂����BError code: " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "[+] �v���Z�X�n���h�����擾���܂���" << std::endl;

	// DLL�p�X���������ރ������̈���m��
	LPVOID dllPathMemory = VirtualAllocEx(processHandle, nullptr, dllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (dllPathMemory == NULL) {
		std::cerr << "[-] VirtualAllocEx�����s���܂����BError code: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] ���������m�ۂ��܂��� " << PrintAddress(dllPathMemory) << std::endl;

	// DLL�p�X����������
	BOOL successWriteMemory = WriteProcessMemory(processHandle, dllPathMemory, dllPath, dllPathSize, nullptr);
	if (!successWriteMemory) {
		std::cerr << "[-] WriteProcessMemory�����s���܂����BError code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] DLL�p�X���������݂܂��� " << std::endl;

	// �����[�g�X���b�h���쐬����LoadLibrary�����s
	HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), dllPathMemory, 0, nullptr);
	if (threadHandle == NULL) {
		std::cerr << "[-] CreateRemoteThread�����s���܂����BError code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] �����[�g�X���b�h���쐬���܂��� " << std::endl;

	// �����[�g�X���b�h�̏I����ҋ@���Ă���N���[���A�b�v
	WaitForSingleObject(threadHandle, INFINITE);
	VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
	CloseHandle(threadHandle);
	CloseHandle(processHandle);
	return 0;
}