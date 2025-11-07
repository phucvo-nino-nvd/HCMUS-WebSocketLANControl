#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <fstream>

using namespace std;

// Hàm lưu HBITMAP ra file BMP (thủ công)
bool SaveHBITMAPToFile(HBITMAP hBitmap, HDC hDC, const string& filename) {
    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = -bmp.bmHeight; // đảo ngược để ảnh đúng chiều
    bi.biPlanes = 1;
    bi.biBitCount = 24; // 24-bit RGB
    bi.biCompression = BI_RGB;
    bi.biSizeImage = ((bmp.bmWidth * 3 + 3) & ~3) * bmp.bmHeight;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    vector<BYTE> bmpData(bi.biSizeImage);
    GetDIBits(hDC, hBitmap, 0, bmp.bmHeight, bmpData.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    DWORD dwSizeofDIB = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = dwSizeofDIB;
    bmfHeader.bfType = 0x4D42; // 'BM'

    ofstream out(filename, ios::out | ios::binary);
    if (!out) return false;
    out.write((char*)&bmfHeader, sizeof(BITMAPFILEHEADER));
    out.write((char*)&bi, sizeof(BITMAPINFOHEADER));
    out.write((char*)bmpData.data(), bmpData.size());
    out.close();
    return true;
}

// 📸 Chụp màn hình và lưu file BMP
string screen_cap() {
    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
    HGDIOBJ old = SelectObject(hMemDC, hBitmap);

    BitBlt(hMemDC, 0, 0, w, h, hScreenDC, x, y, SRCCOPY);
    SelectObject(hMemDC, old);

    // Tạo tên file tự động
    int n = 1;
    string filename;
    do {
        filename = "p" + to_string(n++) + ".bmp";
    } while (ifstream(filename).good());

    bool ok = SaveHBITMAPToFile(hBitmap, hScreenDC, filename);

    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    return ok ? "Saved: " + filename + "\n" : "Failed to save screenshot.\n";
}