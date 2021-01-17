#include "stdafx.h"
bool D2DHandle::InitializeD2D(HWND hwnd,GameSettings& settings){
	HRESULT hr;
	hr=D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,d2dfactory.GetAddressOf());
	if(hr!=S_OK){
		MessageBox(NULL,L"Error al crear D2D1Factory",L"Error",MB_OK);
		return false;
	}
	hr=DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),(IUnknown**)dwritefactory.GetAddressOf());
	if(hr!=S_OK){
		MessageBox(NULL,L"Error al crear DWriteFactory",L"Error",MB_OK);
	}
	D2D1_SIZE_U size=SizeU(settings.winwidth,settings.winheight);
	D2D1_RENDER_TARGET_PROPERTIES rtp;
	rtp.dpiX=0.0f;
	rtp.dpiY=0.0f;
	unsigned long long RAM;
	GetPhysicallyInstalledSystemMemory(&RAM);
	float GBRAM=(float)(RAM/1048576.f);
	MessageBox(NULL,std::to_wstring(GBRAM).c_str(),L"RAM",MB_OK);
	rtp.minLevel=(GBRAM>=4.f)?D2D1_FEATURE_LEVEL_10:D2D1_FEATURE_LEVEL_9;
	rtp.pixelFormat=PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED);
	rtp.type=D2D1_RENDER_TARGET_TYPE_HARDWARE;
	rtp.usage=D2D1_RENDER_TARGET_USAGE_NONE;
	hr=d2dfactory->CreateHwndRenderTarget(rtp,HwndRenderTargetProperties(hwnd,size),target.GetAddressOf());
	if(hr!=S_OK){
		MessageBox(NULL,L"Error al crear HwndRenderTarget",L"Error",MB_OK);
		return false;
	}
	CoCreateInstance(CLSID_WICImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_IWICImagingFactory,(LPVOID*)imgfactory.GetAddressOf());
	dwritefactory->CreateTextFormat(L"Microsoft Sans Serif",NULL,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,12.0f,L"en-us",textformat.GetAddressOf());
	d2dfactory->CreateDrawingStateBlock(targetstate.GetAddressOf());
	return true;
}