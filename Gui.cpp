/*! GUI for the expression evalutor.
	Creates a frame which contains a textctrl.
	During textctrl TextEnter event, evaluates the expression on the current line and displays the result in next line.
*/
#include <Expr.h>
#include <wx/wx.h>
#include <wx/textctrl.h>

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

#define NUM_HEX_PER_BYTE	2
#define NUM_DEC_PER_BYTE	3
#define NUM_BIN_PER_BYTE	8
#define END_OF_LINE			2

void MyFrame::PrintResult(number_t number)
{
	char buf[(    sizeof(number)*NUM_HEX_PER_BYTE) + END_OF_LINE +
			 (2 + sizeof(number)*NUM_DEC_PER_BYTE) + END_OF_LINE + 
			 (2 + sizeof(number)*NUM_BIN_PER_BYTE) + END_OF_LINE];
	char *buffer=buf;
	int tot_chars;
	int print_format;
	
	/*Get the symbol value for print format - to decide printing in dec/hex/bin format*/
	print_format = GetIdentifierValue("print_format");
	if ( print_format == 0 )
		print_format = 1;
	if( print_format & 1)
	{
		tot_chars = sprintf(buffer, "%#ld\n", number);
		buffer += tot_chars;
	}
	if( print_format & 2)
	{
		tot_chars = sprintf(buffer, "%#lx\n", number);
		buffer += tot_chars;
	}
	if( print_format & 4)
	{
		int i;
		int bits_per_number = sizeof(number)*NUM_BIN_PER_BYTE;
		buffer[0] = '0';
		buffer[1] = 'b';
		buffer += 2;
		for(i=bits_per_number;i>0;i--)
			buffer[bits_per_number-i] = '0' + ( (number>>(i-1)) & 1) ;
		buffer[bits_per_number] = '\n';
		buffer[bits_per_number+1] = 0;
	}

	/*append the result to the textctrl*/
	textCtrl->AppendText( _(buf) );
}

void PrintResult(number_t number)
{
	global_frame->PrintResult(number);
}
