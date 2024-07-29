// ファイル名：main.cpp
// 説明：任意のプロセスに指定したDLLを強制ロードするプログラムです
// 使用方法：[インジェクター名] [ターゲットプロセス名] [DLLパス]

#include"injector.h"

// エントリポイント
// argv[1]：ターゲットプロセス名
// argv[2]：DLLのパス
// 戻り値：成功した場合は0、失敗した場合は1
int wmain(int argc, wchar_t* argv[]) {

	// 引数の数をチェック
	if (argc != 3) {
		std::cout << "使用方法：[インジェクター名] [プロセス名] [DLLパス]" << std::endl;
		return 1;
	}

	LPCWSTR processName = argv[1];
	LPCWSTR dllPath = argv[2];

	// 引数の内容をチェック
	if (wcslen(processName) == 0) {
		std::cerr << "[-] プロセス名が空です" << std::endl;
		return 1;
	}

	if (wcslen(dllPath) == 0) {
		std::cerr << "[-] DLLパスが空です" << std::endl;
		return 1;
	}

	// DLLパスのサイズを取得
	SIZE_T dllPathSize = (wcslen(dllPath) + 1) * sizeof(wchar_t);

	// プロセス名からプロセスIDを取得
	DWORD processId = 0;
	BOOL successGetPid = GetProcessIdToProcessName(processName, processId);
	if (!successGetPid) {
		std::cerr << "[-] プロセス名からプロセスIDを取得できませんでした" << std::endl;
		return 1;
	}

	// 各種情報を表示
	std::wcout
		<< "[DLLインジェクター]\n"
		<< "プロセス名：" << processName << "\n"
		<< "プロセスID：" << processId << "\n"
		<< "DLLパス：" << dllPath << std::endl;

	// プロセスハンドルを取得
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (processHandle == NULL) {
		std::cerr << "[-] OpenProcessが失敗しました。Error code: " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "[+] プロセスハンドルを取得しました" << std::endl;

	// DLLパスを書き込むメモリ領域を確保
	LPVOID dllPathMemory = VirtualAllocEx(processHandle, nullptr, dllPathSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (dllPathMemory == NULL) {
		std::cerr << "[-] VirtualAllocExが失敗しました。Error code: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] メモリを確保しました " << PrintAddress(dllPathMemory) << std::endl;

	// DLLパスを書き込む
	BOOL successWriteMemory = WriteProcessMemory(processHandle, dllPathMemory, dllPath, dllPathSize, nullptr);
	if (!successWriteMemory) {
		std::cerr << "[-] WriteProcessMemoryが失敗しました。Error code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] DLLパスを書き込みました " << std::endl;

	// リモートスレッドを作成してLoadLibraryを実行
	HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryW), dllPathMemory, 0, nullptr);
	if (threadHandle == NULL) {
		std::cerr << "[-] CreateRemoteThreadが失敗しました。Error code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] リモートスレッドを作成しました " << std::endl;

	// リモートスレッドの終了を待機してからクリーンアップ
	WaitForSingleObject(threadHandle, INFINITE);
	VirtualFreeEx(processHandle, dllPathMemory, 0, MEM_RELEASE);
	CloseHandle(threadHandle);
	CloseHandle(processHandle);
	return 0;
}