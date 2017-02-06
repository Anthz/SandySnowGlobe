#include "SnowGlobe.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	HINSTANCE instance = hPrevInstance;
	LPSTR cmdLine = lpCmdLine;
	SnowGlobe sg(hInstance, nShowCmd, "SandySnowGlobe", 1280, 800);

	if(!sg.Init(true))
		return 1;

	return sg.Run();
}