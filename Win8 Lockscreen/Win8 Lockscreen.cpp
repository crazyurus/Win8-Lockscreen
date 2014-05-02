// Win8 Lockscreen.cpp 
//

#include "stdafx.h"
#include "Win8 Lockscreen.h"
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