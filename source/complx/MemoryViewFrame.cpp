#include "MemoryViewFrame.hpp"
#include <logging.hpp>

enum Tracking
{
    NONE,
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    USP,
    SSP,
};

MemoryViewFrame::MemoryViewFrame(wxWindow* parent, std::reference_wrapper<lc3_state> state, MemoryViewDataModel* model) : MemoryViewFrameDecl(parent), state_ref(state)
{
    EventLog l(__func__);
    memoryView->UpdateRef(state);
    memoryView->AssociateModel(model);
    // Hide the Info column otherwise people may get confused on tracking.
    auto* column = memoryView->GetColumn(0);
    wxASSERT(column && column->GetTitle().IsEmpty());
    column->SetHidden(true);
}

void MemoryViewFrame::OnName(wxCommandEvent& event)
{
    TransferDataFromWindow();
    SetTitle(name);
}

void MemoryViewFrame::OnUpdateShowAddresses(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnShowAddressesCustom(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnDisassemble(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnNormalDisassemble(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnCDisassemble(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnInstructionHighlighting(wxCommandEvent& event)
{
    EventLog l(__func__);
}

void MemoryViewFrame::OnTracking(wxCommandEvent& event)
{
    EventLog l(__func__);
    Refresh();
}

void MemoryViewFrame::Refresh()
{
    TransferDataFromWindow();

    wxASSERT(state_ref);
    lc3_state& state = *state_ref;

    switch(tracking)
    {
        case NONE:
            break;
        case R0:
            memoryView->ScrollTo(state.regs[0]);
            break;
        case R1:
            memoryView->ScrollTo(state.regs[1]);
            break;
        case R2:
            memoryView->ScrollTo(state.regs[2]);
            break;
        case R3:
            memoryView->ScrollTo(state.regs[3]);
            break;
        case R4:
            memoryView->ScrollTo(state.regs[4]);
            break;
        case R5:
            memoryView->ScrollTo(state.regs[5]);
            break;
        case R6:
            memoryView->ScrollTo(state.regs[6]);
            break;
        case R7:
            memoryView->ScrollTo(state.regs[7]);
            break;
        case USP:
            memoryView->ScrollTo(state.savedusp);
            break;
        case SSP:
            memoryView->ScrollTo(state.savedssp);
             break;
        default:
            DFatalLog("Invalid tracking value %d", tracking);
            break;
    }
}