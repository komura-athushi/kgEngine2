#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"
#include "graphics/GraphicsEngine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//ゲームの初期化。
	g_engine.InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	auto& renderContext = g_graphicsEngine->GetRenderContext();

	Model model;
	ModelInitData initData;
	initData.m_tkmFilePath = "Assets/modelData/box.tkm";
	initData.m_fxFilePath = "Assets/shader/model.fx";

	model.Init(initData);

	// ここからゲームループ。
	while (g_engine.DispatchWindowMessage())
	{
		//レンダリング開始。
		g_graphicsEngine->BeginRender();

		model.Draw(renderContext);

		//レンダリング終了。
		g_graphicsEngine->EndRender();

		//デストローイ。
		//DestroyWindow(g_hWnd);
	}
	return 0;
}