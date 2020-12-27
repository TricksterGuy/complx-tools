#include "lc3/lc3_runner.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <istream>

#include "lc3/lc3_debug.hpp"
#include "lc3/lc3_execute.hpp"
#include "lc3/lc3_os.hpp"
#include "lc3/lc3_plugin.hpp"

void lc3_run(lc3_state& state)
{
    // Do this until halted
    while (!state.halted)
    {
        // Step one instruction
        lc3_step(state);
    }
}

void lc3_run(lc3_state& state, unsigned int num)
{
    unsigned int i = 0;
    // Do this num times or until halted.
    while (i < num && !state.halted)
    {
        // Step one instruction
        lc3_step(state);
        // Increment instruction count
        i++;
    }
}

void lc3_step(lc3_state& state)
{
    // If we are halted then don't step.
    if (state.halted) return;

    if (state.trace != nullptr)
        lc3_trace(state);

    // Tick all plugins
    lc3_tick_plugins(state);
    // Fetch Instruction
    uint16_t data = state.mem[state.pc];
    // Test for blackbox (If the line was a JSR statement and it had a blackbox).
    bool blackbox_finish = lc3_blackbox_test(state);
    // Increment PC
    state.pc++;
    // Form Instruction
    lc3_instr instr = lc3_decode(state, data);
    // Execute Instruction
    const lc3_state_change change = lc3_execute(state, instr);
    // Test for blackbox (If the first line of subroutine had a blackbox).
    if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
        blackbox_finish = blackbox_finish || lc3_blackbox_test(state);
    // Increment executions
    state.executions++;

    if (state.max_stack_size != 0)
    {
        // If the change is INTERRUPT END
        if (change.changes == LC3_INTERRUPT_END)
        {
            // After you've scrubbed all of the floors in hyrule (remove all instructions that have been added except the LC3_INTERRUPT change.
            lc3_state_change* hmm = &state.undo_stack.back();
            while (hmm->changes != LC3_INTERRUPT_BEGIN)
            {
                state.undo_stack.pop_back();
                hmm = &state.undo_stack.back();
            }
            // Please sire have mercy (Change LC3_INTERRUPT_BEGIN to LC3_INTERRUPT to signal a completed interrupt)
            lc3_state_change& lib = state.undo_stack.back();
            lib.changes = LC3_INTERRUPT;
        }
        else
        {
            // Push Old into state
            state.undo_stack.push_back(change);
        }
        // Never pop if you are in privileged mode.

        if (state.privilege && state.max_stack_size < state.undo_stack.size())
            state.undo_stack.pop_front();
    }

    // Tock all plugins
    lc3_tock_plugins(state);

    if (blackbox_finish)
        lc3_finish(state);

    // If we hit an error or a halt instruction return. no need to do any breakpoint tests.
    if (state.halted) return;
    // Breakpoint test
    lc3_break_test(state, &change);

    if (!state.interrupt_enabled) return;

    // Chime in on all observers.
    for (const auto& func : state.interrupt_test)
        func(state);

    // Interrupt?
    if (!lc3_interrupt(state)) return;

    lc3_state_change interrupt;
    interrupt.changes = LC3_INTERRUPT_BEGIN;
    interrupt.warnings = state.warnings;
    interrupt.executions = state.executions;
    if (state.max_stack_size != 0)
        state.undo_stack.push_back(interrupt);
    state.rti_stack.push_back(lc3_rti_stack_item{true});

    // Another breakpoint test
    lc3_break_test(state, &interrupt);
}

void lc3_back(lc3_state& state)
{
    // If there are no changes in the stack we are done
    if (state.undo_stack.empty()) return;
    // Pop Changes from state
    lc3_state_change& changes = state.undo_stack.back();
    // Will not allow to backstep out of running interrupt.
    if (changes.changes == LC3_INTERRUPT_BEGIN) return;

    // Will never happen.
    assert(changes.changes != LC3_INTERRUPT_END);

    if (changes.changes != LC3_INTERRUPT)
    {
        // Unexecute instruction
        state.pc = changes.pc;
        state.regs[0x7] = changes.r7;

        state.privilege = changes.privilege;
        state.n = changes.n;
        state.z = changes.z;
        state.p = changes.p;
        state.halted = changes.halted;
        state.savedusp = changes.savedusp;
        state.savedssp = changes.savedssp;

        if (changes.changes == LC3_REGISTER_CHANGE)
        {
            // Bad if the register is > 7
            assert(changes.location < 0x7);
            state.regs[changes.location] = changes.value;
        }
        else if (changes.changes == LC3_MEMORY_CHANGE)
        {
            state.mem[changes.location] = changes.value;
        }
        else if (changes.changes == LC3_MULTI_CHANGE)
        {
            for (const auto& info : changes.info)
            {
                if (info.is_reg)
                {
                    assert(info.location < 0x7);
                    state.regs[info.location] = info.value;
                }
                else
                {
                    state.mem[info.location] = info.value;
                }
            }
        }
        else if (changes.changes == LC3_SUBROUTINE_BEGIN)
        {
            if (!state.call_stack.empty())
                state.call_stack.pop_back();
            if (changes.subroutine.is_trap && state.lc3_version != 0)
            {
                if (!state.rti_stack.empty())
                    state.rti_stack.pop_back();
                state.regs[0x6] = changes.subroutine.r6;
            }
        }
        else if (changes.changes == LC3_SUBROUTINE_END)
        {
            if (state.max_call_stack_size != 0)
            {
                state.call_stack.push_back(changes.subroutine);
                if (state.max_call_stack_size < state.call_stack.size())
                    state.call_stack.pop_front();
            }
        }

        // Decrement PC
        state.pc--;
        // Decrease execution count
        state.executions--;
    }

    state.warnings = changes.warnings;

    if (changes.changes == LC3_INTERRUPT)
        state.executions = changes.executions;

    state.undo_stack.pop_back();
}

void lc3_rewind(lc3_state& state, unsigned int num)
{
    bool interrupt_begin = false;
    // Do this num times or until no more changes or until we aren't able.
    while (!state.undo_stack.empty() && num > 0 && !interrupt_begin)
    {
        lc3_state_change& last = state.undo_stack.back();
        interrupt_begin = (last.changes == LC3_INTERRUPT_BEGIN);
        // Backstep
        lc3_back(state);
        num--;

    }
}

void lc3_rewind(lc3_state& state)
{
    bool interrupt_begin = false;
    // Do this until no more changes.
    while (!state.undo_stack.empty() && !interrupt_begin)
    {
        lc3_state_change& last = state.undo_stack.back();
        interrupt_begin = (last.changes == LC3_INTERRUPT_BEGIN);
        // Backstep
        lc3_back(state);
    }
}

void lc3_next_line(lc3_state& state)
{
    // Subroutine depth
    int depth = 0;

    do
    {
        // Get Next Instruction.
        lc3_instr instr = lc3_decode(state, state.mem[state.pc]);
        // So if we get a JSR/JSRR or if we get a TRAP and true traps are enabled
        if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
            depth++;

        // If we get a RET instruction JMP R7
        if (instr.data.opcode == JMP_INSTR && instr.jmp.base_r == 7)
            depth--;

        // If we get an RTI instruction
        if (instr.data.opcode == RTI_INSTR)
            depth--;

        // Execute
        lc3_step(state);

        // If we got interrupted
        if (state.interrupt_enabled && state.undo_stack.back().changes == LC3_INTERRUPT_BEGIN)
            depth++;

    }
    while (depth != 0 && !state.halted);
}

void lc3_prev_line(lc3_state& state)
{
    // Subroutine depth
    int depth = 0;

    do
    {
        if (!state.undo_stack.empty())
        {
            lc3_state_change& last = state.undo_stack.back();
            // Can't backstep through interrupt
            if (last.changes == LC3_INTERRUPT_BEGIN)
                return;

            // Get rid of all processed interrupts.
            while (last.changes == LC3_INTERRUPT && !state.undo_stack.empty())
            {
                lc3_back(state);
                last = state.undo_stack.back();
            }
        }

        // Execute (Have to do this first you can't assume mem[pc - 1] was the last
        // instruction due to jumps.
        lc3_back(state);
        // Get the instruction that got you where you are.
        lc3_instr instr = lc3_decode(state, state.mem[state.pc]);
        // If we get a RET instruction JMP R7
        if (instr.data.opcode == JMP_INSTR && instr.jmp.base_r == 7)
            depth++;
        // So if we get a JSR/JSRR or if we get a TRAP and true traps are enabled
        if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
            depth--;
        // Don't have to handle interrupts here...
    }
    while (depth != 0 && !state.halted && !state.undo_stack.empty());
}

void lc3_finish(lc3_state& state)
{
    // Subroutine depth We assume the user is already in a subroutine and just wants to get out of it
    int depth = 1;

    do
    {
        // Get Next Instruction.
        lc3_instr instr = lc3_decode(state, state.mem[state.pc]);
        // So if we get a JSR/JSRR or if we get a TRAP and true traps are enabled
        if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
            depth++;

        // If we get a RET instruction JMP R7
        if (instr.data.opcode == JMP_INSTR && instr.jmp.base_r == 7)
            depth--;

        // If we got an RTI instruction
        if (instr.data.opcode == RTI_INSTR)
            depth--;

        // Execute
        lc3_step(state);

        // If we got interrupted
        if (state.interrupt_enabled && state.undo_stack.back().changes == LC3_INTERRUPT_BEGIN)
            depth++;
    }
    while (depth != 0 && !state.halted);
}

bool lc3_interrupt(lc3_state& state)
{
    // No interrupts? return.
    if (state.interrupts.empty()) return false;

    int my_priority = state.priority;
    int max_priority = -1;
    int max_vector = -1;

    // Try to find a interrupt with higher priority
    auto max_pos = state.interrupts.begin();
    for (auto i = state.interrupts.begin(); i != state.interrupts.end(); ++i)
    {
        const lc3_interrupt_req& inter = *i;
        // We will be interrupted
        // second case is if we generated an exception.
        if (my_priority < inter.priority || (my_priority == inter.priority && inter.vector < 0x80))
        {
            // You will interrupt the guy interrupting
            if (max_priority < inter.priority)
            {
                max_priority = inter.priority;
                max_vector = inter.vector;
                max_pos = i;
            }
        }
    }

    if (max_priority == -1) return false;

    // Interrupt acknowledged.
    state.interrupts.erase(max_pos);

    // HEY PROCESS I'M REALLY HAPPY FOR YOU AND IMMA LET YOU FINISH BUT THIS INTERRUPT HANDLER IS THE BEST HANDLER OF ALL TIME...  OF ALL TIME.
    if (state.privilege) // in user mode
    {
        state.savedusp = state.regs[6];
        state.regs[6] = static_cast<int16_t>(state.savedssp);
    }
    // push PSR&PC to STACK
    int psr = (state.privilege << 15) | (state.priority << 8) | (state.n << 2) | (state.z << 1) | state.p;
    state.regs[6] -= 2;
    state.mem[state.regs[6] + 1] = static_cast<int16_t>(psr);
    state.mem[state.regs[6]] = static_cast<int16_t>(state.pc);

    // Set up new PSR
    state.privilege = 0;
    state.priority = max_priority;
    state.n = 0;
    state.z = 0;
    state.p = 0;

    // Get interrupt vector address contents
    state.pc = state.mem[0x0100 | max_vector];
    if (state.interrupt_vector != -1)
        state.interrupt_vector_stack.push_back(state.interrupt_vector);
    state.interrupt_vector = max_vector;

    return true;
}

void lc3_keyboard_interrupt(lc3_state& state)
{
    lc3_signal_interrupt_once(state, 4, 0x80);
}

void lc3_signal_interrupt(lc3_state& state, int priority, int vector)
{
    state.interrupts.push_back(lc3_interrupt_req{static_cast<unsigned char>(vector), static_cast<unsigned char>(priority)});
}

void lc3_check_keyboard_interrupt(lc3_state& state)
{
    // Note this function is reimplemented as complx_step in the GUI simulator
    // If we aren't executing a keyboard interrupt
    if (state.interrupt_vector != 0x80)
    {
        // If interrupts are enabled for keyboard and interrupts are enabled and there is a character
        if (((state.mem[DEV_KBSR] >> 14) & 1) && state.interrupt_enabled && state.input->peek() != EOF)
        {
            state.keyboard_int_counter++;
            if (state.keyboard_int_counter >= state.keyboard_int_delay)
            {
                lc3_keyboard_interrupt(state);
                state.keyboard_int_counter = 0;
            }
        }
    }
}

bool lc3_signal_interrupt_once(lc3_state& state, int priority, int vector)
{
    // Try to find this interrupt
    for (const auto& inter : state.interrupts)
    {
        if (inter.priority == priority && inter.vector == vector)
            return false;
    }
    lc3_signal_interrupt(state, priority, vector);
    return true;
}

void lc3_tick_plugins(lc3_state& state)
{
    if (state.instructionPlugin != nullptr)
        state.instructionPlugin->OnTick(state);
    for (auto& vector_plugin : state.trapPlugins)
        if (vector_plugin.second != nullptr)
            vector_plugin.second->OnTick(state);
    for (auto* plugin : state.plugins)
        plugin->OnTick(state);
}

void lc3_tock_plugins(lc3_state& state)
{
    if (state.instructionPlugin != nullptr)
        state.instructionPlugin->OnTock(state);
    for (auto i = state.trapPlugins.begin(); i != state.trapPlugins.end(); ++i)
        if (i->second != nullptr)
            i->second->OnTock(state);
    for (unsigned int i = 0; i < state.plugins.size(); i++)
        state.plugins[i]->OnTock(state);
}
