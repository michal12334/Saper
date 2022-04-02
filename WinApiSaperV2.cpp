// WinApiSaperV2.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "WinApiSaperV2.h"
#include <utility>
#include <time.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcField(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    BoardSize(HWND, UINT, WPARAM, LPARAM);

HWND mainWindow;

bool isDebugOn = false;
bool isEnd = false;

struct Field {
    HWND hWnd;
    bool isCovered = true;
    bool isFlagged = false;
    int status = 0; // -1 - bomb
    int x;
    int y;

    void setPosition(int x, int y) {
        this->x = x;
        this->y = y;
        RECT rc;
        GetWindowRect(hWnd, &rc);
        MoveWindow(hWnd, x, y, rc.right - rc.left, rc.bottom - rc.top, TRUE);
    }

    void drawText(LPCWSTR text, COLORREF color) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        HDC hdc = GetDC(hWnd);
        HFONT font = CreateFont(
            SIZE,
            0,
            0,
            0,
            FW_BOLD,
            false,
            FALSE,
            0,
            EASTEUROPE_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            _T("Arial")
        );
        HFONT oldFont = (HFONT) SelectObject(hdc, font);
        SetBkColor(hdc, RGB(0xC8, 0xC8, 0xC8));
        SetTextColor(hdc, color);
        DrawText(hdc, text, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }

    void drawTextDebug(LPCWSTR text, COLORREF color) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        HDC hdc = GetDC(hWnd);
        HFONT font = CreateFont(
            SIZE,
            0,
            0,
            0,
            FW_BOLD,
            false,
            FALSE,
            0,
            EASTEUROPE_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            _T("Arial")
        );
        HFONT oldFont = (HFONT) SelectObject(hdc, font);
        SetBkColor(hdc, RGB(0x69, 0x69, 0x69));
        SetTextColor(hdc, color);
        DrawText(hdc, text, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }

    void draw() {
        RECT rc;
        rc.left = 0;
        rc.right = SIZE;
        rc.top = 0;
        rc.bottom = SIZE;
        if(isCovered) {
            FillRect(GetDC(hWnd), &rc, (HBRUSH) (COLOR_GRAYTEXT + COLOR_WINDOW));
            if(isFlagged) {
                HDC hdc = GetDC(hWnd);
                HBITMAP bitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP_FLAG));
                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBBitmap = (HBITMAP) SelectObject(memDC, bitmap);
                //BitBlt(hdc, 0, 0, 25, 25, memDC, 0, 0, SRCCOPY);
                StretchBlt(hdc, 0, 0, SIZE, SIZE, memDC, 0, 0, 48, 48, SRCCOPY);
                SelectObject(memDC, oldBBitmap);
                DeleteObject(bitmap);
                DeleteDC(memDC);
            } else if(isDebugOn) {
                switch(status) {
                case -1: {
                    HDC hdc = GetDC(hWnd);
                    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                    HPEN oldPen = (HPEN) SelectObject(hdc, pen);
                    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
                    HBRUSH oldBrush = (HBRUSH) SelectObject(hdc, brush);
                    Ellipse(hdc, 3, 3, SIZE - 3, SIZE - 3);
                    SelectObject(hdc, oldPen);
                    DeleteObject(pen);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(brush);
                    break;
                }

                case 1: {
                    drawTextDebug(L"1", RGB(0, 0, 255));
                    break;
                }

                case 2: {
                    drawTextDebug(L"2", RGB(0, 255, 0));
                    break;
                }

                case 3: {
                    drawTextDebug(L"3", RGB(255, 0, 0));
                    break;
                }

                case 4: {
                    drawTextDebug(L"4", RGB(0, 0, 0xAA));
                    break;
                }

                case 5: {
                    drawTextDebug(L"5", RGB(0xAA, 0, 0));
                    break;
                }

                case 6: {
                    drawTextDebug(L"6", RGB(0, 0xAA, 0));
                    break;
                }

                case 7: {
                    drawTextDebug(L"7", RGB(0, 0, 0x55));
                    break;
                }

                case 8: {
                    drawTextDebug(L"8", RGB(0, 0x55, 0));
                    break;
                }
                }
            }
        } else {
            FillRect(GetDC(hWnd), &rc, CreateSolidBrush(RGB(0xC8, 0xC8, 0xC8)));
            switch(status) {
                case -1: {
                    HDC hdc = GetDC(hWnd);
                    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
                    HPEN oldPen = (HPEN) SelectObject(hdc, pen);
                    HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
                    HBRUSH oldBrush = (HBRUSH) SelectObject(hdc, brush);
                    Ellipse(hdc, 3, 3, SIZE - 3, SIZE - 3);
                    SelectObject(hdc, oldPen);
                    DeleteObject(pen);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(brush);
                    break;
                }

                case 1: {
                    drawText(L"1", RGB(0, 0, 255));
                    break;
                }

                case 2: {
                    drawText(L"2", RGB(0, 255, 0));
                    break;
                }

                case 3: {
                    drawText(L"3", RGB(255, 0, 0));
                    break;
                }

                case 4: {
                    drawText(L"4", RGB(0, 0, 0xAA));
                    break;
                }

                case 5: {
                    drawText(L"5", RGB(0xAA, 0, 0));
                    break;
                }

                case 6: {
                    drawText(L"6", RGB(0, 0xAA, 0));
                    break;
                }

                case 7: {
                    drawText(L"7", RGB(0, 0, 0x55));
                    break;
                }

                case 8: {
                    drawText(L"8", RGB(0, 0x55, 0));
                    break;
                }
            }
        }
    }

    static const int SIZE;
};

const int Field::SIZE = 25;

Field **fields = nullptr;
int verticalSize = 10;
int horizontalSize = 10;
int numberOfBombs = 10;

void destroyFields() {
    if(fields == nullptr)
        return;
    for(int i = 0; i < verticalSize; i++) {
        for(int j = 0; j < horizontalSize; j++) {
            DestroyWindow(fields[i][j].hWnd);
        }
    }
    for(int i = 0; i < verticalSize; i++)
        delete [] fields[i];
    delete [] fields;
    fields = nullptr;
}

ATOM RegisterFieldClass(WCHAR className[]) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProcField;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInst;
    wcex.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_WINAPISAPERV2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(0xC8, 0xC8, 0xC8));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPISAPERV2);
    wcex.lpszClassName  = className;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HWND createFieldWindow() {
    WCHAR className[50];
    WCHAR title[50];
    lstrcpyW(className, L"field");
    lstrcpyW(title, L"field");
    RegisterFieldClass(className);
    return CreateWindow(className, title, WS_VISIBLE | WS_CHILD,
        0, 0, Field::SIZE, Field::SIZE, mainWindow, nullptr, hInst, nullptr);
}

void generateBombs() {
    int m = verticalSize * horizontalSize;
    int nb = numberOfBombs;
    while(nb--) {
        int a, k, l;
        do {
            a = rand() % m;
            k = a / verticalSize;
            l = a % verticalSize;
        } while(fields[l][k].status != 0);
        fields[l][k].status = -1;
    }
    for(int i = 0; i < verticalSize; i++) {
        for(int j = 0; j < horizontalSize; j++) {
            if(fields[i][j].status == -1)
                continue;
            int a = 0;
            if(i != 0 && fields[i - 1][j].status == -1)
                a++;
            if(j != 0 && fields[i][j - 1].status == -1)
                a++;
            if(i != verticalSize - 1 && fields[i + 1][j].status == -1)
                a++;
            if(j != horizontalSize - 1 && fields[i][j + 1].status == -1)
                a++;
            if(i != 0 && j != 0 && fields[i - 1][j - 1].status == -1)
                a++;
            if(i != 0 && j != horizontalSize - 1 && fields[i - 1][j + 1].status == -1)
                a++;
            if(i != verticalSize - 1 && j != 0 && fields[i + 1][j - 1].status == -1)
                a++;
            if(i != verticalSize - 1 && j != horizontalSize - 1 && fields[i + 1][j + 1].status == -1)
                a++;
            fields[i][j].status = a;
        }
    }
}

void createFields() {
    destroyFields();

    //verticalSize = rand() % 15 + 10;
    //horizontalSize = rand() % 21 + 10;

    std::pair<int, int> desktopSize = std::make_pair(GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));

    std::pair<int, int> windowSize;
    windowSize.first = horizontalSize * (Field::SIZE + 1) - 1 + 16;
    windowSize.second = verticalSize * (Field::SIZE + 1) - 1 + 89;

    std::pair<int, int> windowPosition;
    windowPosition.first = (desktopSize.first - windowSize.first) / 2;
    windowPosition.second = (desktopSize.second - windowSize.second) / 2;

    MoveWindow(mainWindow, windowPosition.first, windowPosition.second, windowSize.first, windowSize.second, TRUE);

    fields = new Field*[verticalSize];
    for(int i = 0; i < verticalSize; i++)
        fields[i] = new Field[horizontalSize];
    for(int i = 0; i < verticalSize; i++) {
        for(int j = 0; j < horizontalSize; j++) {
            fields[i][j].hWnd = createFieldWindow();
            fields[i][j].setPosition((Field::SIZE + 1) * j, 30 + (Field::SIZE + 1) * i);
        }
    }

    generateBombs();
}

void unCover(int i, int j) {
    if(!fields[i][j].isCovered)
        return;
    fields[i][j].isCovered = false;
    fields[i][j].draw();
    if(fields[i][j].status != 0)
        return;
    if(i != 0 && !fields[i - 1][j].isFlagged)
        unCover(i - 1, j);
    if(i != verticalSize - 1 && !fields[i + 1][j].isFlagged)
        unCover(i + 1, j);
    if(j != 0 && !fields[i][j - 1].isFlagged)
        unCover(i, j - 1);
    if(j != horizontalSize - 1 && !fields[i][j + 1].isFlagged)
        unCover(i, j + 1);
    if(i != 0 && j != 0 && !fields[i - 1][j - 1].isFlagged)
        unCover(i - 1, j - 1);
    if(i != 0 && j != horizontalSize - 1 && !fields[i - 1][j + 1].isFlagged)
        unCover(i - 1, j + 1);
    if(i != verticalSize - 1 && j != 0 && !fields[i + 1][j - 1].isFlagged)
        unCover(i + 1, j - 1);
    if(i != verticalSize - 1 && j != horizontalSize - 1 && !fields[i + 1][j + 1].isFlagged)
        unCover(i + 1, j + 1);
}

struct Time {
    static float getDeltaTime() {
        return deltaTime;
    }

    static ULONGLONG previousTime;
    static float deltaTime;
};

ULONGLONG Time::previousTime = GetTickCount64();
float Time::deltaTime = 0;

struct Timer {
    float time = 0;
    bool isRunning = false;

    void draw() {
        RECT rc;
        GetClientRect(mainWindow, &rc);
        rc.bottom = 30;
        rc.right = rc.right / 2;
        const int BUF_SIZE = 7;
        TCHAR buf[BUF_SIZE];
        _stprintf_s(buf, BUF_SIZE, _T("%00006.1f"), time);
        HDC hdc = GetDC(mainWindow);
        HFONT font = CreateFont(
            25,
            0,
            0,
            0,
            FW_BOLD,
            false,
            FALSE,
            0,
            EASTEUROPE_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            _T("Arial")
        );
        HFONT oldFont = (HFONT) SelectObject(hdc, font);
        SetBkColor(hdc, RGB(0xFF, 0xFF, 0xFF));
        SetTextColor(hdc, RGB(255, 0, 0));
        DrawText(hdc, buf, BUF_SIZE - 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }

    void update() {
        if(isRunning) {
            time += Time::getDeltaTime();
        }
        draw();
    }

    void run() {
        isRunning = true;
    }

    void reset() {
        time = 0;
        isRunning = false;
    }
};

Timer timer;

struct FlagCounter {
    static int getNumberOfFlags() {
        int result = 0;
        for(int i = 0; i < verticalSize; i++)
            for(int j = 0; j < horizontalSize; j++)
                if(fields[i][j].isFlagged)
                    result++;
        return result;
    }

    static void draw() {
        RECT rc;
        GetClientRect(mainWindow, &rc);
        rc.bottom = 30;
        rc.left = rc.right / 2;
        const int BUF_SIZE = 6;
        TCHAR buf[BUF_SIZE];
        _stprintf_s(buf, BUF_SIZE, _T("%04d"), numberOfBombs - getNumberOfFlags());
        HDC hdc = GetDC(mainWindow);
        HFONT font = CreateFont(
            25,
            0,
            0,
            0,
            FW_BOLD,
            false,
            FALSE,
            0,
            EASTEUROPE_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_SWISS,
            _T("Arial")
        );
        HFONT oldFont = (HFONT) SelectObject(hdc, font);
        SetBkColor(hdc, RGB(0xFF, 0xFF, 0xFF));
        SetTextColor(hdc, RGB(255, 0, 0));
        DrawText(hdc, buf, BUF_SIZE - 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }
};

void drawFields() {
    if(fields == nullptr)
        return;
    for(int i = 0; i < verticalSize; i++)
        for(int j = 0; j < horizontalSize; j++)
            fields[i][j].draw();
    FlagCounter::draw();
}

bool isWinByUncoveringEverythingExceptForBombs() {
    for(int i = 0; i < verticalSize; i++)
        for(int j = 0; j < horizontalSize; j++)
            if(fields[i][j].isCovered && fields[i][j].status != -1)
                return false;
    return true;
}

bool isWinByFlaggingEveryBomb() {
    if(FlagCounter::getNumberOfFlags() != numberOfBombs)
        return false;
    for(int i = 0; i < verticalSize; i++)
        for(int j = 0; j < horizontalSize; j++)
            if(fields[i][j].status == -1 && !fields[i][j].isFlagged)
                return false;
    return true;
}

bool isWin() {
    return isWinByUncoveringEverythingExceptForBombs() || isWinByFlaggingEveryBomb();
}

void createNewGame(int newVSize, int newHSize, int newNumberOfBombs) {
    destroyFields();
    verticalSize = newVSize;
    horizontalSize = newHSize;
    numberOfBombs = newNumberOfBombs;
    timer.reset();
    isEnd = false;
    createFields();
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    srand(time(NULL));

    // Initialize global strings
    //LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    lstrcpyW(szTitle, L"WinAPI Minesweeper");
    LoadStringW(hInstance, IDC_WINAPISAPERV2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    createFields();

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINAPISAPERV2));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    destroyFields();

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINAPISAPERV2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINAPISAPERV2);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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

   mainWindow = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!mainWindow)
   {
      return FALSE;
   }

   ShowWindow(mainWindow, nCmdShow);
   UpdateWindow(mainWindow);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 80, NULL);
        break;

    case WM_TIMER: {
        auto currentTime = GetTickCount64();
        Time::deltaTime = (currentTime - Time::previousTime) / 1000.0f;
        Time::previousTime = currentTime;
        if(isEnd)
            break;
        timer.update();
        //draw();
        break;
    }

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_NEW:
            case ID_GAME_NEW:
                createFields();
                timer.reset();
                isEnd = false;
                break;

            case IDM_DEBUG:
            case ID_HELP_DEBUG: {
                isDebugOn = !isDebugOn;
                if(isDebugOn)
                    CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUG, MF_CHECKED);
                else
                    CheckMenuItem(GetMenu(hWnd), ID_HELP_DEBUG, MF_UNCHECKED);
                drawFields();
                break;
            }

            case ID_GAME_CUSTOMSIZE:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_BOARD_SIZE), hWnd, BoardSize);
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
            drawFields();
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

LRESULT CALLBACK WndProcField(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_LBUTTONDOWN:
        if(isEnd)
            break;
        timer.run();
        for(int i = 0; i < verticalSize; i++)
            for(int j = 0; j < horizontalSize; j++)
                if(fields[i][j].hWnd == hWnd) {
                    if(fields[i][j].isFlagged)
                        break;
                    unCover(i, j);
                    if(fields[i][j].status == -1) {
                        isEnd = true;
                        MessageBox(mainWindow, L"Boom!", L"Mine", MB_OK | MB_ICONSTOP);
                    } else if(isWin()) {
                        isEnd = true;
                        MessageBox(mainWindow, L"Win!", L"Minesweeper", MB_OK);
                    }
                }
        break;

    case WM_RBUTTONDOWN:
        if(isEnd)
            break;
        timer.run();
        for(int i = 0; i < verticalSize; i++)
            for(int j = 0; j < horizontalSize; j++)
                if(fields[i][j].hWnd == hWnd) {
                    if(!fields[i][j].isCovered)
                        break;
                    fields[i][j].isFlagged = !fields[i][j].isFlagged;
                    fields[i][j].draw();
                    if(isWin()) {
                        isEnd = true;
                        MessageBox(mainWindow, L"Win!", L"Minesweeper", MB_OK);
                    }
                }
        FlagCounter::draw();
        break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
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
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
        for(int i = 0; i < verticalSize; i++)
            for(int j = 0; j < horizontalSize; j++)
                if(fields[i][j].hWnd == hWnd)
                    fields[i][j].draw();
    }
    break;
    /*case WM_DESTROY:
        PostQuitMessage(0);
        break;*/
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

bool isHorizontalSizeValid(int hSize) {
    return hSize >= 10 && hSize <= 24;
}

bool isVerticalSizeValid(int vSize) {
    return vSize >= 10 && vSize <= 30;
}

bool isNumberOfBombsValid(int nb, int vSize, int hSize) {
    return nb >= 10 && nb < vSize * hSize;
}

INT_PTR CALLBACK BoardSize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hDlg, IDC_EDIT_WIDTH, horizontalSize, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, verticalSize, TRUE);
        SetDlgItemInt(hDlg, IDC_EDIT_MINES, numberOfBombs, TRUE);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK) {
            int newHorizontalSize = GetDlgItemInt(hDlg, IDC_EDIT_WIDTH, NULL, TRUE);
            int newVerticalSize = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, NULL, TRUE);
            int newNumberOfBombs = GetDlgItemInt(hDlg, IDC_EDIT_MINES, NULL, TRUE);

            if(!isVerticalSizeValid(newVerticalSize))
                newVerticalSize = verticalSize;

            if(!isHorizontalSizeValid(newHorizontalSize))
                newHorizontalSize = horizontalSize;

            if(!isNumberOfBombsValid(newNumberOfBombs, newVerticalSize, newHorizontalSize))
                newNumberOfBombs = numberOfBombs;
            if(!isNumberOfBombsValid(newNumberOfBombs, newVerticalSize, newHorizontalSize))
                newNumberOfBombs = 10;

            createNewGame(newVerticalSize, newHorizontalSize, newNumberOfBombs);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR) TRUE;
        } else if(LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR) TRUE;
        }
        break;
    }
    return (INT_PTR) FALSE;
}
