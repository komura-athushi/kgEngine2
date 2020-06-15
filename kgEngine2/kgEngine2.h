#pragma once

class GraphicsEngine;
extern HWND			g_hWnd;				//ウィンドウハンドル。

LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class CEngine
{
public:
	CEngine();
	~CEngine();
	//ゲームの初期化。
	void InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName);
	//ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
	bool DispatchWindowMessage();
	//ウィンドウクラスの初期化
	void InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName);
	//インスタンスを取得
	static CEngine& GetInstance()
	{
		static CEngine* instance = nullptr;
		if (instance == nullptr) {
			instance = new CEngine;
		}
		return *instance;
	}
private:
};

//エンジンのインスタンスを取得
static inline CEngine& Engine()
{
	return CEngine::GetInstance();
}