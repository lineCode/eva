
#include <windows.h>
#include <Windowsx.h>
#include <stdio.h>
#include "Expr.h"

#define ID_TEXTBOX	100

HWND	dlg_hWnd;
HWND	hText;
WNDPROC wpOrigEditProc;

void ResizeTextBox()
{
	RECT rcClient;
	
	GetClientRect(dlg_hWnd, &rcClient);
	SetWindowPos(hText, NULL, 0, 0,rcClient.right,rcClient.bottom, SWP_NOZORDER);
}
void AppendText(char *buffer)
{
	int ndx = GetWindowTextLength (hText);
	SetFocus (hText);
	SendMessage (hText, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	SendMessage (hText, EM_REPLACESEL, 0, (LPARAM) ((LPSTR) buffer));
}

void ProcessTextChange()
{
	long char_index, line_index, line_length;
	TCHAR *buffer;
	long buffer_size;

	char_index = SendMessage(hText, EM_LINEINDEX , -1, 0);
	line_index = SendMessage(hText, EM_LINEFROMCHAR, -1, 0);
	line_length = SendMessage( hText, EM_LINELENGTH, char_index, 0);
	
	buffer_size = (sizeof(TCHAR)*line_length) + sizeof(TCHAR)*2;
	buffer = (TCHAR *)malloc( buffer_size );
	((WORD *)buffer)[0] = buffer_size;
	line_length = SendMessage( hText, EM_GETLINE, line_index, (LPARAM)buffer ) + 1;
	
	if (line_length > buffer_size )
		line_length = buffer_size;
	buffer[line_length-1]='\n';
	buffer[line_length] = 0;
	EvaluateExpressions((char *)buffer, line_length, 0);
	free(buffer);
}

LRESULT APIENTRY EditSubclassProc(HWND hwnd, UINT wMessage, WPARAM wParam, LPARAM lParam) 
{ 
	switch (wMessage)
	{
		case WM_GETDLGCODE:
			return DLGC_WANTALLKEYS;
		case WM_CHAR:
			if (wParam == VK_RETURN)
				ProcessTextChange();
	}
	return CallWindowProc(wpOrigEditProc, hwnd, wMessage, wParam, lParam); 
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,  WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
		case WM_DESTROY:
			PostQuitMessage(WM_QUIT);
			break;
		case WM_SIZING:
		case WM_SIZE:
			ResizeTextBox();
			break;
		default:
			return DefWindowProc(hWnd, Msg, wParam, lParam);
    }
    return 0;
}

void PrintResult(number_t number)
{
	char * buf = ConvertNumberToString(number);
	if ( buf == NULL )
		return;
	
	AppendText("\r\n");
	AppendText(buf);
	free(buf);

}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPCTSTR 	ClsName = (LPCTSTR)"Eva";
	LPCTSTR		WndName = (LPCTSTR)"Expression Evaluator";
	MSG			Msg;
	WNDCLASSEX	WndClsEx;

	WndClsEx.cbSize        = sizeof(WNDCLASSEX);
	WndClsEx.style         = CS_HREDRAW | CS_VREDRAW;
	WndClsEx.lpfnWndProc   = WndProcedure;
	WndClsEx.cbClsExtra    = 0;
	WndClsEx.cbWndExtra    = 0;
	WndClsEx.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	WndClsEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClsEx.lpszMenuName  = NULL;
	WndClsEx.lpszClassName = ClsName;
	WndClsEx.hInstance     = hInstance;
	WndClsEx.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&WndClsEx);
	
#if 0
	AllocConsole();
	freopen("CONOUT$", "wb", stdout);
	freopen("CONOUT$", "wb", stderr);
#endif

	InitEva();
	
	dlg_hWnd = CreateWindow(ClsName, WndName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,  CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if( !dlg_hWnd )
		return 0;

	ShowWindow(dlg_hWnd, SW_SHOWNORMAL);
	UpdateWindow(dlg_hWnd);

	hText = CreateWindowEx( 0, "edit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |  ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | WS_VSCROLL | WS_EX_LEFT, 0, 0, 0, 0, dlg_hWnd, (HMENU)(ID_TEXTBOX), hInstance, NULL);
	if( !hText )
		return 0;

	wpOrigEditProc = (WNDPROC) SetWindowLongPtr (hText, GWL_WNDPROC, (LONG_PTR ) EditSubclassProc); 
		
	ShowWindow(hText, SW_SHOWNORMAL);
	UpdateWindow(hText);
	ResizeTextBox();

	while( GetMessage(&Msg, NULL, 0, 0) )
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}
