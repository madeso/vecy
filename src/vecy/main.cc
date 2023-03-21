// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include <memory>




class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

struct vec2i
{
    int x = 0;
    int y = 0;

    vec2i operator-() const
    {
        return { -x, -y };
    }
};


struct Shape
{
    virtual ~Shape() = default;

    virtual void paint(wxDC* dc, const vec2i& transform) = 0;
};

struct RectangleShape : Shape
{
    wxColor color;
    int x = 0; int y = 0;
    int w = 0; int h = 0;

    RectangleShape(wxColor cc, int xx, int yy, int ww, int hh)
        : color(cc)
        , x(xx)
        , y(yy)
        , w(ww)
        , h(hh)
    {
    }

    void paint(wxDC* dc, const vec2i& t) override
    {
        wxBrush brush(color, wxBRUSHSTYLE_SOLID);
        dc->SetBrush(brush);
        dc->DrawRectangle(wxRect(x+t.x, y+t.y, w, h));
    }
};


vec2i operator-(const vec2i& lhs, const vec2i& rhs)
{
    return
    {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}

vec2i operator+(const vec2i& lhs, const vec2i& rhs)
{
    return
    {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}

class CanvasWidget : public wxControl
{
public:
	CanvasWidget(wxWindow* parent, wxWindowID id)
		: wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    {
        shapes.emplace_back(std::make_shared<RectangleShape>(*wxRED, 10, 10, 10, 10));
        shapes.emplace_back(std::make_shared<RectangleShape>(*wxBLUE, 20, 10, 10, 30));
	}

	void OnPaint(wxPaintEvent& event);

    void paint_now();
    void render(wxDC& dc);

    vec2i pan;

    vec2i mouse0;
    vec2i mouse_movement;
    bool mm_down = false;

    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

	DECLARE_EVENT_TABLE();

    std::vector<std::shared_ptr<Shape>> shapes;
};

BEGIN_EVENT_TABLE(CanvasWidget, wxControl)
    EVT_PAINT(CanvasWidget::OnPaint)
    EVT_MOTION(CanvasWidget::mouseMoved)
    EVT_LEFT_DOWN(CanvasWidget::mouseDown)
    EVT_LEFT_UP(CanvasWidget::mouseReleased)
    EVT_RIGHT_DOWN(CanvasWidget::mouseDown)
    EVT_RIGHT_UP(CanvasWidget::mouseReleased)
    EVT_MIDDLE_DOWN(CanvasWidget::mouseDown)
    EVT_MIDDLE_UP(CanvasWidget::mouseReleased)
    EVT_RIGHT_DOWN(CanvasWidget::mouseDown)
    EVT_RIGHT_UP(CanvasWidget::mouseReleased)

    EVT_LEAVE_WINDOW(CanvasWidget::mouseLeftWindow)
    EVT_KEY_DOWN(CanvasWidget::keyPressed)
    EVT_KEY_UP(CanvasWidget::keyReleased)
    EVT_MOUSEWHEEL(CanvasWidget::mouseWheelMoved)
END_EVENT_TABLE()

vec2i get_position(wxMouseEvent& e) { return { e.GetX(), e.GetY() }; }

void CanvasWidget::mouseMoved(wxMouseEvent& e)
{
    const auto m = get_position(e);

    if (mm_down)
    {
        const auto delta = mouse0 - m;
        mouse_movement = delta;
    }

    paint_now();
}

void CanvasWidget::mouseDown(wxMouseEvent& e)
{
    if (e.GetButton() == wxMOUSE_BTN_MIDDLE)
    {
        mouse0 = get_position(e);
        mm_down = true;
        paint_now();
    }
}
void CanvasWidget::mouseWheelMoved(wxMouseEvent&) {}
void CanvasWidget::mouseReleased(wxMouseEvent& e)
{
    if (e.GetButton() == wxMOUSE_BTN_MIDDLE)
    {
        mm_down = false;
        pan = pan + mouse_movement;
        mouse_movement = { 0,0 };
    }
}
void CanvasWidget::mouseLeftWindow(wxMouseEvent&) {}
void CanvasWidget::keyPressed(wxKeyEvent&) {}
void CanvasWidget::keyReleased(wxKeyEvent&) {}

void CanvasWidget::paint_now()
{
    wxClientDC dc(this);
    render(dc);
}

void CanvasWidget::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    render(dc);
}

void CanvasWidget::render(wxDC& dc)
{
    // wxCoord width = 0;
    // wxCoord height = 0;
	// GetClientSize(&width, &height);

    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    const auto transform = -(pan + mouse_movement);

    for (auto& shape : shapes)
    {
        shape->paint(&dc, transform);
    }
}

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    wxDECLARE_EVENT_TABLE();
};


enum
{
    ID_Hello = 1
};


wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()


wxIMPLEMENT_APP(MyApp);


bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Hello World", wxPoint(50, 50), wxSize(450, 340) );
    frame->Show( true );
    return true;
}


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );
    SetMenuBar( menuBar );
    // CreateStatusBar();
    // SetStatusText( "Welcome to wxWidgets!" );

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    CanvasWidget* canvas = new CanvasWidget(this, wxID_ANY);
    sizer->Add(canvas, 1, wxEXPAND);
    SetSizer(sizer);
    SetAutoLayout(true);
}


void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}


void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}


void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
