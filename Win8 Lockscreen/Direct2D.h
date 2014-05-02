#pragma once
#include "stdafx.h"
#include "Animation.h"
class Direct2D
{

public:
	Direct2D();
	
	void CreateDeviceIndependentResources();
	void CreateDeviceResources();
	void RunMessageLoop();
	HRESULT Initialize();
	~Direct2D();

private:

	BOOL GetPic();//获取图片路径
	TCHAR Pathname[MAX_PATH];//存储图片路径
	DWM_TIMING_INFO m_DwmTimingInfo;
	ID2D1Factory *pD2DFactory = NULL;
	ID2D1HwndRenderTarget *pRT = NULL;
	ID2D1SolidColorBrush *pWhiteBrush = NULL;
	ID2D1LinearGradientBrush *pBackBrush = NULL;
	ID2D1SolidColorBrush *pRedBrush = NULL;
	D2D1_ELLIPSE Circles[9];
	D2D1_POINT_2F Points[9];
	ID2D1StrokeStyle *pStyle = NULL;


	IWICImagingFactory *pIWICImagingFatory = NULL;
	IWICBitmapDecoder* pIWICBitmapDecoder = NULL;
	IWICBitmapFrameDecode* pIWICBitmapFrameDecode = NULL;
	IWICFormatConverter* pIWICFormatConverter = NULL;
	ID2D1Bitmap* pID2D1Bitmap = NULL;

	HWND hWnd;

	int NowY = 0;//存储当前鼠标的Y坐标
	int StartY = 0;//存储起点Y坐标
	int LastY = 0;//存储最后的Y坐标
	float time = 0.0f;//图片上升时的计时
	int Alpha = 255;//透明度
	int PointCount = 0;//解锁路径的点数
	D2D1::Matrix3x2F translation;

	BOOL IfOver = FALSE;//指示图片是否上升完毕
	BOOL IfDown = FALSE;//指示鼠标是否落下
	BOOL Release = FALSE;//指示鼠标是否松开
	BOOL Droppping = FALSE;//指示当前图片是否在上升
	BOOL IfUnlock = FALSE;//指示是否解锁完成
	BOOL GetStart = FALSE;//指示解锁的起点
	BOOL IfCircles[9];//9个圆

	Drop<float> drop;
	D2D1::Matrix3x2F DropTranslation = D2D1::Matrix3x2F::Translation(0, 0);
	const float TotalTime = 1.0f;
	const D2D1_RECT_F rect = D2D1::RectF(0, 0, static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), static_cast<float>(GetSystemMetrics(SM_CYSCREEN)));
	
	D2D1_POINT_2F StartPoint;//解锁的起点
	D2D1_POINT_2F NowPoint;//鼠标现在的点


	void DrawSnow();//负责绘制
	void DrawBackground();//负责绘制图片后面的背景
	void resize(UINT width, UINT height);
	void InitWIC();//初始化WIC
	void InitCircles();//初始化圆的数据
	void SetStyle();//设置窗口风格
	void ProcessDropping();//处理图片上升的参数
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
	void CloseTask();//关闭任务管理器
	void Drawlines();//负责绘制直线
	BOOL IfinCircles(D2D1_POINT_2F point, D2D1_POINT_2F& DirectPoint);//指示点是否在圆内
};

