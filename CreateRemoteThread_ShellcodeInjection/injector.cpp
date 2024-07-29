// �t�@�C�����Finjector.cpp
// �����F�C���W�F�N�^�[�Ŏg�p����֐��Ȃǂ��`���Ă��܂�

#include"injector.h"

// �v���Z�X������v���Z�XID���擾����֐�
// processName�F��������v���Z�X��
// processId�F�v���Z�XID�i�Q�ƂƂ��Ď󂯎��j
// �߂�l�F���������ꍇ��TRUE�A���s�����ꍇ��FALSE��Ԃ�
BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId) {
	// �v���Z�X�̃X�i�b�v�V���b�g���쐬
	HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
	if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshot�����s���܂����BError code: " << GetLastError() << std::endl;
		return FALSE;
	}

	// �v���Z�X���̏�����
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

	// �ŏ��̃v���Z�X���擾
	BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

	while (hasNextProcess) {
		if (wcsstr(processEntry.szExeFile, processName) != NULL) {
			processId = processEntry.th32ProcessID;
			return TRUE;
		}
		// ���̃v���Z�X���擾
		hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
	}

	CloseHandle(processSnapshotHandle);
	return FALSE;
}

// �w�肳�ꂽ�t�@�C�����J���A���̓��e���������ɓǂݍ���Ő擪�A�h���X��Ԃ��֐�
// filePath�F�ǂݍ��ރt�@�C���̃p�X
// fileSize�F�ǂݍ��܂ꂽ�t�@�C���̃T�C�Y�i�Q�ƂƂ��Ď󂯎��j
// �߂�l�F�t�@�C���̐擪�A�h���X
LPVOID ReadFileBytes(LPCWSTR filePath, DWORD& fileSize) {

	// �t�@�C���̃n���h�����擾
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "CreateFile�����s���܂����B Error code: " << GetLastError() << std::endl;
		return nullptr;
	}

	// �t�@�C���̃T�C�Y���擾
	fileSize = GetFileSize(fileHandle, nullptr);
	if (fileSize == INVALID_FILE_SIZE) {
		std::cerr << "GetFileSize�����s���܂����B" << std::endl;
		CloseHandle(fileHandle);
		return nullptr;
	}

	// �t�@�C���T�C�Y���̃��������m��
	LPVOID fileBytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
	if (fileBytes == nullptr) {
		std::cerr << "HeapAlloc�����s���܂����B" << std::endl;
		CloseHandle(fileHandle);
		return nullptr;
	}

	// �f�[�^��ǂݎ��
	BOOL successRead = ReadFile(fileHandle, fileBytes, fileSize, nullptr, nullptr);
	if (!successRead) {
		std::cerr << "ReadFile�����s���܂����B Error code: " << GetLastError() << std::endl;
		HeapFree(GetProcessHeap(), 0, fileBytes);
		CloseHandle(fileHandle);
		return nullptr;
	}

	CloseHandle(fileHandle);
	return fileBytes;
}

// �A�h���X��16�i���̕�����Ƃ��ĕ\������
// address�F�������A�h���X
// �߂�l�F�������A�h���X��16�i�����܂ޕ�����
std::string PrintAddress(LPVOID address) {
	std::ostringstream stream;
	stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
	return stream.str();
}