#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/ModelRender.h"
#include "HID/GamePad.h"

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


	Sprite sprite;

	SpriteInitData spriteInitData;
	spriteInitData.m_ddsFilePath[0] = "Assets/sprite/a.dds";
	spriteInitData.m_fxFilePath = "Assets/shader/sprite.fx";
	spriteInitData.m_width = FRAME_BUFFER_W;
	spriteInitData.m_height = FRAME_BUFFER_H;
	sprite.Init(spriteInitData);

	ModelRender modelRender2;
	modelRender2.Init("Assets/modelData/box3.tkm");
	modelRender2.SetPosition(Vector3(0.0f, 0.0f, -180.0f));

	ModelRender modelRender3;
	modelRender3.Init("Assets/modelData/box4.tkm");
	modelRender3.SetPosition(Vector3(0.0f, 0.0f, 0.0f));

	ModelRender modelRender4;
	modelRender4.Init("Assets/modelData/box5.tkm");
	modelRender4.SetPosition(Vector3(-900.0f, 400.0f, 900.0f));

	ModelRender modelRender5;
	modelRender5.Init("Assets/modelData/box5.tkm");
	modelRender5.SetPosition(Vector3(500.0f, 00.0f, -300.0f));
	modelRender5.SetScale(Vector3::One * 3.0f);

	ModelRender modelRender6;
	modelRender6.Init("Assets/modelData/unityChan.tkm", animationClip, 2);
	modelRender6.PlayAnimation(0, 0.0f);
	modelRender6.SetPosition(Vector3(500.0f, 0.0f, 0.0f));
	modelRender6.SetScale(Vector3::One * 3.0f);
	
	ModelRender modelRender7;
	modelRender7.Init("Assets/modelData/unityChan.tkm", animationClip, 2);
	modelRender7.PlayAnimation(0, 0.0f);
	modelRender7.SetPosition(Vector3(800.0f, 0.0f, -1000.0f));

	ModelRender modelRender8;
	modelRender8.Init("Assets/modelData/unityChan.tkm", animationClip, 2);
	modelRender8.PlayAnimation(0, 0.0f);
	modelRender8.SetPosition(Vector3(1000.0f, 0.0f, 2000.0f));
	modelRender8.SetScale(Vector3::One * 3.0f);

	ModelRender modelRender9;
	modelRender9.Init("Assets/modelData/unityChan.tkm", animationClip, 2);
	modelRender9.PlayAnimation(0, 0.0f);
	modelRender9.SetPosition(Vector3(800.0f, 0.0f, 1000.0f));

	g_gamePad.Init(0);
	//modelRender3.SetShadowCaster(false);
	//ModelRender box;
	//box.Init("Assets/modelData/box.tkm");
	int a = 0;
	// ここからゲームループ。
	Vector3 pos = Vector3(00.0f, 400.0f, -700.0f);
	Vector3 target = Vector3(0.0f, 0.0f, 0.0f);
	g_camera3D->SetPosition(pos);
	g_camera3D->SetTarget(target);
	g_camera3D->Update();
	while (g_engine.DispatchWindowMessage())
	{
		g_gamePad.Update();
		Vector3 speed = Vector3::Zero;
		speed.x = g_gamePad.GetLStickXF();
		speed.z = g_gamePad.GetLStickYF();
		speed *= 15.0f;
		pos += speed;
		target += speed;
		g_camera3D->SetPosition(pos);
		g_camera3D->SetTarget(target);
		g_camera3D->Update();
		//レンダリング開始。
		g_graphicsEngine->BeginRender();

		//modelRender3.Update();

		modelRender2.Update();

		modelRender.Update();

		modelRender4.Update();

		modelRender5.Update();

		modelRender6.Update();

		modelRender7.Update();

		modelRender8.Update();

		modelRender9.Update();

		g_graphicsEngine->RendertoShadow();

		g_graphicsEngine->BeginDeferredRender();

		modelRender2.Draw();
		modelRender.Draw();
		//modelRender3.Draw();
		modelRender4.Draw();
		modelRender5.Draw();
		modelRender7.Draw();
		modelRender6.Draw();
		modelRender8.Draw();
		modelRender9.Draw();

		if (a >= 200) {
			//modelRender.PlayAnimation(0, 0.0f);
		}
		else if (a >= 100) {
			modelRender.PlayAnimation(1, 0.0f);
		}
		a++;
		//box.Update();
		//box.Draw();
	

		g_graphicsEngine->EndModelDraw();

		

		g_graphicsEngine->RendertoPostEffect();
		

		//レンダリング終了。
		g_graphicsEngine->EndRender();
		
	}
	return 0;
}