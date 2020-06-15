#pragma once

class GraphicsEngine;
extern HWND			g_hWnd;				//�E�B���h�E�n���h���B

LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



class CEngine
{
public:
	CEngine();
	~CEngine();
	//�Q�[���̏������B
	void InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName);
	//�E�B���h�E���b�Z�[�W���f�B�X�p�b�`�Bfalse���Ԃ��Ă�����A�Q�[���I���B
	bool DispatchWindowMessage();
	//�E�B���h�E�N���X�̏�����
	void InitWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName);
	//�C���X�^���X���擾
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

//�G���W���̃C���X�^���X���擾
static inline CEngine& Engine()
{
	return CEngine::GetInstance();
}