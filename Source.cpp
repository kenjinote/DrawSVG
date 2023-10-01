#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "d2d1")

#include <windows.h>
#include <shlwapi.h>
#include <d2d1svg.h>
#include <d2d1_3.h>

TCHAR szClassName[] = TEXT("Window");

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static ID2D1Factory* pFactory;
	static ID2D1HwndRenderTarget* pRenderTarget;
	static ID2D1SvgDocument* pSvgDocument;
	static ID2D1DeviceContext5* d2dContext;
	switch (msg)
	{
	case WM_CREATE:
		{
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
			pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(640, 480)), &pRenderTarget);
			pRenderTarget->QueryInterface(&d2dContext);
			IStream* pStream = NULL;
			HRESULT hr = SHCreateStreamOnFile(L"tiger.svg", STGM_READ, &pStream);
			d2dContext->CreateSvgDocument(pStream, D2D1::SizeF(640, 480), &pSvgDocument);
			pStream->Release();
			pStream = NULL;
		}
		break;
	case WM_PAINT:
		ValidateRect(hWnd, NULL);
		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		d2dContext->DrawSvgDocument(pSvgDocument);
		pRenderTarget->EndDraw();
		break;
	case WM_SIZE:
		{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			pRenderTarget->Resize(D2D1::SizeU(width, height));
		}
		break;
	case WM_DESTROY:
		pSvgDocument->Release();
		pSvgDocument = NULL;
		pRenderTarget->Release();
		pRenderTarget = NULL;
		pFactory->Release();
		pFactory = NULL;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	MSG msg;
	WNDCLASS wndclass = {
		0,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		0,
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Draw SVG"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
