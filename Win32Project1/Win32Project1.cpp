// Win32Project1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32Project1.h"
#include "Direct2D.h"


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	Direct2D d2d;
	if (SUCCEEDED(d2d.Initialize()))
	{
		d2d.RunMessageLoop();
	}
	
	return 0;
}




//错误	3	error X3501: 'main': entrypoint not found	C:\Users\雨培\Documents\Visual Studio Projects\DirectXSamples\Tutorial06\FXC	Tutorial06
