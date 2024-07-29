// ファイル名：injector.cpp
// 説明：インジェクターで使用する関数などを定義しています

#include"injector.h"

// プロセス名からプロセスIDを取得する関数
// processName：検索するプロセス名
// processId：プロセスID（参照として受け取る）
// 戻り値：成功した場合はTRUE、失敗した場合はFALSEを返す
BOOL GetProcessIdToProcessName(LPCWSTR processName, DWORD& processId) {
	// プロセスのスナップショットを作成
	HANDLE processSnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, processId);
	if (processSnapshotHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "[-] CreateToolhelp32Snapshotが失敗しました。Error code: " << GetLastError() << std::endl;
		return FALSE;
	}

	// プロセス情報の初期化
	PROCESSENTRY32 processEntry = { sizeof(PROCESSENTRY32) };

	// 最初のプロセスを取得
	BOOL hasNextProcess = Process32First(processSnapshotHandle, &processEntry);

	while (hasNextProcess) {
		if (wcsstr(processEntry.szExeFile, processName) != NULL) {
			processId = processEntry.th32ProcessID;
			return TRUE;
		}
		// 次のプロセスを取得
		hasNextProcess = Process32Next(processSnapshotHandle, &processEntry);
	}

	CloseHandle(processSnapshotHandle);
	return FALSE;
}

// 指定されたファイルを開き、その内容をメモリに読み込んで先頭アドレスを返す関数
// filePath：読み込むファイルのパス
// fileSize：読み込まれたファイルのサイズ（参照として受け取る）
// 戻り値：ファイルの先頭アドレス
LPVOID ReadFileBytes(LPCWSTR filePath, DWORD& fileSize) {

	// ファイルのハンドルを取得
	HANDLE fileHandle = CreateFile(filePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE) {
		std::cerr << "CreateFileが失敗しました。 Error code: " << GetLastError() << std::endl;
		return nullptr;
	}

	// ファイルのサイズを取得
	fileSize = GetFileSize(fileHandle, nullptr);
	if (fileSize == INVALID_FILE_SIZE) {
		std::cerr << "GetFileSizeが失敗しました。" << std::endl;
		CloseHandle(fileHandle);
		return nullptr;
	}

	// ファイルサイズ分のメモリを確保
	LPVOID fileBytes = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
	if (fileBytes == nullptr) {
		std::cerr << "HeapAllocが失敗しました。" << std::endl;
		CloseHandle(fileHandle);
		return nullptr;
	}

	// データを読み取る
	BOOL successRead = ReadFile(fileHandle, fileBytes, fileSize, nullptr, nullptr);
	if (!successRead) {
		std::cerr << "ReadFileが失敗しました。 Error code: " << GetLastError() << std::endl;
		HeapFree(GetProcessHeap(), 0, fileBytes);
		CloseHandle(fileHandle);
		return nullptr;
	}

	CloseHandle(fileHandle);
	return fileBytes;
}

// アドレスを16進数の文字列として表示する
// address：メモリアドレス
// 戻り値：メモリアドレスの16進数を含む文字列
std::string PrintAddress(LPVOID address) {
	std::ostringstream stream;
	stream << std::showbase << std::hex << reinterpret_cast<uintptr_t>(address);
	return stream.str();
}