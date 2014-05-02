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

	int NowY = 0;//实时更新的坐标
	int StartY = 0;//鼠标按下的坐标
	int LastY = 0;//鼠标松开时的坐标
	float time = 0.0f;//上升的计时
	int Alpha = 255;//透明度
	int PointCount = 0;//指示解锁路径点数
	D2D1::Matrix3x2F translation;

	BOOL IfOver = FALSE;//指示是否上升完毕
	BOOL IfDown = FALSE;//指示鼠标是否已按下
	BOOL Release = FALSE;//指示鼠标是否已释放
	BOOL Droppping = FALSE;//指示是否在上升
	BOOL IfUnlock = FALSE;//指示是否解锁完成
	BOOL GetStart = FALSE;//指示解锁路径已经确定起点
	BOOL IfCircles[9];//9个圆

	Drop<float> drop;
	D2D1::Matrix3x2F DropTranslation = D2D1::Matrix3x2F::Translation(0, 0);
	const float TotalTime = 1.0f;
	const D2D1_RECT_F rect = D2D1::RectF(0, 0, static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), static_cast<float>(GetSystemMetrics(SM_CYSCREEN)));
	
	D2D1_POINT_2F StartPoint;//解锁的起点
	D2D1_POINT_2F NowPoint;//解锁时实时更新的点


	void DrawSnow();//负责绘制
	void DrawBackground();//绘制背景
	void resize(UINT width, UINT height);
	void InitWIC();//初始化WIC
	void InitCircles();//把圆的参数设定好
	void SetStyle();//设置窗口的扩展风格
	void ProcessDropping();//处理窗口上拉的特效
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
	void CloseTask();//负责关闭任务管理器
	void Drawlines();//负责绘制解锁的路径
	BOOL IfinCircles(D2D1_POINT_2F point, D2D1_POINT_2F& DirectPoint);//判断解锁时点是否在圆内
};

