// Win32AVI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32AVI.h"
#include <Vfw.h>
#include <Commdlg.h>

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


PAVIFILE aviFile = NULL;
PAVISTREAM aviStream = NULL;
PGETFRAME aviFrame = NULL;
BITMAPINFOHEADER * bmpInfoHeader = NULL;

BOOL InitAVIResource(PCTSTR fileName);
void RemoveAVIResource();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // AVI File Initialize
    AVIFileInit();

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32AVI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32AVI));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // AVI File Exit
    AVIFileExit();

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32AVI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32AVI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case ID_FILE_OPEN:
            {
                OPENFILENAME ofn;       // common dialog box structure
                TCHAR szFile[260];       // buffer for file name

                // Initialize OPENFILENAME
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFile = szFile;
                // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
                // use the contents of szFile to initialize itself.
                ofn.lpstrFile[0] = _T('\0');
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = _T("AVI\0*.*\0");
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                
                if (GetOpenFileName(&ofn) == TRUE) {
                    RemoveAVIResource();
                    InitAVIResource(szFile);
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            if (bmpInfoHeader != NULL) {
                SetDIBitsToDevice(
                    hdc, 0, 0, bmpInfoHeader->biWidth,
                    bmpInfoHeader->biHeight,
                    0, 0, 0, bmpInfoHeader->biHeight, bmpInfoHeader + 1,
                    (BITMAPINFO *)bmpInfoHeader, DIB_RGB_COLORS
                    );
            }
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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

//AVI ファイルを開いて必要な初期化処理を行う
BOOL InitAVIResource(PCTSTR fileName) {
    //AVIFile を開く
    if (AVIFileOpen(&aviFile, fileName, OF_READ, NULL)) {
        return FALSE;
    }
    //AVI ストリームを取得する
    if (AVIFileGetStream(aviFile, &aviStream, streamtypeVIDEO, 0)) {
        AVIFileRelease(aviFile);
        aviFile = NULL;
        return FALSE;
    }
    //フレームを解凍するためのオブジェクトを取得
    aviFrame = AVIStreamGetFrameOpen(aviStream, NULL);
    if (aviFrame == NULL) {
        AVIStreamRelease(aviStream);
        AVIFileRelease(aviFile);
        aviStream = NULL;
        aviFile = NULL;
        return FALSE;
    }

    bmpInfoHeader = (BITMAPINFOHEADER *)AVIStreamGetFrame(aviFrame, 10);
    return TRUE;
}

//AVI ファイルに関連するすべてのリソースを解放する
void RemoveAVIResource() {
    //カレントフレームを NULL に設定して描画されないようにする
    bmpInfoHeader = NULL;

    //各リソースを解放
    if (aviFrame) AVIStreamGetFrameClose(aviFrame);
    if (aviStream) AVIStreamRelease(aviStream);
    if (aviFile) AVIFileRelease(aviFile);

    aviFrame = NULL;
    aviStream = NULL;
    aviFile = NULL;
}
