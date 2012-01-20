#include <windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include <Expr.h>
#include "resource.h"

WNDPROC wpOrigEditProc;

static void AppendText(HWND hText, char *buffer)
{
	int ndx;
	
	ndx = GetWindowTextLength(hText);
	SetFocus(hText);
	SendMessage(hText, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	SendMessage(hText, EM_REPLACESEL, 0, (LPARAM)((LPSTR) buffer));
}

static void PrintResult(HWND hText, number_t number)
{
	char * buf;

	buf = ConvertNumberToString(number);
	if (buf == NULL) {
		return;
	}
	
	AppendText(hText, "\r\n=");
	AppendText(hText, buf);
	free(buf);
}

static void ProcessTextChange(HWND hText)
{
	long char_index, line_index, line_length;
	TCHAR *buffer;
	long buffer_size;

	/*Find where the last edit was done character index, line*/
	char_index = SendMessage(hText, EM_LINEINDEX , -1, 0);
	line_index = SendMessage(hText, EM_LINEFROMCHAR, -1, 0);
	/*Find line length*/
	line_length = SendMessage( hText, EM_LINELENGTH, char_index, 0);
	/*Calculate buffer size required to store the entire line*/
	buffer_size = (sizeof(TCHAR) * line_length) + (sizeof(TCHAR) * 2);
	
	/*Allocate buffer*/
	buffer = (TCHAR *)malloc(buffer_size);
	((WORD *)buffer)[0] = buffer_size;
	/*Get the line*/
	line_length = SendMessage(hText, EM_GETLINE, line_index, (LPARAM)buffer) + 1;
	
	if (line_length > buffer_size) {
		line_length = buffer_size;
	}
	buffer[line_length-1]='\n';
	buffer[line_length] = 0;
	
	/*Finally evaluate the expression*/
	EvaluateExpressions((char *)buffer, line_length, 0);
	/*Print the number in user specified format*/
	PrintResult(hText, ExprResult);
	
	/*cleanup*/
	free(buffer);
}

LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT wMessage, WPARAM wParam, LPARAM lParam) 
{
	switch (wMessage)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_CHAR:
			if (wParam == VK_RETURN) {
				ProcessTextChange(hwnd);
			}
	}
	return CallWindowProc(wpOrigEditProc, hwnd, wMessage, wParam, lParam); 
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,  WPARAM wParam, LPARAM lParam)
{
	static HWND hText;
	
    switch(Msg)
    {
		case WM_CREATE:
			/*Create a Text box*/
			hText = CreateWindowEx( 0, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
									0, 0, 0, 0, hWnd, (HMENU)ID_TEXTBOX, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
			wpOrigEditProc = (WNDPROC)SetWindowLongPtr(hText, GWL_WNDPROC, (LONG_PTR)EditSubclassProc); 
			ShowWindow(hText, SW_SHOWNORMAL);
			UpdateWindow(hText);
			break;
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLOREDIT:
		{
			HDC hdc = (HDC)wParam;
			HWND hwnd = (HWND)lParam;
			COLORREF crFg = RGB(255,255,255);	//White for foreground
			COLORREF crBk = RGB(0,0,0);			//Black for background
			
			SetBkColor(hdc, crBk);
			SetTextColor(hdc, crFg);
			SetDCBrushColor(hdc, crBk);
			
			return (LRESULT) GetStockObject(DC_BRUSH);
		}
		case WM_SIZING:
		case WM_SIZE:
			/*Handle resize*/
            MoveWindow(hText, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			break;
		case WM_DESTROY:
			PostQuitMessage(WM_QUIT);
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPCTSTR 	ClsName = (LPCTSTR)"Eva";
	LPCTSTR		WndName = (LPCTSTR)"Expression Evaluator";
	MSG			Msg;
	WNDCLASSEX	WndClsEx;
	HWND		hDlg;

	WndClsEx.cbSize			= sizeof(WNDCLASSEX);
	WndClsEx.style			= CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc	= WndProcedure;
	WndClsEx.cbClsExtra		= 0;
	WndClsEx.cbWndExtra		= 0;
	WndClsEx.hCursor		= LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClsEx.lpszMenuName	= NULL;
	WndClsEx.lpszClassName	= ClsName;
	WndClsEx.hInstance		= hInstance;
	WndClsEx.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CALC_ICON));
    WndClsEx.hIconSm		= (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CALC_ICON), IMAGE_ICON, 16, 16, 0);

	RegisterClassEx(&WndClsEx);
	
#if 0
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
#endif

	/*Initialize Eva*/
	InitEva();
	
	/*Create new window for us*/
	hDlg = CreateWindow(ClsName, WndName, WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL,
						hInstance, NULL);
	ShowWindow(hDlg, SW_SHOWNORMAL);
	UpdateWindow(hDlg);

	/*Message Loop*/
	while(GetMessage(&Msg, NULL, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}
