#include "lc3/lc3_os.hpp"

extern std::array<uint16_t, 0x2e3> lc3_osv1;
extern std::array<uint16_t, 0x337> lc3_osv2;

void lc3_load_os(lc3_state& state, int lc3_version)
{
    if (lc3_version == -1)
        lc3_version = state.lc3_version;
    switch (lc3_version)
    {
    case 0:
        std::copy(lc3_osv1.begin(), lc3_osv1.end(), state.mem);
        break;
    case 1:
        std::copy(lc3_osv2.begin(), lc3_osv2.end(), state.mem);
        break;
    default:
        return;
    }
}
