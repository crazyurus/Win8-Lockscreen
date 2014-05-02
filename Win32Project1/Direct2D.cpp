#include "stdafx.h"
#include "Direct2D.h"

template<class Interface>
inline void Free(Interface **ppInterfaceToRelease)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

Direct2D::~Direct2D()
{
	Free(&pD2DFactory);
	Free(&pRT);
	Free(&pIWICImagingFatory);
	Free(&pIWICFormatConverter);
	Free(&pIWICBitmapFrameDecode);
	Free(&pIWICBitmapDecoder);
	Free(&pStyle);
}

Direct2D::Direct2D()
{
	CreateDeviceIndependentResources();
	ZeroMemory(Pathname, _countof(Pathname));
	ZeroMemory(IfCircles, _countof(IfCircles));
}


void Direct2D::CreateDeviceIndependentResources()
{
	HRESULT hr = E_FAIL;
	hr = CoInitialize(0);
	if (SUCCEEDED(hr)) hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			(LPVOID*)&pIWICImagingFatory);
	}
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"Sorry,can't init correctly.", L"Sorry", MB_ICONWARNING);
		exit(1);
	}
}


void Direct2D::CreateDeviceResources()
{
	HRESULT hr = E_FAIL;
	ID2D1GradientStopCollection *pID2D1GradientStopCollection = NULL;
	hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(static_cast<UINT>(rect.right - rect.left),
		static_cast<UINT>(rect.bottom - rect.top))), &pRT);
	if (SUCCEEDED(hr)) hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod,1),
		&pWhiteBrush);
	if (SUCCEEDED(hr)) hr = pD2DFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
		D2D1_CAP_STYLE_ROUND,
		D2D1_CAP_STYLE_ROUND,
		D2D1_CAP_STYLE_ROUND,
		D2D1_LINE_JOIN_ROUND), NULL, 0, &pStyle);
	if (SUCCEEDED(hr)) hr = pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Goldenrod, 1), &pRedBrush);
	
	if (SUCCEEDED(hr))
	{
		
		 const D2D1_GRADIENT_STOP stops[] = {
			{ 0.f,D2D1::ColorF(D2D1::ColorF::Green,1) },
			{ 0.5f, D2D1::ColorF(D2D1::ColorF::SkyBlue,1) },
			{ 1.f, D2D1::ColorF(D2D1::ColorF::Gold,1) },
		};
		hr = pRT->CreateGradientStopCollection(
			stops,
			ARRAYSIZE(stops),
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&pID2D1GradientStopCollection);
	}
	if (SUCCEEDED(hr)) hr = pRT->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(
		D2D1::Point2F(0.0f, 0.0f),
		D2D1::Point2F(static_cast<float>(rect.right), static_cast<float>(rect.bottom))
		),
		D2D1::BrushProperties(),
		pID2D1GradientStopCollection,
		&pBackBrush
		);
	Free(&pID2D1GradientStopCollection);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"Sorry,can't init correctly.", L"Sorry", MB_ICONWARNING);
		exit(1);
	}

}

void Direct2D::DrawSnow()
{
	if (IsWindow(hWnd)) SetActiveWindow(hWnd);
	ProcessDropping();
	if (IfUnlock)
	{
		SetLayeredWindowAttributes(hWnd, 0, (Alpha = Alpha - 5), LWA_ALPHA);
		if (Alpha <= 0) PostQuitMessage(0);	
	}
	if (pRT && pID2D1Bitmap)
	{
		pRT->BeginDraw();
		pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		DrawBackground();
		if (IfOver)
		{
			if (GetStart)
			{
				Drawlines();
			}
		}
		else
		{
			pRT->SetTransform(DropTranslation * translation);
			pRT->DrawBitmap(pID2D1Bitmap, rect);
		}
		if (FAILED(pRT->EndDraw()))
		{
			MessageBox(hWnd, L"Sorry,can't init correctly.", L"Sorry", MB_ICONWARNING);
			exit(1);
		}
	}
}

void Direct2D::resize(UINT width, UINT height)
{
	if (pRT)
	{
		pRT->Resize(D2D1::SizeU(width, height));
	}
}

void Direct2D::InitWIC()
{
	HRESULT hr = E_FAIL;
	hr = pIWICImagingFatory->CreateDecoderFromFilename(
		Pathname,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&pIWICBitmapDecoder);
	if (SUCCEEDED(hr)) hr = pIWICBitmapDecoder->GetFrame(0,&pIWICBitmapFrameDecode);
	if (SUCCEEDED(hr)) hr = pIWICImagingFatory->CreateFormatConverter(&pIWICFormatConverter);
	if (SUCCEEDED(hr)) hr = pIWICFormatConverter->Initialize(pIWICBitmapFrameDecode,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		NULL,
		0.0f,
		WICBitmapPaletteTypeCustom);
	if (SUCCEEDED(hr)&&pRT) hr = pRT->CreateBitmapFromWicBitmap(
		pIWICFormatConverter, NULL, &pID2D1Bitmap);
	if (FAILED(hr))
	{
		MessageBox(hWnd, L"Sorry,can't init correctly.", L"Sorry", MB_ICONWARNING);
		exit(1);
	}
}

LRESULT CALLBACK Direct2D::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Direct2D *pDirect2D = (Direct2D *)pcs->lpCreateParams;
		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pDirect2D)
			);
		
		MoveWindow(hwnd, 0, 0, static_cast<int>(pDirect2D->rect.right - pDirect2D->rect.left), static_cast<int>( pDirect2D->rect.bottom - pDirect2D->rect.top), TRUE);
		pDirect2D->resize(static_cast<UINT>(pDirect2D->rect.right - pDirect2D->rect.left), static_cast<UINT>( pDirect2D->rect.bottom - pDirect2D->rect.top));
		pDirect2D->SetStyle();
	}
	else
	{
		Direct2D *pDirect2D = reinterpret_cast<Direct2D *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA
			)));

		if (pDirect2D)
		{
			switch (message)
			{
			case WM_PAINT:
			{
				BeginPaint(hwnd, &ps);
				pDirect2D->DrawSnow();
				EndPaint(hwnd, &ps);
				break;
			}
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_KEYDOWN:
			case WM_KEYUP:
				return 0;
			case WM_LBUTTONDOWN:
			{
				pDirect2D->IfDown = TRUE;
				if (!pDirect2D->IfUnlock && !pDirect2D->IfOver)
				{
					pDirect2D->Release = FALSE;
					pDirect2D->StartY = GET_Y_LPARAM(lParam);
					pDirect2D->NowY = GET_Y_LPARAM(lParam);
					pDirect2D->LastY = pDirect2D->NowY;
				}
				break;
			}
			case WM_ACTIVATE:
				if (WA_INACTIVE == LOWORD(wParam))
				{
					SetActiveWindow(pDirect2D->hWnd);
					return 0;
				}
				break;
			case WM_MOUSEMOVE:
			{
				if (pDirect2D->IfDown)
				{
					if (!pDirect2D->IfUnlock && !pDirect2D->IfOver)
					{

						pDirect2D->LastY = pDirect2D->NowY;
						pDirect2D->NowY = GET_Y_LPARAM(lParam);
						SendMessage(pDirect2D->hWnd, WM_PAINT, 0, 0);
						break;
					}
					if (!pDirect2D->IfUnlock && pDirect2D->IfOver && pDirect2D->GetStart)
					{
						pDirect2D->NowPoint = {float( GET_X_LPARAM(lParam)),float( GET_Y_LPARAM(lParam) )};
						D2D1_POINT_2F Buffer;
						if (pDirect2D->IfinCircles(pDirect2D->NowPoint, Buffer))
						{
							pDirect2D->NowPoint = Buffer;
							pDirect2D->Points[pDirect2D->PointCount++] = Buffer;
							pDirect2D->StartPoint = Buffer;
						}
						SendMessage(pDirect2D->hWnd, WM_PAINT, 0, 0);
					}
					if (!pDirect2D->IfUnlock && pDirect2D->IfOver &&!pDirect2D->GetStart)
					{
						if (pDirect2D->IfinCircles(D2D1_POINT_2F{static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)) }, pDirect2D->StartPoint))
						{
							pDirect2D->NowPoint = pDirect2D->StartPoint;
							pDirect2D->GetStart = TRUE;
							pDirect2D->Points[pDirect2D->PointCount++] = pDirect2D->StartPoint;
						}
					}
					
				}
				break;
			}
			case WM_LBUTTONUP:
			{
				if (pDirect2D->IfDown)
				{
					if (!pDirect2D->IfUnlock && !pDirect2D->IfOver)
					{
						pDirect2D->IfDown = FALSE;
						pDirect2D->NowY = GET_Y_LPARAM(lParam);
						pDirect2D->Release = TRUE;
						SendMessage(pDirect2D->hWnd, WM_PAINT, 0, 0);
						while (pDirect2D->Droppping)
						{
							SendMessage(pDirect2D->hWnd, WM_PAINT, 0, 0);
						}
						break;
					}
					if (!pDirect2D->IfUnlock && pDirect2D->IfOver &&pDirect2D->GetStart)
					{
						pDirect2D->IfUnlock = TRUE;
						while (pDirect2D->Alpha)
						{
							SendMessage(pDirect2D->hWnd, WM_PAINT, 0, 0);
						}
					}
					

				}
				break;
			}
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_SIZE:
			{
				pDirect2D->SetStyle();
				break;
			}
			}
		}
		
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
HRESULT Direct2D::Initialize()
{
	HRESULT hr = S_FALSE;
	IMAGE_DOS_HEADER idh;
	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	/*hr = CreateDeviceIndependentResources();*/
	
		// Register the window class.
	    if(!GetPic()) return E_FAIL;
		InitCircles();
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Direct2D::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = (HINSTANCE)& idh;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.lpszClassName = L"Direct2D";

		RegisterClassEx(&wcex);

			// Create the application window.
			//
			// Because the CreateWindow function takes its size in pixels, we
			// obtain the system DPI and use it to scale the window size.
		FLOAT dpiX, dpiY;
		pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		hWnd = CreateWindow(
			L"Direct2D",
			L"Direct2D",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(640.f * dpiX / 96.f)),
			static_cast<UINT>(ceil(480.f * dpiY / 96.f)),
			NULL,
			NULL,
			wcex.hInstance,
			this
			);
		hr = hWnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			CreateDeviceResources();
			InitWIC();
			ShowWindow(hWnd, SW_SHOWNORMAL);
			UpdateWindow(hWnd);
		}
		ZeroMemory(&m_DwmTimingInfo, sizeof(m_DwmTimingInfo));
		m_DwmTimingInfo.cbSize = sizeof(m_DwmTimingInfo);

		// Get the composition refresh rate. If the DWM isn't running,
		// get the refresh rate from GDI -- probably going to be 60Hz
		if (FAILED(DwmGetCompositionTimingInfo(NULL, &m_DwmTimingInfo)))
		{
			HDC hdc = GetDC(hWnd);
			m_DwmTimingInfo.rateCompose.uiDenominator = 1;
			m_DwmTimingInfo.rateCompose.uiNumerator = GetDeviceCaps(hdc, VREFRESH);
			ReleaseDC(hWnd, hdc);
		}

		return hr;
}

void Direct2D::RunMessageLoop()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		CloseTask();
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, int(rect.right - rect.left), int(rect.bottom - rect.top), SWP_SHOWWINDOW);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DrawSnow();
		}
	}

}

void Direct2D::DrawBackground()
{
	pRT->FillRectangle(rect, pBackBrush);
	for (int i = 0; i < 9; i++)
	{
		pRT->DrawEllipse(Circles[i], pWhiteBrush, 5);
		pRT->FillEllipse(D2D1::Ellipse(Circles[i].point, 10, 10), pWhiteBrush);
	}
	
}

void Direct2D::CloseTask()
{
	HWND hTask = NULL;
	hTask = FindWindow(L"TaskManagerWindow", L"任务管理器");
	if (IsWindow(hTask))
	{
		DWORD dTask = 0;
		GetWindowThreadProcessId(hTask, &dTask);
		HANDLE hTa = OpenProcess(PROCESS_TERMINATE, FALSE, dTask);
		if (hTa)
		{
			TerminateProcess(hTa, 0);
			CloseHandle(hTa);
		}
	}
}

BOOL Direct2D::GetPic()
{
	OPENFILENAME ofn = { sizeof(OPENFILENAME) };
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = Pathname;
	ofn.lpstrFile[0] = 0;
	ofn.lpstrFilter = L"Pics\0*.JPG;*.PNG\0\0";
	ofn.nMaxFile = _countof(Pathname);
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = L"Choose a Pic";
	BOOL IfOk = GetOpenFileName(&ofn);
	wcscpy_s<MAX_PATH>(Pathname, ofn.lpstrFile);
	return IfOk;
}

void Direct2D::InitCircles()
{
	float start = rect.right / 3.0f + 50.0f;
	for (int i = 1; i < 4; i++)
	{
		for (int q = 1; q < 4; q++)
		{
			Circles[(i-1) *3 + q - 1] = D2D1::Ellipse(D2D1_POINT_2F{2* rect.right / 5 + 40 + ( q - 1 ) * 120, rect.bottom / 3 +50 + (i-1)*120}, 40, 40);
		}
	}
	
}
BOOL Direct2D::IfinCircles(D2D1_POINT_2F point, D2D1_POINT_2F& DirectPoint)
{
	for (int i = 0; i < 9; i++)
	{
		if (pow((point.x - Circles[i].point.x), 2) + pow((point.y - Circles[i].point.y), 2) <= 40.0f * 40.0f)
		{
			
			if (IfCircles[i])
			{
				return FALSE;
			}
			DirectPoint = Circles[i].point;
			IfCircles[i] = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}
void Direct2D::Drawlines()
{
	for (int i = 1; i < PointCount; i++)
	{
		pRT->DrawLine(Points[i], Points[i - 1], pWhiteBrush,5,pStyle);
	}
	pRT->DrawLine(StartPoint, NowPoint, pWhiteBrush,5,pStyle);
}
void Direct2D::SetStyle()
{
	int Style;
	Style = GetWindowLong(hWnd, GWL_STYLE);
	Style = Style &~WS_CAPTION;
	Style = Style &~WS_SYSMENU;
	Style = Style &~WS_SIZEBOX;
	SetWindowLong(hWnd, GWL_STYLE, Style);
	Style = GetWindowLong(hWnd, GWL_EXSTYLE);
	Style = Style | WS_EX_LAYERED;
	SetWindowLong(hWnd, GWL_EXSTYLE, Style);
	SetLayeredWindowAttributes(hWnd, RGB(255, 255, 255), 180, NULL);
}
void Direct2D::ProcessDropping()
{
	if (!IfDown && !Release)
	{
		translation = D2D1::Matrix3x2F::Translation(
			0,
			(NowY - LastY)< 0 ? static_cast<float>(NowY - StartY) : 0.0f);
	}
	else
	{
		translation = D2D1::Matrix3x2F::Translation(0, static_cast<float>(NowY - StartY));
	}

	if (Release)
	{
		drop.SetStart(static_cast<float>(StartY - NowY));
		drop.SetEnd(static_cast<float>(rect.bottom));
		drop.SetDuration(TotalTime);
		DropTranslation = translation;
		Droppping = TRUE;
	}
	if (Droppping)
	{
		translation = D2D1::Matrix3x2F::Translation(0, -drop.GetValue(time));
		time += static_cast<float>(m_DwmTimingInfo.rateCompose.uiDenominator) /
			static_cast<float>(m_DwmTimingInfo.rateCompose.uiNumerator);
	}
	if (time >= TotalTime)
	{
		Droppping = FALSE;
		IfOver = TRUE;
		time = 0.0f;
		LastY = 0;
		DropTranslation = D2D1::Matrix3x2F::Translation(0, 0);
		translation = D2D1::Matrix3x2F::Identity();
	}
}