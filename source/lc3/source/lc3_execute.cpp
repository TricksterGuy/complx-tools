#include "lc3/lc3_execute.hpp"

#include <algorithm>
#include <cstdlib>

#include "lc3/lc3_plugin.hpp"
#include "lc3/lc3_runner.hpp"

const char* WARNING_MESSAGES[LC3_WARNINGS] =
{
    "W%03d: ""Reading beyond end of input. Halting.",
    "W%03d: ""Writing x%04x to reserved memory at x%04x.",
    "W%03d: ""Reading from reserved memory at x%04x.",
    "W%03d: ""Unsupported Trap x%02x. Assuming Halt.",
    "W%03d: ""Unsupported Instruction x%04x. Halting.",
    "W%03d: ""Malformed Instruction x%04x. Halting.",
    "W%03d: ""RTI executed in user mode. Halting.",
    "W%03d: ""Trying to write invalid character x%04x.",
    "W%03d: ""PUTS called with invalid address x%04x.",
    "W%03d: ""Trying to write to the display when its not ready.",
    "W%03d: ""Trying to read from the keyboard when its not ready.",
    "W%03d: ""Turning off machine via the MCR register.",
    "W%03d: ""PUTSP called with invalid address x%04x",
    "W%03d: ""PUTSP found an unexpected NUL byte at address x%04x.",
    "W%03d: ""Invalid value x%04x loaded into the PSR.",
    "W%03d: ""Executing trap vector table address x%04x.",
    "W%03d: ""Executing interrupt vector table address x%04x.",
};

lc3_state_change lc3_execute(lc3_state& state, uint16_t data)
{
    lc3_instruction instruction(data);

    // Initialize Changes (We don't know everything yet)
    lc3_state_change changes;
    changes.pc = state.pc;
    changes.r7 = state.regs[0x7];
    changes.privilege = state.privilege;
    changes.n = state.n;
    changes.z = state.z;
    changes.p = state.p;
    changes.halted = state.halted;
    changes.changes = LC3_NO_CHANGE;
    changes.location = 0xFFFF;
    changes.value = 0xFFFF;
    changes.savedusp = state.savedusp;
    changes.savedssp = state.savedssp;
    changes.warnings = state.warnings;

    changes.subroutine.address = 0x0;
    changes.subroutine.r6 = 0x0;
    changes.subroutine.is_trap = false;

    if (state.strict_execution && lc3_check_malformed_instruction(instruction))
    {
        state.halted = 1;
        state.pc--;
        lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc]);
        goto post_processing;
    }

    switch(instruction.opcode())
    {
        case BR_INSTR:
            if ((instruction.n() && state.n) || (instruction.z() && state.z) || (instruction.p() && state.p))
                state.pc = state.pc + instruction.pc_offset9();
            break;
        case ADD_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];
            if (instruction.is_imm())
                state.regs[changes.location] = state.regs[instruction.sr1()] + instruction.imm5();
            else
                state.regs[changes.location] = state.regs[instruction.sr1()] + state.regs[instruction.sr2()];
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case LD_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];

            state.regs[changes.location] = lc3_mem_read(state, state.pc + instruction.pc_offset9());
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case ST_INSTR:
            changes.changes = LC3_MEMORY_CHANGE;
            changes.location = state.pc + instruction.pc_offset9();
            changes.value = state.mem[changes.location];

            lc3_mem_write(state, changes.location, state.regs[instruction.dr()]);
            break;
        case JSR_INSTR:
            state.regs[0x7] = state.pc;
            if (instruction.is_jsr())
                state.pc += instruction.pc_offset11();
            // Special case you trash R7...
            else if (instruction.base_r() == 0x7)
                state.pc = changes.r7;
            else
                state.pc = state.regs[instruction.base_r()];

            // Special bookeeping.
            // If not within an interrupt, then don't want to store subroutines within an interrupt.
            if (state.privilege)
            {
                changes.changes = LC3_SUBROUTINE_BEGIN;
                changes.subroutine.address = state.pc;
                changes.subroutine.r6 = state.regs[0x6];

                if (state.call_stack.empty() && state.in_lc3test)
                {
                    int32_t num_params = 0;
                    lc3_subroutine_call_info call_info;
                    call_info.address = state.pc;
                    call_info.r6 = state.regs[0x6];
                    if (state.subroutines.find(state.pc) != state.subroutines.end())
                        num_params = state.subroutines[state.pc].num_params;
                    for (int32_t i = 0; i < num_params; i++)
                    {
                        call_info.params.push_back(state.mem[call_info.r6 + i]);
                    }

           	        for (int32_t i = 0; i < 8; i++)
                    {
                        call_info.regs[i] = state.regs[i];
                    }

                    state.first_level_calls.push_back(call_info);
                }
                if (state.max_call_stack_size != 0)
                {
                    state.call_stack.push_back(changes.subroutine);
                    if (state.max_call_stack_size < state.call_stack.size())
                        state.call_stack.pop_front();
                }
            }
            break;
        case AND_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];
            if (instruction.is_imm())
                state.regs[changes.location] = state.regs[instruction.sr1()] & instruction.imm5();
            else
                state.regs[changes.location] = state.regs[instruction.sr1()] & state.regs[instruction.sr2()];
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case LDR_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];

            state.regs[changes.location] = lc3_mem_read(state, state.regs[instruction.base_r()] + instruction.offset6());
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case STR_INSTR:
            changes.changes = LC3_MEMORY_CHANGE;
            changes.location = state.regs[instruction.base_r()] + instruction.offset6();
            changes.value = state.mem[changes.location];

            lc3_mem_write(state, changes.location, state.regs[instruction.dr()]);
            break;
        case RTI_INSTR:
            if (state.privilege)
            {
                if (state.interrupt_enabled)
                {
                    lc3_signal_interrupt(state, state.priority, 0x00);
                }
                else
                {
                    lc3_warning(state, LC3_USER_RTI, 0);
                    if (!state.true_traps)
                    {
                        state.halted = 1;
                        state.pc--;
                    }
                }
            }
            else
            {
                const bool bits[8] = {0, 1, 1, 0, 1, 0, 0, 0};
                // Pop PC and psr
                state.pc = state.mem[state.regs[6]];
                uint16_t psr = state.mem[state.regs[6] + 1];
                // Invalid PSR check, if the unspecified bits are filled or trying
                // to set multiple nzp bits warn.
                if ((psr & 0x78F8) != 0 || bits[psr & 7] != 1)
                    lc3_warning(state, LC3_INVALID_PSR_VALUE, psr);

                state.regs[6] += 2;
                state.privilege = (psr >> 15) & 1;
                state.priority = (psr >> 8) & 7;
                state.n = (psr >> 2) & 1;
                state.z = (psr >> 1) & 1;
                state.p = psr & 1;

                if (state.privilege) {
                    state.savedssp = state.regs[6];
                    state.regs[6] = state.savedusp;
                }

                // Determine if this RTI is for an interrupt or a trap.
                // If not lc3 2019 revision this should always be true.
                bool in_interrupt = state.lc3_version == 0;
                if (!state.rti_stack.empty())
                {
                    lc3_rti_stack_item item = state.rti_stack.back();
                    state.rti_stack.pop_back();
                    in_interrupt = item.is_interrupt;
                }

                if (in_interrupt)
                {
                    if (!state.interrupt_vector_stack.empty())
                    {
                        state.interrupt_vector = state.interrupt_vector_stack.back();
                        state.interrupt_vector_stack.pop_back();
                    }
                    else
                    {
                        state.interrupt_vector = -1;
                    }

                    changes.changes = LC3_INTERRUPT_END;
                }
                else
                {
                    changes.changes = LC3_SUBROUTINE_END;
                    if (!state.call_stack.empty())
                    {
                        changes.subroutine = state.call_stack.back();
                        state.call_stack.pop_back();
                    }
                }
            }
            break;
        case NOT_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];
            state.regs[changes.location] = ~state.regs[instruction.sr1()];
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case LDI_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];

            state.regs[changes.location] = lc3_mem_read(state, lc3_mem_read(state, state.pc + instruction.pc_offset9()));
            lc3_setcc(state, state.regs[changes.location]);
            break;
        case STI_INSTR:
            changes.changes = LC3_MEMORY_CHANGE;
            changes.location = lc3_mem_read(state, state.pc + instruction.pc_offset9());
            changes.value = state.mem[changes.location];

            lc3_mem_write(state, changes.location, state.regs[instruction.dr()]);
            break;
        case JMP_INSTR:
            state.pc = state.regs[instruction.base_r()];
            if (state.privilege && instruction.base_r() == 0x7)
            {
                changes.changes = LC3_SUBROUTINE_END;
                if (!state.call_stack.empty())
                {
                    changes.subroutine = state.call_stack.back();
                    state.call_stack.pop_back();
                }
            }
            break;
        case LEA_INSTR:
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.dr();
            changes.value = state.regs[changes.location];

            state.regs[changes.location] = state.pc + instruction.pc_offset9();
            // In the 2019 revision of LC-3 LEA no longer sets condition codes.
            if (state.lc3_version == 0)
                lc3_setcc(state, state.regs[changes.location]);
            break;
        case TRAP_INSTR:
            // First version of the lc3 stored return info in R7
            // R7's going to change save it But again its already saved.
            if (state.lc3_version == 0)
                state.regs[0x7] = state.pc;

            // Return information is done via the stack in the lc3 revision and is handled in lc3_trap.
            // Execute the trap
            lc3_trap(state, changes, instruction.vector());
            break;
        case ERROR_INSTR:
            if (state.instructionPlugin)
            {
                // Success use it.
                state.instructionPlugin->OnExecute(state, instruction, changes);
            }
            else
            {
                lc3_warning(state, LC3_UNSUPPORTED_INSTRUCTION, instruction.bits());
                if (!state.true_traps)
                {
                    state.halted = 1;
                    state.pc--;
                }
            }
            break;
        default:
            lc3_warning(state, LC3_UNSUPPORTED_INSTRUCTION, instruction.bits());
            if (!state.true_traps)
            {
                state.halted = 1;
                state.pc--;
            }
            break;
    }

    // Post processing.  If it is a register change and the register is r7
    // then move it.  Though why people would do something like ADD R7, R0, #1 is
    // beyond me...
    post_processing:
    if (changes.changes == LC3_REGISTER_CHANGE && changes.location == 0x7)
    {
        changes.changes = LC3_NO_CHANGE;
        changes.r7 = changes.value;
    }

    return changes;
}

void lc3_trap(lc3_state& state, lc3_state_change& changes, uint8_t vector)
{
    if (state.privilege)
    {
        if (state.call_stack.empty() && state.in_lc3test && vector != TRAP_HALT)
        {
            lc3_trap_call_info call_info;
            call_info.vector = vector;
            for (int32_t i = 0; i < 8; i++)
                call_info.regs[i] = state.regs[i];
            state.first_level_traps.push_back(call_info);
        }
    }

    uint16_t r0 = state.regs[0];
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0);

    // The only nice thing about the revision is that traps now set PSR[15] to 0 for us.
    // No need to check the PC's location.
    if (state.lc3_version != 0)
        kernel_mode = state.privilege == 0;

    if (state.true_traps)
    {
        changes.changes = LC3_SUBROUTINE_BEGIN;
        changes.subroutine.r6 = state.regs[6];
        changes.subroutine.is_trap = true;

        if (state.lc3_version > 0)
        {
            int16_t psr = lc3_psr(state);
            // If we are in user mode we must switch usp/ssp
            if (state.privilege)
            {
                state.savedusp = state.regs[6];
                state.regs[6] = state.savedssp;
            }

            state.privilege = 0;
            state.regs[6] -= 2;
            state.mem[static_cast<uint16_t>(state.regs[6])] = state.pc;
            state.mem[static_cast<uint16_t>(state.regs[6] + 1)] = psr;
            state.rti_stack.push_back(lc3_rti_stack_item{false});
        }

        state.pc = state.mem[vector];
        changes.subroutine.address = state.pc;

        // If not within an interrupt
        /// TODO should check if there are any interrupts on the rti stack.
        if (state.privilege || state.lc3_version > 0)
        {
            if (state.max_call_stack_size != 0)
            {
                state.call_stack.push_back(changes.subroutine);
                if (state.max_call_stack_size < state.call_stack.size())
                    state.call_stack.pop_front();
            }
        }
    }
    else
    {
        switch(vector)
        {
            case TRAP_GETC:
                changes.changes = LC3_REGISTER_CHANGE;
                changes.location = 0;
                changes.value = state.regs[0];
                state.regs[0] = state.reader(state, *state.input);
                break;
            case TRAP_OUT:
                lc3_write_char(state, *state.output, state.regs[0]);
                state.output->flush();
                break;
            case TRAP_PUTS:
                if ((r0 < 0x3000U || r0 >= 0xFE00U) && !kernel_mode)
                {
                    lc3_warning(state, LC3_PUTS_INVALID_MEMORY, r0);
                }
                else
                {
                    while (state.mem[r0] != 0x0000)
                    {
                        lc3_write_char(state,*state.output, state.mem[r0]);
                        r0++;
                    }
                    state.output->flush();
                }
                break;
            case TRAP_IN:
                changes.changes = LC3_REGISTER_CHANGE;
                changes.location = 0;
                changes.value = state.regs[0];
                lc3_write_str(state, state.writer, *state.output, "Input character: ");
                state.regs[0] = state.reader(state, *state.input);
                // Don't call lc3_write_char since it will spit out a warning on non printable character
                state.writer(state, *state.output, state.regs[0]);
                state.output->flush();
                break;
            case TRAP_PUTSP:
                // PUTSP is considered in the appendix to be incorrect.
                // Or at least every implementation I've seen writes it
                // so that it ends when a NUL is encountered and not a
                // memory address with 0x0000 in it. Meh...
                if ((r0 < 0x3000U || r0 >= 0xFE00U) && !kernel_mode)
                {
                    lc3_warning(state, LC3_PUTSP_INVALID_MEMORY, r0);
                }
                else
                {
                    bool putsp_should_stop = false;
                    while (state.mem[r0] != 0x0000)
                    {
                        if (putsp_should_stop)
                            lc3_warning(state, LC3_PUTSP_UNEXPECTED_NUL, r0);
                        uint16_t chunk = state.mem[r0];
                        if ((chunk & 0xFF) != 0)
                            lc3_write_char(state, *state.output, chunk & 0xFF);
                        else
                            lc3_warning(state, LC3_PUTSP_UNEXPECTED_NUL, r0);
                        if ((chunk & 0xFF00) != 0)
                            lc3_write_char(state, *state.output, (chunk >> 8) & 0xFF);
                        else
                            putsp_should_stop = true;
                        r0++;
                    }
                    state.output->flush();
                }

                break;
            case TRAP_HALT:
                state.halted = 1;
                state.pc--;
                // In case anyone writes a bad interrupt and halts in the middle.
                state.privilege = 1;
                state.priority = 0;
                break;
            default:
            {
                if (state.trapPlugins.find(vector) != state.trapPlugins.end())
                {
                    state.trapPlugins[vector]->OnExecute(state, changes);
                }
                else
                {
                    lc3_warning(state, LC3_UNSUPPORTED_TRAP, vector);
                    if (!state.true_traps)
                    {
                        state.halted = 1;
                        state.pc--;
                    }
                }
            }
        }
    }
}

void lc3_setcc(lc3_state& state, int16_t value)
{
    state.n = value < 0;
    state.z = value == 0;
    state.p = value > 0;
}

int16_t lc3_mem_read(lc3_state& state, uint16_t addr, bool privileged)
{
    state.memory_ops[addr].reads++;
    state.total_reads++;

    // You are executing a trap if you are between 0x200 and 0x3000.
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0) || privileged;

    if (addr < 0x3000U || addr >= 0xFE00U)
    {
        switch(addr)
        {
        case DEV_KBSR:
            /// TODO remove this call to lc3_random.  Its ok though since the state is already initialized and the sequence of rand calls is guaranteed to be the same
            /// Unless the code polls from both status registers at the same time...
            if (lc3_random(state) % 16 < 5)
            {
                int val = state.peek(state, *state.input);
                if (val != -1)
                {
                    state.mem[DEV_KBSR] |= 0x8000;
                    state.mem[DEV_KBDR] = val;
                }
            }
            break;
        case DEV_KBDR:
            if (state.mem[DEV_KBSR]) // Will work if interrupts are enabled immediately available!
            {
                state.mem[DEV_KBDR] = state.reader(state, *state.input); // In case of interrupt
                state.mem[DEV_KBSR] &= 0x4000;
            }
            else
            {
                lc3_warning(state, LC3_KEYBOARD_NOT_READY, 0);
                state.mem[DEV_KBDR] = 0;
            }
            break;
        case DEV_DSR:
            /// TODO remove this call to lc3_random.  Its ok though since the state is already initialized and the sequence of rand calls is guaranteed to be the same
            /// Unless the code polls from both status registers at the same time...
            if (lc3_random(state) % 4 < 1)
            {
                state.mem[DEV_DSR] = static_cast<int16_t>(1 << 15);
            }
            break;
        case DEV_DDR:
            if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_READ, addr);
            break;
        case DEV_PSR:
            if (state.lc3_version > 0)
            {
                state.mem[DEV_PSR] = lc3_psr(state);
            } else
            {
                if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                    return state.address_plugins[addr]->OnRead(state, addr);
                else if (!kernel_mode)
                    // Warn if reading from reserved memory if you aren't in kernel mode
                    lc3_warning(state, LC3_RESERVED_MEM_READ, addr);
            }
            break;
        case DEV_MCR:
            state.mem[DEV_MCR] = static_cast<int16_t>(1 << 15);
            break;
        default:
            // Hey does a plugin handle this address
            if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                return state.address_plugins[addr]->OnRead(state, addr);
            else if (!kernel_mode)
                // Warn if reading from reserved memory if you aren't in kernel mode
                lc3_warning(state, LC3_RESERVED_MEM_READ, addr);
            break;
        }
    }

    // Intercept if plugin registered for address.
    if (state.address_plugins.find(addr) != state.address_plugins.end() && state.address_plugins[addr])
            return state.address_plugins[addr]->OnRead(state, addr);

    return state.mem[addr];
}

void lc3_mem_write(lc3_state& state, uint16_t addr, int16_t value, bool privileged)
{
    state.memory_ops[addr].writes++;
    state.total_writes++;

    // You are executing a trap if you are between 0x200 and 0x3000.
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0) || privileged;

    if (addr < 0x3000U || addr >= 0xFE00U)
    {
        switch(addr)
        {
        case DEV_KBSR:
            // Bit 14 is writable
            value &= 0x4000;
            state.mem[DEV_KBSR] &= ~0x4000;
            state.mem[DEV_KBSR] |= value;
            break;
        case DEV_KBDR:
        case DEV_DSR:
            if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, addr);
            break;
        case DEV_DDR:
            if (state.mem[DEV_DSR])
            {
                state.mem[DEV_DSR] = 0;
                lc3_write_char(state, *state.output, value);
                state.output->flush();
            }
            else
            {
                lc3_warning(state, LC3_DISPLAY_NOT_READY, 0);
            }
            break;
        case DEV_PSR:
            if (state.lc3_version > 0) {
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, addr);
                /// TODO consider allowing writing to the PSR.
            } else {
                if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                    state.address_plugins[addr]->OnWrite(state, addr, value);
                else if (!kernel_mode)
                    lc3_warning(state, LC3_RESERVED_MEM_WRITE, value, addr);
            }
            break;
        case DEV_MCR:
            if (!(value & 0x8000))
            {
                if (!kernel_mode) lc3_warning(state, LC3_TURN_OFF_VIA_MCR, 0);
                state.halted = 1;
                state.pc--;
            }
            break;
        default:
            // Hey does a plugin handle this address
            if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                state.address_plugins[addr]->OnWrite(state, addr, value);
            else if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, value, addr);
        }
    }

    // Intercept if plugin registered for address.
    if (state.address_plugins.find(addr) != state.address_plugins.end() && state.address_plugins[addr])
        return state.address_plugins[addr]->OnWrite(state, addr, value);

    state.mem[addr] = value;
}

void lc3_warning(lc3_state& state, uint32_t warn_id, int16_t arg1, int16_t arg2)
{
    state.warn_stats[warn_id] += 1;

    if (state.true_traps)
    {
        // Trigger an exception for these warnings.
        if ((warn_id == LC3_RESERVED_MEM_READ || warn_id == LC3_RESERVED_MEM_WRITE) && state.lc3_version >= 1)
        {
            // This happens in other overload that takes a message below.
            state.warnings++;
            lc3_signal_exception(state, INTERRUPT_ACCCESS_CONTROL_VIOLATION);
            return;
        }
        else if (warn_id == LC3_UNSUPPORTED_INSTRUCTION)
        {
            state.warnings++;
            lc3_signal_exception(state, INTERRUPT_ILLEGAL_OPCODE);
        }
        else if (warn_id == LC3_USER_RTI)
        {
            state.warnings++;
            lc3_signal_exception(state, INTERRUPT_PRIVILEGE);
        }
    }

    if (state.warn_limits.find(warn_id) != state.warn_limits.end() && state.warn_limits[warn_id] <= state.warn_stats[warn_id])
    {
        // This happens in other overload that takes a message below.
        state.warnings++;
        return;
    }

    char warning[128];
    sprintf(warning, WARNING_MESSAGES[warn_id], warn_id, static_cast<uint16_t>(arg1), static_cast<uint16_t>(arg2));
    std::string msg = warning;

    lc3_warning(state, msg);

    if (state.warn_limits.find(warn_id) != state.warn_limits.end() && state.warn_limits[warn_id] <= state.warn_stats[warn_id])
    {
        lc3_warning(state, "Limit for previous warning has been reached will no longer output messages of this type");
        return;
    }
}

void lc3_warning(lc3_state& state, const std::string& msg)
{
    char warning[256];
    std::string message;

    uint16_t addr = state.pc - 1;
    std::string instr = lc3_disassemble(state, state.mem[addr]);

    sprintf(warning, "Warning at x%04x (instruction - %s): %s", addr, instr.c_str(), msg.c_str());

    message = warning;
    (*state.warning) << message << std::endl;

    state.warnings++;
}
