// Maquina_Turing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Maquina_Turing.h"

#define MAX_LOADSTRING 100

// Global Variables:
HWND hwnd;
HWND inputValues;
HWND acceptedLanguage;
HWND renderWindow;
Scene mainScene;
Parser parser;
int timesParsed=0;
bool tapemoving=false;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
GameSettings settings;
std::thread parserthread;
std::shared_ptr<D2DHandle> d2d;
bool needsRecreation=true;
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg;
	msg.message=WM_NULL;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAQUINA_TURING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	d2d=std::make_shared<D2DHandle>();
	settings.fullscreen=false;
	settings.winwidth=800;
	settings.winheight=600;
	settings.screenwidth=GetSystemMetrics(SM_CXSCREEN);
	settings.screenheight=GetSystemMetrics(SM_CYSCREEN);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	CoInitializeEx(NULL,COINITBASE_MULTITHREADED);
	if(!d2d->InitializeD2D(hwnd,settings)){
		MessageBox(hwnd,L"Error inicializando Direct2D",L"Error",MB_OK|MB_ICONERROR);
		return -1;
	}
	// Main message loop:
	while (msg.message!=WM_QUIT)
	{
		if (PeekMessage(&msg,0,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(IsWindowVisible(hwnd)){
			if(needsRecreation){
				mainScene.recreateResources(d2d,settings);
				needsRecreation=false;
			}
			mainScene.btnStart->setEnabled(!tapemoving);
			mainScene.tfInput->setEnabled(!tapemoving);
			d2d->target->BeginDraw();
			d2d->target->Clear(ColorF(ColorF::White));
			D2DUISystem::getInstance().draw(d2d);
			d2d->target->SaveDrawingState(d2d->targetstate.Get());
			if(mainScene.selection->getBounds().right>800){
				d2d->target->SetTransform(Matrix3x2F::Translation(800-mainScene.selection->getBounds().right,0.f));
			}
			mainScene.selection->draw(d2d);
			if(mainScene.tape.size()!=0){
				for(int i=0;i<mainScene.tape.size();i++){
					if(mainScene.tape.at(i)->isVisible()){
						mainScene.tape.at(i)->draw(d2d);
						mainScene.tapeSquares.at(i)->draw(d2d);
					}
				}
			}
			mainScene.marker->draw(d2d);
			d2d->target->RestoreDrawingState(d2d->targetstate.Get());
			d2d->target->EndDraw();
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAQUINA_TURING));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= CreateSolidBrush(RGB(255,255,255));
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	RECT wr={0,0,settings.winwidth,settings.winheight};
	AdjustWindowRect(&wr,WS_OVERLAPPEDWINDOW,false);
	hwnd = CreateWindowEx(WS_EX_LAYERED,szWindowClass,szTitle, WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_SIZEBOX,
		0, 0, wr.right-wr.left, wr.bottom-wr.top, NULL, NULL, hInstance, NULL);
	SetLayeredWindowAttributes(hwnd,RGB(0,0,0),255,LWA_ALPHA);

	if (!hwnd)
	{
		return FALSE;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wchar_t newcharacter;
	int x,y;
	int wmId, wmEvent;
	int markerLeft=0;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_MOUSEMOVE:
		x=GET_X_LPARAM(lParam);
		y=GET_Y_LPARAM(lParam);
		if(mainScene.btnStart->isEnabled()){
			if((x>=mainScene.btnStart->getBounds().left&&x<=mainScene.btnStart->getBounds().right)&&(y>=mainScene.btnStart->getBounds().top&&y<=mainScene.btnStart->getBounds().bottom)){
				mainScene.btnStart->setHovered(true);
			}
			else{
				mainScene.btnStart->setHovered(false);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		x=GET_X_LPARAM(lParam);
		y=GET_Y_LPARAM(lParam);
		if(mainScene.btnStart->isEnabled()){
			if((x>=mainScene.btnStart->getBounds().left&&x<=mainScene.btnStart->getBounds().right)&&(y>=mainScene.btnStart->getBounds().top&&y<=mainScene.btnStart->getBounds().bottom)){
				mainScene.btnStart->setPressed(true);
			}
			else{
				mainScene.btnStart->setPressed(false);
			}
		}
		if(mainScene.tfInput->isEnabled()&&mainScene.tfInput->isEnabled()){
			if((x>=mainScene.tfInput->getBounds().left&&x<=mainScene.tfInput->getBounds().right)&&(y>=mainScene.tfInput->getBounds().top&&y<=mainScene.tfInput->getBounds().bottom)){
				mainScene.tfInput->setFocus(true);
				DWRITE_HIT_TEST_METRICS htmetrics;
				mainScene.tfInput->textlayout->HitTestPoint(x-mainScene.tfInput->getBounds().left,y-mainScene.tfInput->getBounds().top,&mainScene.tfInput->trail,&mainScene.tfInput->inside,&htmetrics);
				mainScene.tfInput->caretPos=htmetrics.textPosition;
				if(mainScene.tfInput->trail)mainScene.tfInput->caretPos++;
				mainScene.tfInput->trail=false;
				mainScene.tfInput->measureCaret(mainScene.tfInput->caretPos);
			}
			else{
				mainScene.tfInput->setFocus(false);
			}
		}
		break;
	case WM_LBUTTONUP:
		if(mainScene.btnStart->isEnabled()){
			if(mainScene.btnStart->isPressed()){
				mainScene.btnStart->setPressed(false);
				if(mainScene.tfInput->getText().length()!=0){
					timesParsed++;
					parserthread=std::thread([&](){
						parser.parseString(hWnd,mainScene.tfInput->getText());
					});
					parserthread.detach();
				}
				else{
					MessageBox(hWnd,L"La cadena a evaluar no puede estar vacía",L"Error",MB_OK|MB_ICONERROR);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		switch(wParam){
		case VK_LEFT:
			if(mainScene.tfInput->hasFocus()){
				std::wstring txt=mainScene.tfInput->getText();
				if(mainScene.tfInput->caretPos>0)mainScene.tfInput->caretPos--;
				mainScene.tfInput->measureCaret(mainScene.tfInput->caretPos);
			}
			break;
		case VK_RIGHT:
			if(mainScene.tfInput->hasFocus()){
				std::wstring txt=mainScene.tfInput->getText();
				if(mainScene.tfInput->caretPos<txt.length())mainScene.tfInput->caretPos++;
				mainScene.tfInput->measureCaret(mainScene.tfInput->caretPos);
			}
			break;
		case VK_DELETE:
			if(mainScene.tfInput->hasFocus()){
				std::wstring txt=mainScene.tfInput->getText();
				if(mainScene.tfInput->caretPos<txt.length()){
					txt.erase(mainScene.tfInput->caretPos,1);
				}
				mainScene.tfInput->setText(txt);
				mainScene.tfInput->measureCaret(mainScene.tfInput->caretPos);
			}
			break;
		}
		break;
	case WM_CHAR:
		if(mainScene.tfInput->hasFocus()){
			std::wstring txt=mainScene.tfInput->getText();
			switch(wParam){
			case 0x08://backspace
				if(mainScene.tfInput->caretPos>0){
					txt.erase(mainScene.tfInput->caretPos-1,1);
					mainScene.tfInput->caretPos--;
				}
				break;
			case 0x09://tab. Inserta 4 espacios
				break;
			case 0x0D://enter
				break;
			default://cualquier caracter
				newcharacter=(wchar_t)wParam;
				txt.insert(txt.begin()+mainScene.tfInput->caretPos,1,newcharacter);
				mainScene.tfInput->caretPos++;
			}
			mainScene.tfInput->setText(txt);
			mainScene.tfInput->measureCaret(mainScene.tfInput->caretPos);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CHANGESTATUS:
		mainScene.lbResultStatus->setText(std::wstring((wchar_t*)lParam));
		mainScene.lbResultStatus->recreateResources(d2d);
		break;
	case WM_MOVEMARKER:
		mainScene.cursorPos++;
		markerLeft=10+(50*mainScene.cursorPos)+15;
		mainScene.marker->setBounds(markerLeft,200,markerLeft+20,220);
		mainScene.selection->setBounds(10+(50*mainScene.cursorPos),220,10+(50*mainScene.cursorPos)+50,270);
		mainScene.lbMarkerPos->setText(std::to_wstring(mainScene.cursorPos));
		mainScene.lbMarkerPos->recreateResources(d2d);
		break;
	case WM_RETURNMARKER:
		mainScene.cursorPos--;
		markerLeft=10+(50*mainScene.cursorPos)+15;
		mainScene.marker->setBounds(markerLeft,200,markerLeft+20,220);
		mainScene.selection->setBounds(10+(50*mainScene.cursorPos),220,10+(50*mainScene.cursorPos)+50,270);
		mainScene.lbMarkerPos->setText(std::to_wstring(mainScene.cursorPos));
		mainScene.lbMarkerPos->recreateResources(d2d);
		break;
	case WM_STARTTAPE:
		mainScene.cursorPos=0;
		mainScene.lbMarkerPos->setText(std::to_wstring(mainScene.cursorPos));
		mainScene.lbMarkerPos->recreateResources(d2d);
		markerLeft=10+(50*mainScene.cursorPos)+15;
		mainScene.marker->setBounds(markerLeft,200,markerLeft+20,220);
		mainScene.selection->setBounds(10+(50*mainScene.cursorPos),220,10+(50*mainScene.cursorPos)+50,270);
		tapemoving=true;
		if(mainScene.tape.size()!=0){
			for(int i=0;i<mainScene.tape.size();i++){
				mainScene.tape.at(i)->setText(L"b");
				mainScene.tape.at(i)->setVisible(false);
				mainScene.tapeSquares.at(i)->setVisible(false);
			}
		}
		break;
	case WM_ENDTAPE:
		tapemoving=false;
		break;
	case WM_ADDLETTER:
		{
			if(mainScene.tape.size()==(int)wParam){
				std::shared_ptr<ImageView>square=std::shared_ptr<ImageView>(new ImageView(L"Images\\Machine\\square.png"));
				square->setOpacity();
				square->setBounds(10+(50*(int)wParam),220,10+(50*(int)wParam)+50,270);
				square->setVisible(true);
				mainScene.tapeSquares.push_back(square);
				std::shared_ptr<TextLabel>label=std::shared_ptr<TextLabel>(new TextLabel(std::wstring(1,parser.characters.at((int)wParam))));
				label->setFont(L"Microsoft Sans Serif");
				label->setForeground(0.f,0.f,0.f,1.f);
				label->setBounds(10+(50*(int)wParam),220,10+(50*(int)wParam)+50,270);
				label->setOpacity();
				label->setLocale(L"en-US");
				label->setPadding(0,0);
				label->setTextSize(12.f);
				label->setVisible(true);
				label->setHorizontalTextAlignment(HorizontalConstants::CENTER_HORIZONTAL);
				label->setVerticalTextAlignment(VerticalConstants::CENTER_VERTICAL);
				mainScene.tape.push_back(label);
			}
			else{
				mainScene.tape.at((int)wParam)->setText(std::wstring(1,parser.characters.at((int)wParam)));
				mainScene.tape.at((int)wParam)->setVisible(true);
				mainScene.tapeSquares.at((int)wParam)->setVisible(true);
			}
			mainScene.tape.at((int)wParam)->recreateResources(d2d);
			mainScene.tapeSquares.at((int)wParam)->recreateResources(d2d);
		}
		break;
	case WM_UPDATELETTER:
		mainScene.tape.at((int)wParam)->setText(std::wstring(1,parser.characters.at((int)wParam)));
		mainScene.tape.at((int)wParam)->recreateResources(d2d);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
