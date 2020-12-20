#include "colorlcd.hpp"

#include <cstdlib>
#include <memory>

wxDEFINE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    uint16_t width, height, startaddr, initaddr;

    width = params.read_ushort_required("width");
    height = params.read_ushort_required("height");
    startaddr = params.read_ushort_required("startaddr");
    initaddr = params.read_ushort_required("initaddr");

    /*if (wxTheApp == nullptr)
    {
        fprintf(stderr, "[ERROR] Using this plugin with the command line version of the simulator is not supported at this time\n");
        return nullptr;
    }*/

    instance = std::make_unique<ColorLCDPlugin>(width, height, initaddr, startaddr);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

ColorLCDPlugin::ColorLCDPlugin(uint16_t _width, uint16_t _height, uint16_t _initaddr, uint16_t _startaddr) :
    Plugin(COLORLCD_MAJOR_VERSION, COLORLCD_MINOR_VERSION, LC3_OTHER, "Color LCD Display"), width(_width),
    height(_height), initaddr(_initaddr), startaddr(_startaddr), lcd(nullptr),
    lcd_initializing(false)
{
    BindAddress(initaddr);
    BindNAddresses(startaddr, width * height);
    Connect(wxID_ANY, wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEventHandler(ColorLCDPlugin::InitDisplay));
    Connect(wxID_ANY, wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEventHandler(ColorLCDPlugin::DestroyDisplay));
}

ColorLCDPlugin::~ColorLCDPlugin()
{
    delete lcd;
}

void ColorLCDPlugin::InitDisplay(wxThreadEvent& event)
{
    lcd = new ColorLCD(wxTheApp->GetTopWindow(), width, height, startaddr, event.GetPayload<lc3_state*>());
    lcd_initializing = false;
    lcd->Show();
}

void ColorLCDPlugin::DestroyDisplay(wxThreadEvent& WXUNUSED(event))
{
    delete lcd;
    lcd = nullptr;
}

void ColorLCDPlugin::OnWrite(lc3_state& state, uint16_t address, int16_t value)
{
    if (address == initaddr)
    {
        uint16_t data = value;
        if (data == 0x8000U && lcd == nullptr)
        {
            auto* evt = new wxThreadEvent(wxEVT_COMMAND_CREATE_DISPLAY);
            evt->SetPayload<lc3_state*>(&state);
            wxQueueEvent(this, evt);
            lcd_initializing = true;
        }
        else if (data == 0x8000U && (lcd != nullptr || lcd_initializing))
        {
            lc3_warning(state, "ColorLCD already initialized!");
        }
        else if (data == 0 && lcd == nullptr)
        {
            lc3_warning(state, "ColorLCD is destroyed already!");
        }
        else if (data == 0 && (lcd != nullptr || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else if (static_cast<uint16_t>(state.mem[address]) == 0x8000U && data != 0x8000U && (lcd != nullptr || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else
        {
            lc3_warning(state, "Incorrect value written to ColorLCD");
        }
    }
    else if (address >= startaddr && address < startaddr + width * height && !lcd_initializing)
    {
        if (lcd == nullptr)
            lc3_warning(state, "Writing to LCD while its not initialized!");
    }

    state.mem[address] = value;
}

ColorLCD::ColorLCD(wxWindow* top, int _width, int _height, uint16_t _startaddr, lc3_state* s) :
    COLORLCDGUI(top), timer(this), state(s), width(_width), height(_height), startaddr(_startaddr)
{
    //Centre();
    // 60 fps.
    timer.Start(1000.0 / 60);
    Connect(timer.GetId(), wxEVT_TIMER, wxTimerEventHandler(ColorLCD::OnUpdate), nullptr, this);
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}

void ColorLCD::OnUpdate(wxTimerEvent& WXUNUSED(event))
{
    Refresh();
}

void ColorLCD::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    /// @note see bwlcd.cpp
    if (state == nullptr) return;

    wxPaintDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    int cw, ch;
    displayPanel->GetSize(&cw, &ch);

    int tw = cw / width;
    int th = ch / height;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            uint16_t val = state->mem[startaddr + j + i * width];

            unsigned char r = (val >> 10) & 0x1f;
            unsigned char g = (val >> 5) & 0x1f;
            unsigned char b = val & 0x1f;

            wxColour color(r * 255 / 31, g * 255 / 31, b * 255 / 31);
            wxBrush brush(color);
            dc.SetBrush(brush);

            dc.DrawRectangle(j * tw, i * th, tw, th);
        }
    }
}
