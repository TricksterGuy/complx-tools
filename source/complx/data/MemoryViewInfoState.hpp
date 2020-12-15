#ifndef MEMORY_VIEW_INFO_STATE_HPP
#define MEMORY_VIEW_INFO_STATE_HPP

/// Represents what to render in the Info Column in MemoryView.
enum MemoryViewInfoState
{
    DRAW_PC = 1,
    DRAW_BREAKPOINT = 2,
    DRAW_WATCHPOINT = 4,
    DRAW_BLACKBOX = 8,
    BREAKPOINT_DISABLED = 16,
    WATCHPOINT_DISABLED = 32,
    BLACKBOX_DISABLED = 64,
    IS_HALTED = 128,
};

#endif

