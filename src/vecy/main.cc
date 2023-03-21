// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <vector>
#include <memory>

#include "glm/vec2.hpp"


class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

glm::vec2 from_to(const glm::vec2& f, const glm::vec2& t)
{
    return t - f;
}

struct CanvasTransform
{
    // canvas view
    glm::vec2 scroll = glm::vec2{0.0f, 0.0f};
    float scale = 1.0f;

    // "config"
    float scale_range_min = 0.1f;
    float scale_range_max = 15.0f;

    void zoom(const glm::vec2& mouse, float zoom)
    {
        // todo(Gustav): change to use screen_to_world
        const auto focus = from_screen_to_world(mouse);

        const float scale_factor = 1 + 0.01f * abs(zoom);

        if (zoom < 0.0f)
        {
            scale /= scale_factor;
        }

        if (zoom > 0.0f)
        {
            scale *= scale_factor;
        }

        scale = std::min(std::max(scale_range_min, scale), scale_range_max);

        const auto new_focus = from_world_to_screen(focus);
        scroll = scroll + from_to(new_focus, mouse);
    }

    [[nodiscard]] glm::vec2 from_screen_to_world(const glm::vec2& p) const
    {
        return (p - scroll) / scale;
    }

    [[nodiscard]] glm::vec2 from_world_to_screen(const glm::vec2& p) const
    {
        return scroll + p * scale;
    }
};

struct Shape
{
    virtual ~Shape() = default;

    virtual void paint(wxDC* dc, const CanvasTransform& transform) = 0;
};

void draw_rectangle(wxDC* dc, const CanvasTransform& t, int xx, int yy, int ww, int hh)
{
    const glm::ivec2 p = t.from_world_to_screen(glm::ivec2{ xx, yy });
    const glm::ivec2 r = t.from_world_to_screen(glm::ivec2{ xx+ww, yy+hh });
    const auto w = r.x - p.x;
    const auto h = r.y - p.y;
    if (w > 0 && h > 0)
    {
        dc->DrawRectangle(wxRect(p.x, p.y, w, h));
    }
}

void draw_line
(
    wxDC* dc,
    const glm::vec2& from,
    const glm::vec2& to,
    const wxColor& color
)
{
    wxPen pen{ color };
    dc->SetPen(pen);
    dc->DrawLine({ static_cast<int>(from.x), static_cast<int>(from.y) }, { static_cast<int>(to.x), static_cast<int>(to.y) });
}

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

    void paint(wxDC* dc, const CanvasTransform& t) override
    {
        wxBrush brush{ color, wxBRUSHSTYLE_SOLID };
        dc->SetBrush(brush);
        dc->SetPen(*wxTRANSPARENT_PEN);
        draw_rectangle(dc, t, x, y, w, h);
    }
};

class CanvasWidget : public wxControl
{
public:
	CanvasWidget(wxWindow* parent, wxWindowID id)
		: wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    {
        shapes.emplace_back(std::make_shared<RectangleShape>(*wxRED, 10, 10, 10, 10));
        shapes.emplace_back(std::make_shared<RectangleShape>(*wxBLUE, 25, 10, 10, 30));
	}

	void OnPaint(wxPaintEvent& event);

    void paint_now();
    void render(wxDC& dc);

    CanvasTransform transform;

    glm::ivec2 mouse0 = { 0,0 };
    glm::vec2 mouse_movement = { 0,0 };
    bool mm_down = false;

    glm::ivec2 get_position(wxMouseEvent& e)
    {
        return { e.GetX(), e.GetY() };
    }

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

void CanvasWidget::mouseMoved(wxMouseEvent& e)
{
    const auto m = get_position(e);

    if (mm_down)
    {
        const auto delta = m - mouse0;
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

void CanvasWidget::mouseWheelMoved(wxMouseEvent& e)
{
    const auto p = get_position(e);
    transform.zoom(p, (e.GetWheelRotation() * e.GetWheelDelta()) / 240.f);
    paint_now();
}

void CanvasWidget::mouseReleased(wxMouseEvent& e)
{
    if (e.GetButton() == wxMOUSE_BTN_MIDDLE)
    {
        mm_down = false;
        transform.scroll += mouse_movement;
        mouse_movement = { 0,0 };
        paint_now();
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
    dc.SetBackground(*wxBLACK_BRUSH);
    dc.Clear();

    auto trans = transform;
    trans.scroll += mouse_movement;

    // draw grid
    const float grid_size = 25.0f;
    {
        wxCoord width = 0;
        wxCoord height = 0;
        GetClientSize(&width, &height);

        const auto size = glm::ivec2{ width, height };

        const float scaled_grid_size = grid_size * trans.scale;

        const wxColor grid_color = *wxGREEN;

        for (float x = fmodf(trans.scroll.x, scaled_grid_size); x < size.x; x += scaled_grid_size)
        {
            draw_line
            (
                &dc,
                glm::vec2(x, 0.0f),
                glm::vec2(x, size.y),
                grid_color
            );
        }

        for (float y = fmodf(trans.scroll.y, scaled_grid_size); y < size.y; y += scaled_grid_size)
        {
            draw_line
            (
                &dc, 
                glm::vec2(0.0f, y),
                glm::vec2(size.x, y),
                grid_color
            );
        }
    }

    for (auto& shape : shapes)
    {
        shape->paint(&dc, trans);
    }

    dc.SetTextForeground(*wxWHITE);

    const auto str = wxString::Format("scale: %f", transform.scale);
    dc.DrawText(str, 0, 0);
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
