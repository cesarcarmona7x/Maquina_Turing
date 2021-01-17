#ifndef D2DHANDLE_INCLUDED
#define D2DHANDLE_INCLUDED
#include "stdafx.h"
class D2DHandle{
public:
	D2DHandle(){}
	~D2DHandle(){}
	bool InitializeD2D(HWND hwnd,GameSettings& settings);
	ComPtr<ID2D1Factory>d2dfactory;
	ComPtr<ID2D1HwndRenderTarget>target;
	ComPtr<IDWriteFactory>dwritefactory;
	ComPtr<ID2D1SolidColorBrush>solidbrush;
	ComPtr<ID2D1LinearGradientBrush>lineargradient;
	ComPtr<ID2D1RadialGradientBrush>radialgradient;
	ComPtr<ID2D1BitmapBrush>bmpbrush;
	ComPtr<IWICImagingFactory> imgfactory;
	ComPtr<IWICBitmap> wicbitmap;
	ComPtr<IWICBitmapFrameDecode> bmpframedecoder;
	ComPtr<IWICFormatConverter> formatconverter;
	ComPtr<IWICBitmapScaler> bmpscaler;
	ComPtr<IWICBitmapDecoder>bmpdecoder;
	ComPtr<IDWriteTextFormat> textformat;
	ComPtr<ID2D1DrawingStateBlock> targetstate;
};
#endif