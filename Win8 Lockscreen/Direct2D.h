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

	BOOL GetPic();//��ȡͼƬ·��
	TCHAR Pathname[MAX_PATH];//�洢ͼƬ·��
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

	int NowY = 0;//ʵʱ���µ�����
	int StartY = 0;//��갴�µ�����
	int LastY = 0;//����ɿ�ʱ������
	float time = 0.0f;//�����ļ�ʱ
	int Alpha = 255;//͸����
	int PointCount = 0;//ָʾ����·������
	D2D1::Matrix3x2F translation;

	BOOL IfOver = FALSE;//ָʾ�Ƿ��������
	BOOL IfDown = FALSE;//ָʾ����Ƿ��Ѱ���
	BOOL Release = FALSE;//ָʾ����Ƿ����ͷ�
	BOOL Droppping = FALSE;//ָʾ�Ƿ�������
	BOOL IfUnlock = FALSE;//ָʾ�Ƿ�������
	BOOL GetStart = FALSE;//ָʾ����·���Ѿ�ȷ�����
	BOOL IfCircles[9];//9��Բ

	Drop<float> drop;
	D2D1::Matrix3x2F DropTranslation = D2D1::Matrix3x2F::Translation(0, 0);
	const float TotalTime = 1.0f;
	const D2D1_RECT_F rect = D2D1::RectF(0, 0, static_cast<float>(GetSystemMetrics(SM_CXSCREEN)), static_cast<float>(GetSystemMetrics(SM_CYSCREEN)));
	
	D2D1_POINT_2F StartPoint;//���������
	D2D1_POINT_2F NowPoint;//����ʱʵʱ���µĵ�


	void DrawSnow();//�������
	void DrawBackground();//���Ʊ���
	void resize(UINT width, UINT height);
	void InitWIC();//��ʼ��WIC
	void InitCircles();//��Բ�Ĳ����趨��
	void SetStyle();//���ô��ڵ���չ���
	void ProcessDropping();//���������������Ч
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);
	void CloseTask();//����ر����������
	void Drawlines();//������ƽ�����·��
	BOOL IfinCircles(D2D1_POINT_2F point, D2D1_POINT_2F& DirectPoint);//�жϽ���ʱ���Ƿ���Բ��
};

