// �t�@�C�����Finjector.cpp
// �����F�C���W�F�N�^�[�Ŏg�p����֐��Ȃǂ��`���Ă��܂�

#include"injector.h"

// �v���Z�X������v���Z�XID���擾����֐�
// processName�F��������v���Z�X��
// processId�F�v���Z�XID�i�Q�ƂƂ��Ď󂯎��j
// �߂�l�F���������ꍇ��TRUE�A���s�����ꍇ��FALSE��Ԃ�
bool GetProcessIdToProcessName(std::wstring processName, DWORD& processId) {
    // �v���Z�X�̃X�i�b�v�V���b�g���쐬
    HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "[-] CreateToolhelp32Snapshot�����s���܂����BError code: " << GetLastError() << std::endl;
        return false;
    }

    // �v���Z�X���̏�����
    PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

    // �ŏ��̃v���Z�X���擾
    BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

    while (hasNextProcess) {
        // szExeFile��std::wstring�ɕϊ�
        std::wstring exeFile(processEntry.szExeFile);

        // �v���Z�X������v���邩�`�F�b�N
        if (exeFile.find(processName) != std::wstring::npos) {
            processId = processEntry.th32ProcessID;
            CloseHandle(processSnapshotHandle);
            return true;
        }
        // ���̃v���Z�X���擾
        hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
    }

    CloseHandle(processSnapshotHandle);
    return false;
}

// �A�h���X��16�i���̕�����Ƃ��ĕ\������
// address�F�������A�h���X
// �߂�l�F�������A�h���X��16�i�����܂ޕ�����
std::string PrintAddress(LPVOID address) {
    std::ostringstream stream;
    stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
    return stream.str();
}