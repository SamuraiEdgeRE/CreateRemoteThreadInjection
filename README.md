# CreateRemoteThreadInjection

## プロジェクトの概要
このプロジェクトではCreateRemoteThread関数を使った古典的なプロセスインジェクションツールを実装しました。  
プロセスインジェクションは特定のプロセスメモリ空間に外部からコードを書き込んで実行する手法です。  
これにより、ターゲットプロセスのメモリ内で任意のコードを実行することが可能となるため、ウイルス対策ソフトの検出回避、アプリの改造、WindowsAPI関数のフックなどの目的でよく使用されます。

## プロジェクトの目的
このツールの目的は、プロセスインジェクション技術を学び、その実装方法を理解することです。また、セキュリティ研究やマルウェア解析の教育用途にも利用されることを目指しています。

## 使い方  
<pre>
.\[インジェクター名] [プロセス名] [ファイルパス]
</pre>

#### 例：```.\CreateRemoteThreadShellcodeInjection.exe Notepad.exe "C:\Code\TestShellcode\x64\Release\TestShellcode.bin"```
(メモ帳プロセスにTest.binをシェルコードインジェクションインジェクション)

## CreateRemoteThreadインジェクションの詳細
CreateRemoteThreadは別プロセスにスレッドを作成するWindowsAPI関数です。
プロセスインジェクションにおいては、リモートプロセス上でのWindowsローダー関数やシェルコードの実行を担当します。

### DLLインジェクション

#### 1.OpenProcess関数でターゲットプロセスのハンドルを取得する

#### 2.VirtualAllocEx関数でDLLパスを書き込めるだけのメモリ領域を確保する

#### 3.WriteProcessMemory関数で確保したメモリ領域にDLLパスを書き込む

#### 4.CreateRemoteThreadでリモートプロセスにスレッドを作成
スレッドの開始アドレス(第4引数)にLoadLibraryのポインタを、スレッド開始関数に渡される引数としてはDLLパスのポインタを渡します。

### シェルコードインジェクション

#### 1.OpenProcess関数でターゲットプロセスのハンドルを取得する

#### 2.VirtualAllocEx関数でシェルコードを書き込めるだけのメモリ領域を確保する
後続の操作で作成する、スレッドが実行するメモリ領域のため、第5引数に*PAGE_EXECUTE_READWRITE*を渡して実行可能属性を付与しておきます

#### 3.WriteProcessMemory関数で確保したメモリ領域にシェルコードを書き込む

#### 4.CreateRemoteThread関数でリモートプロセスにスレッドを作成
スレッドの開始アドレス(第4引数)にシェルコードのポインタを渡します。
