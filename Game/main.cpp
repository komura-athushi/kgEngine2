#include "gameStdafx.h"
#include "kgEngine2.h"
#include "graphics/Model.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	Engine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, TEXT("Game"));

	auto& renderContext = g_graphicsEngine->GetRenderContext();

	Model model;
	ModelInitData initData;
	initData.m_tkmFilePath = "Assets/modelData/box.tkm";
	model.Init(initData);

	// ��������Q�[�����[�v�B
	while (Engine().DispatchWindowMessage())
	{
		//�����_�����O�J�n�B
		g_graphicsEngine->BeginRender();

		model.Draw(renderContext);

		//�����_�����O�I���B
		g_graphicsEngine->EndRender();

		//�f�X�g���[�C�B
		DestroyWindow(g_hWnd);
	}
	return 0;
}