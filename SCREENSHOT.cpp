#include "stdafx.h"
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <iostream>
#include <atlimage.h> 
#include <Gdiplusimaging.h> 
#pragma comment(lib,"gdiplus.lib")
#pragma comment (lib, "user32.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
using namespace Gdiplus;

void SaveBitmap(char *szFilename,HBITMAP hBitmap);
void CaptureScreen();
void SaveBitmap2(char *szFilename,HBITMAP hBitmap);
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

DWORD FetchKeyCode( )
{
    DWORD i     = 0x00;
    DWORD ret   = 0x00;
    do{
        if( GetAsyncKeyState(i)&0x1 )
            ret = i;
        i++;
    }while( !ret && i < 0xFF );

    return ret;
}
char VKtoCHAR(int vk)
{
    static UCHAR keyboardState[256];
    USHORT asciiVal;
    static HKL layout = GetKeyboardLayout(0);     // get layout
    if( GetKeyboardState(keyboardState) == false )// get keyboard state (shift etc)
        return 0;
    if( ToAsciiEx( vk, MapVirtualKey(vk,0), keyboardState, &asciiVal, 0, layout ) == 0 )//  ASCII
        return 0;
    return static_cast<char>(asciiVal);
}


int ppk()
{
	INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    // Press the "A" key
    ip.ki.wVk = 0x2C; // virtual-key code for the "a" key
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    // Release the "A" key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));	 
	Sleep(500);
	return 0;
}

bool ImageFromClipboard()
{
	if (/*IsClipboardFormatAvailable(CF_UNICODETEXT) ||*/ IsClipboardFormatAvailable(CF_TEXT))
	{
		if (!OpenClipboard(GetOpenClipboardWindow()))
		{
			printf("NIMA NIC W SCHOWKU\n");
			return false;
		}
 
		HANDLE a = GetClipboardData(CF_TEXT);
		LPWORD b = 0;
		LARGE_INTEGER *c = 0;
		ReadFile(a, b, GetFileSizeEx(a, c), 0, 0);
		printf("%s", b);

		CloseClipboard();
	}
	return true;
}
 
int main()
{
	HWND hWnd = GetConsoleWindow();
	CString w = "x.bmp";
	//HWND hWnd;
	HBITMAP hBitmap;
	CImage image; 
	//CaptureScreen();
	/*OPENFILENAME	ofn;
	char	szFileName[512]; 
	strcpy_s(szFileName, 512, "ScreenShot.bmp");

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.Flags=OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
	ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
	ofn.lpstrDefExt= L"bmp";
	ofn.lpstrFile = L"a.bmp";
	ofn.nMaxFile = 512;
	//ofn.hwndOwner = hWnd;*/
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);

	ShowWindow( hWnd, SW_HIDE );
	FreeConsole();

    HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, 
                            nScreenWidth, nScreenHeight);
    SelectObject(hCaptureDC,hCaptureBitmap); 
    BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,
           hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 
	//@TEST

	image.Attach(hCaptureBitmap);
	image.Save(L"babe.png"); 

    //delete image;
    /*ReleaseDC(hDesktopWnd,hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);*/

//	Sleep(1000);

	//while( true )
	//{
		//Sleep(1000*1);
		/*hWnd = GetDesktopWindow();
		keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY, 0);
		keybd_event(VK_SNAPSHOT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

		WaitForSingleObject(hWnd, 1500);*/
		 // Simulate a key press
        /*keybd_event( VK_NUMLOCK,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | 0,
                      0 );
		// Simulate a key release
         keybd_event( VK_NUMLOCK,
                      0x45,
                      KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
                      0);*/
		//hBitmap = ImageFromClipboard(hWnd);

		/*OpenClipboard(NULL);
		hBitmap = (HBITMAP) GetClipboardData(CF_BITMAP);

		if (hBitmap == NULL)
		{ printf("????") ; }
		image.Attach(hBitmap); 
		image.Save(w, Gdiplus::ImageFormatBMP); 

		Sleep(1000);*/
	//}
	return 0;
}
//**********************************************************************************************
void SaveBitmap(char *szFilename,HBITMAP hBitmap)
{
	HDC					hdc=NULL;
	FILE*				fp=NULL;
	LPVOID				pBuf=NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader;

	do
	{
		hdc=GetDC(NULL);
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

		if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBox(NULL,_T("Unable to Allocate Bitmap Memory"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}
		
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf,&bmpInfo,DIB_RGB_COLORS);	

		if((fopen_s(&fp, "a","wb"))==NULL)
		{
			MessageBox(NULL,_T("Unable to Create Bitmap File"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}

		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);

	}while(false);

		if(hdc)
			ReleaseDC(NULL,hdc);

		if(pBuf)
			free(pBuf);

		if(fp)
			fclose(fp);
}
//************************************************************************************************
void CaptureScreen()
{
	OPENFILENAME	ofn;
	char	szFileName[512]; 
	strcpy_s(szFileName, 512, "ScreenShot.bmp");

	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize=sizeof(OPENFILENAME);
	ofn.Flags=OFN_HIDEREADONLY|OFN_PATHMUSTEXIST;
	ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
	ofn.lpstrDefExt= L"bmp";
	ofn.lpstrFile = L"a.bmp";
	ofn.nMaxFile = 512;
	//ofn.hwndOwner = hWnd;
    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, 
                            nScreenWidth, nScreenHeight);
    SelectObject(hCaptureDC,hCaptureBitmap); 
    BitBlt(hCaptureDC,0,0,nScreenWidth,nScreenHeight,
           hDesktopDC,0,0,SRCCOPY|CAPTUREBLT); 
    SaveBitmap2("a.bmp", hCaptureBitmap); //Place holder - Put your code
                                //here to save the captured image to disk
    ReleaseDC(hDesktopWnd,hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);
}
//**************************************************************************************
void	SaveBitmap2(char *szFilename,HBITMAP hBitmap)
{
	HDC					hdc=NULL;
	FILE*				fp=NULL;
	LPVOID				pBuf=NULL;
	BITMAPINFO			bmpInfo;
	BITMAPFILEHEADER	bmpFileHeader;

	do
	{
		hdc=GetDC(NULL);
		ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));
		bmpInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
		GetDIBits(hdc,hBitmap,0,0,NULL,&bmpInfo,DIB_RGB_COLORS);

		if(bmpInfo.bmiHeader.biSizeImage<=0)
			bmpInfo.bmiHeader.biSizeImage=bmpInfo.bmiHeader.biWidth*abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

		if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
		{
			MessageBox(NULL,_T("Unable to Allocate Bitmap Memory"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}
		
		bmpInfo.bmiHeader.biCompression=BI_RGB;
		GetDIBits(hdc,hBitmap,0,bmpInfo.bmiHeader.biHeight,pBuf,&bmpInfo,DIB_RGB_COLORS);	
		fopen_s(&fp, szFilename,"wb");
		/*if((fopen_s(&fp, szFilename,"wb"))==NULL)
		{
			MessageBox(NULL,_T("Unable to Create Bitmap File"),_T("Error"),MB_OK|MB_ICONERROR);
			break;
		}*/

		bmpFileHeader.bfReserved1=0;
		bmpFileHeader.bfReserved2=0;
		bmpFileHeader.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bmpInfo.bmiHeader.biSizeImage;
		bmpFileHeader.bfType='MB';
		bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

		fwrite(&bmpFileHeader,sizeof(BITMAPFILEHEADER),1,fp);
		fwrite(&bmpInfo.bmiHeader,sizeof(BITMAPINFOHEADER),1,fp);
		fwrite(pBuf,bmpInfo.bmiHeader.biSizeImage,1,fp);

	}while(false);

		if(hdc)
			ReleaseDC(NULL,hdc);

		if(pBuf)
			free(pBuf);

		if(fp)
			fclose(fp);
}
//**********************************************************************************
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
        UINT  num = 0;          // number of image encoders
        UINT  size = 0;         // size of the image encoder array in bytes

        ImageCodecInfo* pImageCodecInfo = NULL;

        GetImageEncodersSize(&num, &size);
        if(size == 0)
        return -1;  // Failure

        pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if(pImageCodecInfo == NULL)
        return -1;  // Failure

        GetImageEncoders(num, size, pImageCodecInfo);

        for(UINT j = 0; j < num; ++j)
        {
                if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
                {
                        *pClsid = pImageCodecInfo[j].Clsid;
                        free(pImageCodecInfo);
                        return j;  // Success
                }
        }

        free(pImageCodecInfo);
        return -1;  // Failure
}