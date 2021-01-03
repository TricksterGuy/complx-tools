#ifndef MEMORY_VIEW_FRAME_HPP
#define MEMORY_VIEW_FRAME_HPP

#include <lc3.hpp>
#include "gen/ComplxFrameDecl.h"
#include "MemoryView.hpp"
#include "data/MemoryViewDataModel.hpp"

class MemoryViewFrame : public MemoryViewFrameDecl
{
public:
    MemoryViewFrame(wxWindow* parent, std::reference_wrapper<lc3_state> state, MemoryViewDataModel* model);
    ~MemoryViewFrame() {}
    void OnName(wxCommandEvent& event) override;
    void OnUpdateShowAddresses(wxCommandEvent& event) override;
    void OnShowAddressesCustom(wxCommandEvent& event) override;
    void OnDisassemble(wxCommandEvent& event) override;
    void OnNormalDisassemble(wxCommandEvent& event) override;
    void OnCDisassemble(wxCommandEvent& event) override;
    void OnInstructionHighlighting(wxCommandEvent& event) override;
    void OnTracking(wxCommandEvent& event) override;
    void Refresh();
    /** Updates the reference wrapper. Usually done after loading a new assembly file.
        All views referring to this data model must be refreshed after a call to this function.
     */
    void UpdateRef(std::reference_wrapper<lc3_state> new_value)
    {
        state_ref = new_value;
        memoryView->UpdateRef(new_value);
        Refresh();
    }

private:
    std::optional<std::reference_wrapper<lc3_state>> state_ref;
};

#endif
