#ifndef BWLCD_HPP
#define BWLCD_HPP

#include <memory>

#include <lc3.hpp>
#include <lc3_bwlcd/lc3_bwlcd_api.h>

#include <wx/wx.h>

#include "bwlcdgui.h"

#define BWLCD_MAJOR_VERSION 1
#define BWLCD_MINOR_VERSION 6

class BWLCD : public BWLCDGUI
{
public:
    BWLCD(wxWindow* top, int width, int height, uint16_t startaddr, unsigned int off, unsigned int on);
    void Refresh(lc3_state& state);
private:
    int width;
    int height;
    uint16_t startaddr;
    unsigned int off;
    unsigned int on;
};

class LC3_BWLCD_API BWLCDPlugin : public Plugin
{
public:
    BWLCDPlugin(uint16_t width, uint16_t height, uint16_t initaddr, uint16_t startaddr, unsigned int offcolor = 0xa0b0a0, unsigned int oncolor = 0x606860);
    ~BWLCDPlugin() {}
    void OnWrite(lc3_state& state, uint16_t address, int16_t value) override;
    void Refresh(lc3_state& state) override;
    bool AvailableInLC3Test() const override {return false;}
private:
    uint16_t width;
    uint16_t height;
    uint16_t initaddr;
    uint16_t startaddr;
    unsigned int offcolor;
    unsigned int oncolor;
    std::unique_ptr<BWLCD> lcd;
};

extern "C"
{
    LC3_BWLCD_API Plugin* create_plugin(const PluginParams& params);
    LC3_BWLCD_API void destroy_plugin(Plugin* ptr);
}

#endif

