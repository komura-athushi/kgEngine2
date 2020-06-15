#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"
#include "graphics/GraphicsEngine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
//int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	//�Q�[���̏������B
	g_engine.InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	auto& renderContext = g_graphicsEngine->GetRenderContext();

	Model model;
	ModelInitData initData;
	initData.m_tkmFilePath = "Assets/modelData/box.tkm";
	initData.m_fxFilePath = "Assets/shader/model.fx";

	model.Init(initData);

	// ��������Q�[�����[�v�B
	while (g_engine.DispatchWindowMessage())
	{
		//�����_�����O�J�n�B
		g_graphicsEngine->BeginRender();

		model.Draw(renderContext);

		//�����_�����O�I���B
		g_graphicsEngine->EndRender();

		//�f�X�g���[�C�B
		//DestroyWindow(g_hWnd);
	}
	return 0;
}