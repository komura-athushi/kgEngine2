#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	Engine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	auto& renderContext = g_graphicsEngine->GetRenderContext();

	Model model;
	ModelInitData initData;
	initData.m_tkmFilePath = "Assets/modelData/box.tkm";
	model.Init(initData);

	// ここからゲームループ。
	while (Engine().DispatchWindowMessage())
	{
		//レンダリング開始。
		g_graphicsEngine->BeginRender();

		model.Draw(renderContext);

		//レンダリング終了。
		g_graphicsEngine->EndRender();

		//デストローイ。
		DestroyWindow(g_hWnd);
	}
	return 0;
}