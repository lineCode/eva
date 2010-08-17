/*! GUI for the expression evalutor.
	Creates a frame which contains a textctrl.
	During textctrl TextEnter event, evaluates the expression on the current line and displays the result in next line.
*/
#include <Expr.h>
#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/stc/stc.h>

class MyApp: public wxApp
{
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
	public:
		MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
		void OnTextEnter(wxCommandEvent& event);
		void PrintResult(number_t number);

		DECLARE_EVENT_TABLE()
	private:
		wxTextCtrl *textCtrl;
		wxStyledTextCtrl *stc;
};
MyFrame *global_frame;

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
	EVT_TEXT_ENTER(wxID_ANY, MyFrame::OnTextEnter)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( _("Eva"), wxPoint(50, 50), wxSize(450,340) );
    frame->Show(true);
    SetTopWindow(frame);
	InitEva();
	global_frame = frame;
    return true;
} 
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size) :wxFrame( NULL, -1, title, pos, size )
{
	textCtrl = new wxTextCtrl(this, -1, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER | wxTE_MULTILINE | wxTE_RICH2 | wxTE_DONTWRAP);
	stc = new wxStyledTextCtrl(this);

	textCtrl->GetFont().SetPointSize(20);
    CreateStatusBar();
    SetStatusText( _("Ready") );
}

void MyFrame::OnTextEnter(wxCommandEvent& event)
{
	wxString line;
	long x,y;
	const char *line_chars;
	
	textCtrl->PositionToXY(textCtrl->GetInsertionPoint(), &x, &y);
	line = textCtrl->GetLineText(y-1) + '\n';
	line_chars = line.ToAscii();
	
	EvaluateExpressions(line_chars, strlen(line_chars), 0);
	SetStatusText( line );
}

void MyFrame::PrintResult(number_t number)
{
	char * buf = ConvertNumberToString(number);
	if ( buf )
	{
		/*append the result to the textctrl*/
		textCtrl->AppendText( _(buf) );
		free(buf);
	}
}

void PrintResult(number_t number)
{
	global_frame->PrintResult(number);
}
