// Win32AVI.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Win32AVI.h"
#include <Vfw.h>
#include <Commdlg.h>

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
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

    // TODO: �ڴ˷��ô��롣

    // AVI File Initialize
    AVIFileInit();

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32AVI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32AVI));

    MSG msg;

    // ����Ϣѭ��: 
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
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
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
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
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

// �����ڡ������Ϣ�������
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

//AVI �ե�������_���Ʊ�Ҫ�ʳ��ڻ��I����Ф�
BOOL InitAVIResource(PCTSTR fileName) {
    //AVIFile ���_��
    if (AVIFileOpen(&aviFile, fileName, OF_READ, NULL)) {
        return FALSE;
    }
    //AVI ���ȥ�`���ȡ�ä���
    if (AVIFileGetStream(aviFile, &aviStream, streamtypeVIDEO, 0)) {
        AVIFileRelease(aviFile);
        aviFile = NULL;
        return FALSE;
    }
    //�ե�`��������뤿��Υ��֥������Ȥ�ȡ��
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

//AVI �ե�������v�B���뤹�٤ƤΥ꥽�`�����Ť���
void RemoveAVIResource() {
    //�����ȥե�`��� NULL ���O�������軭����ʤ��褦�ˤ���
    bmpInfoHeader = NULL;

    //���꥽�`������
    if (aviFrame) AVIStreamGetFrameClose(aviFrame);
    if (aviStream) AVIStreamRelease(aviStream);
    if (aviFile) AVIFileRelease(aviFile);

    aviFrame = NULL;
    aviStream = NULL;
    aviFile = NULL;
}
