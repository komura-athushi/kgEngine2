#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/ModelRender.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	g_engine.InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	auto& renderContext = g_graphicsEngine->GetRenderContext();

	Model model;
	ModelInitData initData;
	initData.m_tkmFilePath = "Assets/modelData/box.tkm";
	initData.m_fxFilePath = "Assets/shader/model.fx";

	model.Init(initData);

	AnimationClip animationClip[2];
	animationClip[0].Load(L"Assets/animData/unityChan/idle.tka");
	animationClip[0].SetLoopFlag(true);

	animationClip[1].Load(L"Assets/animData/unityChan/run.tka");
	animationClip[1].SetLoopFlag(true);

	ModelRender modelRender;
	modelRender.Init("Assets/modelData/unityChan.tkm", animationClip, 2);
	modelRender.PlayAnimation(0, 0.0f);

	//ModelRender box;
	//box.Init("Assets/modelData/box.tkm");
	int a = 0;
	// ここからゲームループ。
	while (g_engine.DispatchWindowMessage())
	{
		//レンダリング開始。
		g_graphicsEngine->BeginRender();

		//model.Draw(renderContext);

		modelRender.Update();
		modelRender.Draw();

		if (a >= 200) {
			modelRender.PlayAnimation(0, 0.0f);
		}
		else if (a >= 100) {
			modelRender.PlayAnimation(1, 0.0f);
		}
		a++;
		//box.Update();
		//box.Draw();

		//レンダリング終了。
		g_graphicsEngine->EndRender();

	}
	return 0;
}