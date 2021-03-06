// MDIDemo.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "MDIDemo.h"

#define MAX_LOADSTRING 100

#define INIT_MENU_POS 0
#define HELLO_MENU_POS 2
#define RECT_MENU_POS 1
#define IDM_FIRSTCHILD 50000
LRESULT CALLBACK FrameWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CloseEnumProc(HWND, LPARAM);
LRESULT CALLBACK HelloWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK RectWndProc(HWND, UINT, WPARAM, LPARAM);
// structure for storing data unique to each Hello child window
typedef struct tagHELLODATA
{
	UINT iColor;
	COLORREF clrText;
}
HELLODATA, *PHELLODATA;
// structure for storing data unique to each Rect child window 
typedef struct tagRECTDATA
{
	short cxClient;
	short cyClient;
}
RECTDATA, *PRECTDATA;
// global variables
TCHAR szAppName[] = TEXT("MDIDemo");
TCHAR szFrameClass[] = TEXT("MdiFrame");
TCHAR szHelloClass[] = TEXT("MdiHelloChild");
TCHAR szRectClass[] = TEXT("MdiRectChild");
HMENU hMenuInit, hMenuHello, hMenuRect;
HMENU hMenuInitWindow, hMenuHelloWindow, hMenuRectWindow;


// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	hInst = hInstance;

	// グローバル文字列を初期化する
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MDIDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	// Obtain handles to three possible menus & submenus
	hMenuInit = LoadMenu(hInstance, TEXT("MdiMenuInit"));
	hMenuHello = LoadMenu(hInstance, TEXT("MdiMenuHello"));
	hMenuRect = LoadMenu(hInstance, TEXT("MdiMenuRect"));

	hMenuInitWindow = GetSubMenu(hMenuInit, INIT_MENU_POS);
	hMenuHelloWindow = GetSubMenu(hMenuHello, HELLO_MENU_POS);
	hMenuRectWindow = GetSubMenu(hMenuRect, RECT_MENU_POS);

	HWND hwndFrame = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, hMenuInit, hInstance, nullptr);

	if (!hwndFrame)
	{
		return FALSE;
	}

	HWND hwndClient = GetWindow(hwndFrame, GW_CHILD);

	ShowWindow(hwndFrame, nCmdShow);
	UpdateWindow(hwndFrame);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MDIDEMO));

	MSG msg;

	// メイン メッセージ ループ:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateMDISysAccel(hwndClient, &msg) && !TranslateAccelerator(hwndFrame, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// Clean up by deleting unattached menus 
	DestroyMenu(hMenuHello);
	DestroyMenu(hMenuRect);

	return (int)msg.wParam;
}

//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	// Register the frame window class 
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = FrameWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MDIDEMO));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassExW(&wcex);

	// Register the Hello child window class 
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = HelloWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(HANDLE);
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szHelloClass;
	RegisterClassExW(&wcex);

	// Register the Rect child window class
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = RectWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(HANDLE);
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szRectClass;

	return RegisterClassExW(&wcex);
}

LRESULT CALLBACK FrameWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
	lParam)
{
	static HWND hwndClient;
	CLIENTCREATESTRUCT clientcreate;
	HWND hwndChild;
	MDICREATESTRUCT mdicreate;

	switch (message)
	{
	case WM_CREATE: // Create the client window

		clientcreate.hWindowMenu = hMenuInitWindow;
		clientcreate.idFirstChild = IDM_FIRSTCHILD;

		hwndClient = CreateWindow(TEXT("MDICLIENT"), NULL,
			WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,
			0, 0, 0, 0, hwnd, (HMENU)1, hInst,
			(PSTR)&clientcreate);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_FILE_NEWHELLO: // Create a Hello child window
			mdicreate.szClass = szHelloClass;
			mdicreate.szTitle = TEXT("Hello");
			mdicreate.hOwner = hInst;
			mdicreate.x = CW_USEDEFAULT;
			mdicreate.y = CW_USEDEFAULT;
			mdicreate.cx = CW_USEDEFAULT;
			mdicreate.cy = CW_USEDEFAULT;
			mdicreate.style = 0;
			mdicreate.lParam = 0;
			hwndChild = (HWND)SendMessage(hwndClient,
				WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;

		case IDM_FILE_NEWRECT: // Create a Rect child window

			mdicreate.szClass = szRectClass;
			mdicreate.szTitle = TEXT("Rectangles");
			mdicreate.hOwner = hInst;
			mdicreate.x = CW_USEDEFAULT;
			mdicreate.y = CW_USEDEFAULT;
			mdicreate.cx = CW_USEDEFAULT;
			mdicreate.cy = CW_USEDEFAULT;
			mdicreate.style = 0;
			mdicreate.lParam = 0;

			hwndChild = (HWND)SendMessage(hwndClient,
				WM_MDICREATE, 0,
				(LPARAM)(LPMDICREATESTRUCT)&mdicreate);
			return 0;

		case IDM_FILE_CLOSE: // Close the active window

			hwndChild = (HWND)SendMessage(hwndClient,
				WM_MDIGETACTIVE, 0, 0);

			if (SendMessage(hwndChild, WM_QUERYENDSESSION, 0, 0))
				SendMessage(hwndClient, WM_MDIDESTROY,
				(WPARAM)hwndChild, 0);
			return 0;
		case IDM_APP_EXIT: // Exit the program

			SendMessage(hwnd, WM_CLOSE, 0, 0);
			return 0;

			// messages for arranging windows
		case IDM_WINDOW_TILE:
			SendMessage(hwndClient, WM_MDITILE, 0, 0);
			return 0;

		case IDM_WINDOW_CASCADE:
			SendMessage(hwndClient, WM_MDICASCADE, 0, 0);
			return 0;

		case IDM_WINDOW_ARRANGE:
			SendMessage(hwndClient, WM_MDIICONARRANGE, 0, 0);
			return 0;

		case IDM_WINDOW_CLOSEALL: // Attempt to close all children
			EnumChildWindows(hwndClient, CloseEnumProc, 0);
			return 0;

		default: // Pass to active child...
			hwndChild = (HWND)SendMessage(hwndClient,
				WM_MDIGETACTIVE, 0, 0);
			if (IsWindow(hwndChild))
				SendMessage(hwndChild, WM_COMMAND, wParam, lParam);

			break; // ...and then to DefFrameProc
		}
		break;

	case WM_QUERYENDSESSION:
	case WM_CLOSE: // Attempt to close all children

		SendMessage(hwnd, WM_COMMAND, IDM_WINDOW_CLOSEALL, 0);

		if (NULL != GetWindow(hwndClient, GW_CHILD))
			return 0;

		break; // i.e., call DefFrameProc
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	// Pass unprocessed messages to DefFrameProc (not DefWindowProc)
	return DefFrameProc(hwnd, hwndClient, message, wParam, lParam);
}
BOOL CALLBACK CloseEnumProc(HWND hwnd, LPARAM lParam)
{
	if (GetWindow(hwnd, GW_OWNER)) // Check for icon title
		return TRUE;

	SendMessage(GetParent(hwnd), WM_MDIRESTORE, (WPARAM)hwnd, 0);
	if (!SendMessage(hwnd, WM_QUERYENDSESSION, 0, 0))
		return TRUE;
	SendMessage(GetParent(hwnd), WM_MDIDESTROY, (WPARAM)hwnd, 0);
	return TRUE;
}
LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	static COLORREF clrTextArray[] = { RGB(0, 0, 0), RGB(255, 0, 0),
	RGB(0, 255, 0), RGB(0, 0, 255),
	RGB(255, 255, 255) };
	static HWND hwndClient, hwndFrame;
	HDC hdc;
	HMENU hMenu;
	PHELLODATA pHelloData;
	PAINTSTRUCT ps;
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		// Allocate memory for window private data
		pHelloData = (PHELLODATA)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY, sizeof(HELLODATA));
		pHelloData->iColor = IDM_COLOR_BLACK;
		pHelloData->clrText = RGB(0, 0, 0);
		SetWindowLong(hwnd, 0, (long)pHelloData);

		// Save some window handles
		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_COLOR_BLACK:
		case IDM_COLOR_RED:
		case IDM_COLOR_GREEN:
		case IDM_COLOR_BLUE:
		case IDM_COLOR_WHITE:
			// Change the text color
			pHelloData = (PHELLODATA)GetWindowLong(hwnd, 0);

			hMenu = GetMenu(hwndFrame);

			CheckMenuItem(hMenu, pHelloData->iColor, MF_UNCHECKED);
			pHelloData->iColor = wParam;
			CheckMenuItem(hMenu, pHelloData->iColor, MF_CHECKED);

			pHelloData->clrText = clrTextArray[wParam - IDM_COLOR_BLACK];

			InvalidateRect(hwnd, NULL, FALSE);
		}
		return 0;

	case WM_PAINT:
		// Paint the window
		hdc = BeginPaint(hwnd, &ps);

		pHelloData = (PHELLODATA)GetWindowLong(hwnd, 0);
		SetTextColor(hdc, pHelloData->clrText);

		GetClientRect(hwnd, &rect);

		DrawText(hdc, TEXT("Hello, World!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE:
		// Set the Hello menu if gaining focus
		if (lParam == (LPARAM)hwnd)
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)
				hMenuHello, (LPARAM)hMenuHelloWindow);

		// Check or uncheck menu item
		pHelloData = (PHELLODATA)GetWindowLong(hwnd, 0);
		CheckMenuItem(hMenuHello, pHelloData->iColor,
			(lParam == (LPARAM)hwnd) ? MF_CHECKED :
			MF_UNCHECKED);

		// Set the Init menu if losing focus

		if (lParam != (LPARAM)hwnd)
			SendMessage(hwndClient,
				WM_MDISETMENU, (WPARAM)
				hMenuInit, (LPARAM)hMenuInitWindow);

		DrawMenuBar(hwndFrame);
		return 0;
	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (IDOK != MessageBox(hwnd, TEXT("OK to close window?"), TEXT("Hello"), MB_ICONQUESTION | MB_OKCANCEL))
			return 0;

		break; // i.e., call DefMDIChildProc

	case WM_DESTROY:
		pHelloData = (PHELLODATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pHelloData);
		return 0;
	}
	// Pass unprocessed message to DefMDIChildProc
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}
LRESULT CALLBACK RectWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM
	lParam)
{
	static HWND hwndClient, hwndFrame;
	HBRUSH hBrush;
	HDC hdc;
	PRECTDATA pRectData;
	PAINTSTRUCT ps;
	int xLeft, xRight, yTop, yBottom;
	short nRed, nGreen, nBlue;

	switch (message)
	{
	case WM_CREATE:
		// Allocate memory for window private data
		pRectData = (PRECTDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(RECTDATA));

		SetWindowLong(hwnd, 0, (long)pRectData);

		// Start the timer going
		SetTimer(hwnd, 1, 250, NULL);

		// Save some window handles
		hwndClient = GetParent(hwnd);
		hwndFrame = GetParent(hwndClient);
		return 0;
	case WM_SIZE: // If not minimized, save the window size

		if (wParam != SIZE_MINIMIZED)
		{
			pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);

			pRectData->cxClient = LOWORD(lParam);
			pRectData->cyClient = HIWORD(lParam);
		}

		break; // WM_SIZE must be processed by DefMDIChildProc

	case WM_TIMER: // Display a random rectangle

		pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);
		xLeft = rand() % pRectData->cxClient;
		xRight = rand() % pRectData->cxClient;
		yTop = rand() % pRectData->cyClient;
		yBottom = rand() % pRectData->cyClient;
		nRed = rand() & 255;
		nGreen = rand() & 255;
		nBlue = rand() & 255;

		hdc = GetDC(hwnd);
		hBrush = CreateSolidBrush(RGB(nRed, nGreen, nBlue));
		SelectObject(hdc, hBrush);

		Rectangle(hdc, min(xLeft, xRight), min(yTop, yBottom),
			max(xLeft, xRight), max(yTop, yBottom));

		ReleaseDC(hwnd, hdc);
		DeleteObject(hBrush);
		return 0;

	case WM_PAINT: // Clear the window

		InvalidateRect(hwnd, NULL, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_MDIACTIVATE: // Set the appropriate menu
		if (lParam == (LPARAM)hwnd)
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuRect,
			(LPARAM)hMenuRectWindow);
		else
			SendMessage(hwndClient, WM_MDISETMENU, (WPARAM)hMenuInit,
			(LPARAM)hMenuInitWindow);
		DrawMenuBar(hwndFrame);
		return 0;

	case WM_DESTROY:
		pRectData = (PRECTDATA)GetWindowLong(hwnd, 0);
		HeapFree(GetProcessHeap(), 0, pRectData);
		KillTimer(hwnd, 1);
		return 0;
	}
	// Pass unprocessed message to DefMDIChildProc
	return DefMDIChildProc(hwnd, message, wParam, lParam);
}





