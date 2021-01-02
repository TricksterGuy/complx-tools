#ifndef COLORLCD_HPP
#define COLORLCD_HPP

#include <memory>

#include <lc3.hpp>
#include <lc3_colorlcd/lc3_colorlcd_api.h>

#include <wx/wx.h>
#include <wx/timer.h>

#include "colorlcdgui.h"

#define COLORLCD_MAJOR_VERSION 1
#define COLORLCD_MINOR_VERSION 6

class ColorLCD : public COLORLCDGUI
{
public:
    ColorLCD(wxWindow* top, int width, int height, uint16_t startaddr);
    void Refresh(lc3_state& state);
private:
    int width;
    int height;
    uint16_t startaddr;
};

class LC3_COLORLCD_API ColorLCDPlugin : public Plugin
{
public:
    ColorLCDPlugin(uint16_t width, uint16_t height, uint16_t initaddr, uint16_t startaddr);
    ~ColorLCDPlugin() {}
    void OnWrite(lc3_state& state, uint16_t address, int16_t value) override;
    void Refresh(lc3_state& state) override;
    bool AvailableInLC3Test() const override {return false;}
private:
    uint16_t width;
    uint16_t height;
    uint16_t initaddr;
    uint16_t startaddr;
    std::unique_ptr<ColorLCD> lcd;
};

extern "C"
{
    LC3_COLORLCD_API Plugin* create_plugin(const PluginParams& params);
    LC3_COLORLCD_API void destroy_plugin(Plugin* ptr);
}

#endif
