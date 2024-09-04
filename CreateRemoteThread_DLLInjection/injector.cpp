// ファイル名：injector.cpp
// 説明：インジェクターで使用する関数などを定義しています

#include"injector.h"

// プロセス名からプロセスIDを取得する関数
// processName：検索するプロセス名
// processId：プロセスID（参照として受け取る）
// 戻り値：成功した場合はTRUE、失敗した場合はFALSEを返す
bool GetProcessIdToProcessName(std::wstring processName, DWORD& processId) {
    // プロセスのスナップショットを作成
    HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "[-] CreateToolhelp32Snapshotが失敗しました。Error code: " << GetLastError() << std::endl;
        return false;
    }

    // プロセス情報の初期化
    PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

    // 最初のプロセスを取得
    BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

    while (hasNextProcess) {
        // szExeFileをstd::wstringに変換
        std::wstring exeFile(processEntry.szExeFile);

        // プロセス名が一致するかチェック
        if (exeFile.find(processName) != std::wstring::npos) {
            processId = processEntry.th32ProcessID;
            CloseHandle(processSnapshotHandle);
            return true;
        }
        // 次のプロセスを取得
        hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
    }

    CloseHandle(processSnapshotHandle);
    return false;
}

// アドレスを16進数の文字列として表示する
// address：メモリアドレス
// 戻り値：メモリアドレスの16進数を含む文字列
std::string PrintAddress(LPVOID address) {
    std::ostringstream stream;
    stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
    return stream.str();
}