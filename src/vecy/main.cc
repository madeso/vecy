// wxWidgets "Hello world" Program
// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/graphics.h>

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>

#include "open-color.h"

#include "glm/vec2.hpp"


class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

using u8 = std::uint8_t;
using u64 = std::uint64_t;

struct Id
{
    u64 id;

    bool operator==(const Id& rhs) const
    {
        return id == rhs.id;
    }
};

namespace std
{
	template<>
	struct hash<Id>
	{
		std::size_t operator()(const Id& k) const
		{
            return hash<u64>{}(k.id);
		}
	};
}

struct IdGenerator
{
    u64 next = 0;

    Id create()
    {
        const u64 value = next;
        next += 1;
        return { value };
    }
};

struct Rgba
{
    u8 r;
    u8 g;
    u8 b;
    u8 a;

    Rgba(open_color::Hex h, u8 aa = 255)
        : r((h >> 16) & 0xFF)
        , g((h >> 8) & 0xFF)
        , b((h) & 0xFF)
        , a(aa)
    {
    }

    wxColor to_wx() const
    {
        return wxColor{ r, g, b, a };
    }
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

struct Settings
{
    Rgba background_color = open_color::gray_9;
    Rgba grid_color = open_color::green_9;
    Rgba handle_color = open_color::violet_9;

    Rgba selection_border = open_color::gray_3;
    Rgba selection_positive = Rgba{ open_color::blue_9, 100 };
    Rgba selection_negative = Rgba{ open_color::red_9, 100 };

    float handle_radius = 5.0f;
};

struct Painter;

struct Shape
{
    Id id;
    explicit Shape(Id i) : id(std::move(i)) {}
    virtual ~Shape() = default;

    virtual void paint(Painter*, const CanvasTransform& transform, const Settings& settings, bool state) = 0;

    virtual bool is_hit(const CanvasTransform& t, const glm::vec2& p, float extra) = 0;
};

struct Rect
{
    glm::vec2 topleft;
    glm::vec2 size;

    Rect extend(float x) const
    {
        return
        {
            {topleft.x - x, topleft.y - x},
            {size.x + x * 2.0f, size.y + x * 2.0f}
        };
    }

    bool contains(const glm::vec2& p) const
    {
        const bool outside
            = p.x < topleft.x
             || p.x > topleft.x + size.x
             || p.y > topleft.y + size.y
             || p.y < topleft.y
            ;
        return !outside;
    }

    void include(const glm::vec2& p)
    {
        if (p.x < topleft.x)
        {
            size.x += topleft.x - p.x;
            topleft.x = p.x;
        }
        else if (p.x > topleft.x + size.x)
        {
            size.x = p.x - topleft.x;
        }

        if (p.y < topleft.y)
        {
            size.y += topleft.y - p.y;
            topleft.y = p.y;
        }
        else if (p.y > topleft.y + size.y)
        {
            size.y = p.y - topleft.y;
        }
    }
};

Rect from_world_to_screen(const CanvasTransform& t, const Rect& r)
{
    const glm::vec2 p = t.from_world_to_screen(r.topleft);
    const glm::vec2 s = t.from_world_to_screen(r.topleft + r.size);
    return {p, s - p};
}

enum class LineStyle
    { solid, dot, long_dash, short_dash, dot_dash };

wxPenStyle to_wx(LineStyle style)
{
    switch (style)
    {
        case LineStyle::solid: return wxPENSTYLE_SOLID;
        case LineStyle::dot: return wxPENSTYLE_DOT;
        case LineStyle::long_dash: return wxPENSTYLE_LONG_DASH;
        case LineStyle::short_dash: return wxPENSTYLE_SHORT_DASH;
        case LineStyle::dot_dash: return wxPENSTYLE_DOT_DASH;
        default:
            assert(false);
            return wxPENSTYLE_SOLID;
    }
}

struct Outline
{
    Rgba color;
    int width;
    LineStyle style;
};

wxPen to_wx(std::optional<Outline> o)
{
    if (o)
    {
        return wxPen{o->color.to_wx(), o->width, to_wx(o->style)};
    }
    else
    {
        return *wxTRANSPARENT_PEN;
    }
}

wxBrush to_wx_brush(std::optional<Rgba> o)
{
    if (o)
    {
        return wxBrush{ o->to_wx(), wxBRUSHSTYLE_SOLID};
    }
    else
    {
        return *wxTRANSPARENT_BRUSH;
    }
}

bool is_alpha(const Rgba& color)
{
    return color.a < 255;
}

bool is_alpha(std::optional<Rgba> color, std::optional<Outline> outline)
{
    if (color && is_alpha(*color)) { return true; }
    else if (outline && is_alpha(outline->color)) { return true; }
    else { return false; }
}

struct Painter
{
    wxDC* dc;
    wxGraphicsContext* graphics;

    void clear(const Rgba& color)
    {
        wxBrush brush{ color.to_wx(), wxBRUSHSTYLE_SOLID };
        dc->SetBackground(brush);
        dc->Clear();
    }

    void draw_text(const wxString& str, glm::vec2 p, Rgba color)
    {
        dc->SetTextForeground(color.to_wx());
        dc->DrawText(str, p.x, p.y);
    }

    void draw_rectangle(const Rect& r, std::optional<Rgba> color, std::optional<Outline> outline)
    {
        if (r.size.x > 0 && r.size.y > 0)
        {
            if (is_alpha(color, outline))
            {
                graphics->SetBrush(to_wx_brush(color));
                graphics->SetPen(to_wx(outline));
                graphics->DrawRectangle(r.topleft.x, r.topleft.y, r.size.x, r.size.y);
            }
            else
            {
                dc->SetBrush(to_wx_brush(color));
                dc->SetPen(to_wx(outline));
                dc->DrawRectangle(wxRect(r.topleft.x, r.topleft.y, r.size.x, r.size.y));
            }
        }
    }

    void draw_circle(const glm::vec2& p, float radius, std::optional<Rgba> color, std::optional<Outline> outline)
    {
        if(is_alpha(color, outline))
        {
            graphics->SetBrush(to_wx_brush(color));
            graphics->SetPen(to_wx(outline));
            graphics->DrawEllipse(p.x - radius, p.y - radius, radius * 2, radius * 2);
        }
        else
        {
            dc->SetBrush(to_wx_brush(color));
            dc->SetPen(to_wx(outline));
            dc->DrawEllipse(p.x - radius, p.y - radius, radius * 2, radius * 2);

            // dc->DrawCircle(static_cast<wxCoord>(p.x), static_cast<wxCoord>(p.y), static_cast<wxCoord>(radius));
        }
    }

    void draw_line(const glm::vec2& from, const glm::vec2& to, const Outline& outline)
    {
        if(is_alpha(outline.color))
        {
            wxPoint2DDouble points[2] =
            {
                {from.x, from.y},
                {to.x, to.y}
            };
            graphics->SetPen(to_wx(outline));
            graphics->DrawLines(2, points);
        }
        else
        {
            dc->SetPen(to_wx(outline));
            dc->DrawLine({ static_cast<int>(from.x), static_cast<int>(from.y) }, { static_cast<int>(to.x), static_cast<int>(to.y) });
        }
    }
};

struct RectangleShape : Shape
{
    Rgba color;
    Rect rect;

    RectangleShape(Id i, Rgba cc, Rect r)
        : Shape(std::move(i))
        , color(std::move(cc))
        , rect(std::move(r))
    {
    }

    void paint(Painter* dc, const CanvasTransform& t, const Settings& settings, bool state) override
    {
        dc->draw_rectangle(from_world_to_screen(t, rect), color, std::nullopt);

        if (state)
        {
            const auto dx = glm::vec2{ rect.size.x, 0 };
            const auto dy = glm::vec2{ 0, rect.size.y };
            dc->draw_circle(t.from_world_to_screen(rect.topleft), settings.handle_radius, settings.handle_color, std::nullopt);
            dc->draw_circle(t.from_world_to_screen(rect.topleft + dx), settings.handle_radius, settings.handle_color, std::nullopt);
            dc->draw_circle(t.from_world_to_screen(rect.topleft + dy), settings.handle_radius, settings.handle_color, std::nullopt);
            dc->draw_circle(t.from_world_to_screen(rect.topleft + dx + dy), settings.handle_radius, settings.handle_color, std::nullopt);
        }
    }

    bool is_hit(const CanvasTransform& t, const glm::vec2& p, float extra) override
    {
        return from_world_to_screen(t, rect).extend(extra).contains(p);
    }
};

enum class MouseState
{
    none, left, middle, right
};

class CanvasWidget : public wxControl
{
public:
    CanvasWidget(wxWindow* parent, wxWindowID id)
		: wxControl(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    {
        SetDoubleBuffered(true);

        add(std::make_shared<RectangleShape>(ids.create(), open_color::red_5 , Rect{{10, 10}, {10, 10}}));
        add(std::make_shared<RectangleShape>(ids.create(), open_color::blue_5, Rect{{25, 10}, {10, 30}}));
	}

    void add(std::shared_ptr<Shape> s)
    {
        shapes[s->id] = s;
    }

	void OnPaint(wxPaintEvent& event);

    void paint_now();
    void render(Painter& painter);

    CanvasTransform transform;

    glm::ivec2 mouse0 = { 0,0 };
    glm::ivec2 latest_mouse = { 0,0 };
    glm::vec2 mouse_movement = { 0,0 };
    
    MouseState mouse = MouseState::none;

    CanvasTransform get_current_transform() const
    {
        auto trans = transform;
        trans.scroll += mouse_movement;
        return trans;
    }

    glm::ivec2 get_position(wxMouseEvent& e)
    {
        return { e.GetX(), e.GetY() };
    }

    std::unordered_set<Id> get_hit(const CanvasTransform& t, const glm::vec2& p, float x)
    {
        std::unordered_set<Id> ret;
        for (auto& shape : shapes)
        {
            if (shape.second->is_hit(t, p, x))
            {
                ret.insert(shape.first);
            }
        }
        return ret;
    }

    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);

    void on_erase_background(wxEraseEvent&) {}

	DECLARE_EVENT_TABLE();

    Settings settings;
    IdGenerator ids;
    std::unordered_set<Id> hovers;
    std::unordered_map<Id, std::shared_ptr<Shape>> shapes;
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

    EVT_ERASE_BACKGROUND(CanvasWidget::on_erase_background)
END_EVENT_TABLE()

void CanvasWidget::mouseMoved(wxMouseEvent& e)
{
    const auto m = get_position(e);

    switch (mouse)
    {
    case MouseState::none:
        hovers = get_hit(get_current_transform(), m, 10.0f);
        break;
    case MouseState::middle:
        mouse_movement = m - mouse0;
        latest_mouse = m;
        break;
    case MouseState::left:
        latest_mouse = m;
        break;
    }

    paint_now();
}

void CanvasWidget::mouseDown(wxMouseEvent& e)
{
    if (mouse != MouseState::none)
    {
        return;
    }

    const auto m = get_position(e);

    if (e.GetButton() == wxMOUSE_BTN_LEFT)
    {
        mouse = MouseState::left;
        mouse0 = m;
        latest_mouse = m;
        paint_now();
    }
    else if (e.GetButton() == wxMOUSE_BTN_MIDDLE)
    {
        mouse = MouseState::middle;
        mouse0 = m;
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
    switch(mouse)
    {
    case MouseState::left:
        if (e.GetButton() != wxMOUSE_BTN_LEFT) { return; }
        mouse = MouseState::none;
        paint_now();
        break;
    case MouseState::middle:
        if (e.GetButton() != wxMOUSE_BTN_MIDDLE) { return; }
        mouse = MouseState::none;
        transform.scroll += mouse_movement;
        mouse_movement = { 0,0 };
        paint_now();
        break;
    }
}
void CanvasWidget::mouseLeftWindow(wxMouseEvent&) {}
void CanvasWidget::keyPressed(wxKeyEvent&) {}
void CanvasWidget::keyReleased(wxKeyEvent&) {}

void CanvasWidget::paint_now()
{
    wxClientDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    render(Painter{&dc, gc});
    delete gc;
}

void CanvasWidget::OnPaint(wxPaintEvent&)
{
    wxPaintDC dc(this);
    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    render(Painter{&dc, gc});
    delete gc;
}

void CanvasWidget::render(Painter& dc)
{
    dc.clear(settings.background_color);

    const auto trans = get_current_transform();

    // draw grid
    const float grid_size = 25.0f;
    {
        wxCoord width = 0;
        wxCoord height = 0;
        GetClientSize(&width, &height);

        const auto size = glm::ivec2{ width, height };

        const float scaled_grid_size = grid_size * trans.scale;

        Outline grid_line = { settings.grid_color, 1, LineStyle::solid };

        for (float x = fmodf(trans.scroll.x, scaled_grid_size); x < size.x; x += scaled_grid_size)
        {
            dc.draw_line
            (
                glm::vec2(x, 0.0f),
                glm::vec2(x, size.y),
                grid_line
            );
        }

        for (float y = fmodf(trans.scroll.y, scaled_grid_size); y < size.y; y += scaled_grid_size)
        {
            dc.draw_line
            (
                glm::vec2(0.0f, y),
                glm::vec2(size.x, y),
                grid_line
            );
        }
    }

    for (auto& shape : shapes)
    {
        const auto is_hovering = hovers.find(shape.first) != hovers.end();
        shape.second->paint(&dc, trans, settings, is_hovering);
    }

    // draw selection box
    if (mouse == MouseState::left)
    {
        auto r = Rect{ mouse0, {0, 0} };
        r.include(latest_mouse);
        dc.draw_rectangle(r, mouse0.x < latest_mouse.x ? settings.selection_positive : settings.selection_negative, Outline{ settings.selection_border, 1, LineStyle::long_dash });
    }

    const auto str = wxString::Format("scale: %f", transform.scale);
    dc.draw_text(str, {0, 0}, open_color::white);
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
