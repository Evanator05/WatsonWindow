#include "window.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window(LPCSTR windowName, int width, int height) : m_hInstance(GetModuleHandle(nullptr)) {
    LPCSTR CLASS_NAME = "watsonWindow";

    WNDCLASS wndClass = {};
    wndClass.lpszClassName = CLASS_NAME;
    wndClass.hInstance = m_hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpfnWndProc = WindowProc;

    RegisterClass(&wndClass);
    DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    RECT rect;
    rect.left = 0;
    rect.top = 25;
    rect.left = rect.left + width;
    rect.bottom = rect.top + height;

    AdjustWindowRect(&rect, style, false);

    m_hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        windowName,
        style,
        rect.left,
        rect.top,
        width,
        height,
        NULL,
        NULL,
        m_hInstance,
        NULL
    );

    ShowWindow(m_hWnd, SW_SHOW);

    hdc = GetDC(m_hWnd);
    this->width = width;
    this->height = height;
    buffer = (uint8_t*)malloc(width * height * 3);

    memDC = CreateCompatibleDC(hdc);
    bitmap = CreateCompatibleBitmap(hdc, width, height);
    SelectObject(memDC, bitmap);

    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;
}

Window::~Window() {
    ReleaseDC(m_hWnd, hdc);
    DeleteDC(hdc);
    DeleteDC(memDC);
    DeleteObject(bitmap);
    delete buffer;

    LPCSTR CLASS_NAME = "watsonWindow";
    UnregisterClass(CLASS_NAME, m_hInstance);
}

bool Window::ProcessMessages() {
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) {
        switch (msg.message) {
            case WM_QUIT:
                return false;
                break;
            case WM_KEYDOWN:
                pressed[msg.wParam] = true;
                break;
            case WM_KEYUP:
                pressed[msg.wParam] = false;
                break;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

void Window::SetPixel(int x, int y, Color color) {
    int index = (y*width+x)*sizeof(color);
    memcpy(buffer+index, &color, sizeof(color));
}

void Window::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    SetPixel(x, y, { b, g, r });
}

void Window::ClearWindow(Color color) {
    for (int i = 0; i < width*height; i++) {
        SetPixel(i, 0, color);
    }
}

void Window::ClearWindow(uint8_t r, uint8_t g, uint8_t b) {
    ClearWindow({ b, g, r });
}

void Window::UpdateWindow() {
    SetDIBits(memDC, bitmap, 0, height, buffer, &bmi, DIB_RGB_COLORS);
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
}

void Window::DrawRect(int x, int y, int w, int h, Color color) {
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            SetPixel(x+i, y+j, color);
        }
    }
}

void Window::DrawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b) {
    DrawRect(x, y, w, h, { b, g, r });
}

bool Window::isKeyPressed(int key) {
    return pressed[key];
}

bool Window::isKeyPressed(char key) {
    return isKeyPressed((int)key);
}