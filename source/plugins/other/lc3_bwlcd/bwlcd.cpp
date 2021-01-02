#include "bwlcd.hpp"

#include <cstdlib>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    uint16_t width, height, startaddr, initaddr;
    unsigned int oncolor = 0x606860, offcolor = 0xa0b0a0;

    width = params.read_ushort_required("width");
    height = params.read_ushort_required("height");
    startaddr = params.read_ushort_required("startaddr");
    initaddr = params.read_ushort_required("initaddr");
    params.read_uint("oncolor", oncolor);
    params.read_uint("offcolor", offcolor);

    /*if (wxTheApp == nullptr)
    {
        fprintf(stderr, "[ERROR] Using this plugin with the command line version of the simulator is not supported at this time\n");
        return nullptr;
    }*/

    instance = std::make_unique<BWLCDPlugin>(width, height, initaddr, startaddr, offcolor, oncolor);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

BWLCDPlugin::BWLCDPlugin(uint16_t _width, uint16_t _height, uint16_t _initaddr,
                         uint16_t _startaddr, unsigned int _offcolor, unsigned int _oncolor) :
    Plugin(BWLCD_MAJOR_VERSION, BWLCD_MINOR_VERSION, LC3_OTHER, "Black & White LCD Display"), width(_width),
    height(_height), initaddr(_initaddr), startaddr(_startaddr), offcolor(_offcolor), oncolor(_oncolor), lcd(nullptr)
{
    BindAddress(initaddr);
    BindNAddresses(startaddr, width * height);
}

void BWLCDPlugin::OnWrite(lc3_state& state, uint16_t address, int16_t value)
{
    if (address == initaddr)
    {
        uint16_t data = value;
        if (data == 0x8000U)
        {
            if (!lcd)
            {
                lcd = std::make_unique<BWLCD>(wxTheApp->GetTopWindow(), width, height, startaddr, offcolor, oncolor);
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

void BWLCDPlugin::Refresh(lc3_state& state)
{
    if (lcd)
        lcd->Refresh(state);
}


BWLCD::BWLCD(wxWindow* top, int _width, int _height, uint16_t _startaddr, unsigned int _off, unsigned int _on) :
    BWLCDGUI(top), width(_width), height(_height), startaddr(_startaddr), off(_off), on(_on)
{
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}

void BWLCD::Refresh(lc3_state& state)
{
    wxClientDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    wxColour oncolor(on);
    wxColour offcolor(off);
    wxBrush onBrush(oncolor);
    wxBrush offBrush(offcolor);


    int cw, ch;
    displayPanel->GetSize(&cw, &ch);

    int tw = cw / width;
    int th = ch / height;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (state.mem[startaddr + j + i * width])
                dc.SetBrush(onBrush);
            else
                dc.SetBrush(offBrush);
            dc.DrawRectangle(j * tw, i * th, tw, th);
        }
    }
}
