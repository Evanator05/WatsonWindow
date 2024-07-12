#pragma once

#include "windows.h"

typedef unsigned char uint8_t;

struct Color {
    uint8_t b;
    uint8_t g;
    uint8_t r;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
    public:
        int width;
        int height;

        Window(LPCSTR windowName, int width, int height);
        Window(const Window&) = delete;
        Window& operator = (const Window&) = delete;
        ~Window();

        bool ProcessMessages();

        void SetPixel(int x, int y, Color color);
        void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
        
        void ClearWindow(Color color);
        void ClearWindow(uint8_t r, uint8_t g, uint8_t b);

        void UpdateWindow();

        void DrawRect(int x, int y, int w, int h, Color color);
        void DrawRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);

        bool isKeyPressed(int key);
        bool isKeyPressed(char key);
        
    private:
        HDC hdc; 
        HINSTANCE m_hInstance;
        HWND m_hWnd;
        bool pressed[256];

        uint8_t* buffer;
        HDC memDC;
        HBITMAP bitmap;
        BITMAPINFO bmi;
};