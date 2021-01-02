#include "colorlcd.hpp"

#include <cstdlib>

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
    height(_height), initaddr(_initaddr), startaddr(_startaddr)
{
    BindAddress(initaddr);
    BindNAddresses(startaddr, width * height);
}

void ColorLCDPlugin::OnWrite(lc3_state& state, uint16_t address, int16_t value)
{
    if (address == initaddr)
    {
        uint16_t data = value;
        if (data == 0x8000U)
        {
            if (!lcd)
            {
                lcd = std::make_unique<ColorLCD>(wxTheApp->GetTopWindow(), width, height, startaddr);
                lcd->Show();
            }
            else
            {
                lc3_warning(state, "LCD is already initialized.");
            }
        }
        else if (data == 0)
        {
            if (!lcd)
            {
                lc3_warning(state, "LCD is already destroyed / not initialized.");
            }
            else
            {
                lcd.reset();
            }
        }
        else
        {
            lc3_warning(state, "Incorrect value written to LCD");
        }
    }
    else if (address >= startaddr && address < startaddr + width * height && !lcd)
    {
        if (!lcd)
            lc3_warning(state, "Writing to LCD while its not initialized!");
    }

    state.mem[address] = value;
}

void ColorLCDPlugin::Refresh(lc3_state& state)
{
    if (lcd)
        lcd->Refresh(state);
}

ColorLCD::ColorLCD(wxWindow* top, int _width, int _height, uint16_t _startaddr) :
    COLORLCDGUI(top), width(_width), height(_height), startaddr(_startaddr)
{
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}

void ColorLCD::Refresh(lc3_state& state)
{
    wxClientDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    int cw, ch;
    displayPanel->GetSize(&cw, &ch);

    int tw = cw / width;
    int th = ch / height;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            uint16_t val = state.mem[startaddr + j + i * width];

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
