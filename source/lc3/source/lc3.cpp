#include "lc3/lc3.hpp"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <iostream>
#include <istream>
#include <sstream>

#include "lc3/lc3_execute.hpp"
#include "lc3/lc3_expressions.hpp"
#include "lc3/lc3_os.hpp"
#include "lc3/lc3_plugin.hpp"
#include "lc3/lc3_symbol.hpp"

void lc3_init(lc3_state& state, bool randomize_registers, bool randomize_memory, int16_t register_fill_value, int16_t memory_fill_value)
{
    state.dist.reset();
    state.rng.seed(state.default_seed);

    // Set Registers
    state.regs[0] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[1] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[2] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[3] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[4] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[5] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[6] = randomize_registers ? lc3_random(state) : register_fill_value;
    state.regs[7] = randomize_registers ? lc3_random(state) : register_fill_value;

    // PC is initially at address 3000
    /// TODO Add PC parameter and avoid hardcoding this value.
    state.pc = 0x3000;

    // User mode
    state.privilege = 1;
    state.priority = 0;

    // Set Control Flags
    int16_t rand_value = randomize_registers ? lc3_random(state) : register_fill_value;
    state.n = rand_value < 0;
    state.z = rand_value == 0;
    state.p = rand_value > 0;

    // Set Additional Flags
    state.halted = 0;
    state.true_traps = 0;
    state.warnings = 0;
    state.executions = 0;
    state.interrupt_enabled = 0;
    state.strict_execution = 1;
    state.lc3_version = 1;

    // Clear subroutine info
    state.max_call_stack_size = -1;
    state.call_stack.clear();

    state.warn_stats.clear();
    state.warn_limits.clear();
    state.warn_limits[LC3_INVALID_CHARACTER_WRITE] = 100;
    state.warn_limits[LC3_RESERVED_MEM_WRITE] = 100;
    state.warn_limits[LC3_RESERVED_MEM_READ] = 100;
    state.warn_limits[LC3_PUTSP_UNEXPECTED_NUL] = 100;
    state.warn_limits[LC3_EXECUTE_IVT] = 1;
    state.warn_limits[LC3_EXECUTE_TVT] = 1;


    // Set Stack Flags
    state.max_stack_size = -1;
    state.undo_stack.clear();

    // Set I/O Stuff
    state.input = &std::cin;
    state.reader = lc3_read_char;
    state.peek = lc3_peek_char;
    state.output = &std::cout;
    state.writer = lc3_do_write_char;
    state.warning = &std::cout;

    // Clear memory
    if (randomize_memory)
        lc3_randomize(state);
    else
        std::fill(state.mem, state.mem + 65536, memory_fill_value);

    // Add LC3 OS
    lc3_load_os(state);

    // Clear plugins
    lc3_remove_plugins(state);

    // Clear Symbol Table
    state.symbols.clear();
    state.rev_symbols.clear();

    // Clear Breakpoints and all that jazz
    state.breakpoints.clear();
    state.comments.clear();
    state.reg_watchpoints.clear();
    state.mem_watchpoints.clear();
    state.subroutines.clear();

    // Clear pending interrupts
    state.interrupts.clear();
    state.interrupt_test.clear();
    state.interrupt_vector = -1;
    state.savedssp = 0x3000;
    state.savedusp = 0xF000;

    state.keyboard_int_counter = 0;
    state.keyboard_int_delay = DEFAULT_KEYBOARD_INTERRUPT_DELAY;

    state.memory_ops.clear();
    state.total_reads = 0;
    state.total_writes = 0;

    state.trace = nullptr;

    state.in_lc3test = false;
}

void lc3_set_version(lc3_state& state, int version)
{
    if (version >= 0 && version <= 1)
    {
        state.lc3_version = version;
        lc3_load_os(state, version);
    }
    else
    {
        fprintf(stderr, "Invalid lc3 version: %d. Valid values are 0 or 1\n", version);
    }
}

void lc3_remove_plugins(lc3_state& state)
{
    state.instructionPlugin = nullptr;
    state.plugins.clear();
    state.address_plugins.clear();
    state.trapPlugins.clear();
    state.interruptPlugin.clear();

    // Destroy all plugins
    for (const auto& file_plugin : state.filePlugin)
    {
        const PluginInfo& infos = file_plugin.second;
        infos.destroy(infos.plugin);
        dlclose(infos.handle);
    }
    state.filePlugin.clear();

    // Set up "dummy plugins" to sit on reserved addresses
    state.trapPlugins[TRAP_GETC]  = nullptr;
    state.trapPlugins[TRAP_OUT]   = nullptr;
    state.trapPlugins[TRAP_PUTS]  = nullptr;
    state.trapPlugins[TRAP_IN]    = nullptr;
    state.trapPlugins[TRAP_PUTSP] = nullptr;
    state.trapPlugins[TRAP_HALT]  = nullptr;

    state.address_plugins[DEV_KBSR] = nullptr;
    state.address_plugins[DEV_KBDR] = nullptr;
    state.address_plugins[DEV_DSR]  = nullptr;
    state.address_plugins[DEV_DDR]  = nullptr;
    state.address_plugins[DEV_MCR]  = nullptr;

    if (state.lc3_version >= 1)
        state.address_plugins[DEV_PSR] = nullptr;
}

const char* BASIC_DISASSEMBLE_LOOKUP[16][2] =
{
    {"BR%s%s%s #%d", "NOP%s"             },
    {"ADD R%d, R%d, R%d", "ADD R%d, R%d, #%d" },
    {"LD R%d, #%d", ""                  },
    {"ST R%d, #%d", ""                  },
    {"JSRR R%d", "JSR #%d"           },
    {"AND R%d, R%d, R%d", "AND R%d, R%d, #%d" },
    {"LDR R%d, R%d, #%d", ""                  },
    {"STR R%d, R%d, #%d", ""                  },
    {"RTI", ""                  },
    {"NOT R%d, R%d", ""                  },
    {"LDI R%d, #%d", ""                  },
    {"STI R%d, #%d", ""                  },
    {"JMP R%d", "RET"               },
    {"ERROR", ""                  },
    {"LEA R%d, #%d", ""                  },
    {"TRAP x%02x", ""                  },
};

const char* DISASSEMBLE_LOOKUP[16][3] =
{
    {"BR%s%s%s #%d", "BR%s%s%s %s", "NOP%s"           },
    {"ADD R%d, R%d, R%d", "ADD R%d, R%d, #%d", ""                },
    {"LD R%d, #%d", "LD R%d, %s", ""                },
    {"ST R%d, #%d", "ST R%d, %s", ""                },
    {"JSRR R%d", "JSR #%d", "JSR %s"          },
    {"AND R%d, R%d, R%d", "AND R%d, R%d, #%d", ""                },
    {"LDR R%d, R%d, #%d", "", ""                },
    {"STR R%d, R%d, #%d", "", ""                },
    {"RTI", "", ""                },
    {"NOT R%d, R%d", "", ""                },
    {"LDI R%d, #%d", "LDI R%d, %s", ""                },
    {"STI R%d, #%d", "STI R%d, %s", ""                },
    {"JMP R%d", "RET", ""                },
    {"ERROR", "", ""                },
    {"LEA R%d, #%d", "LEA R%d, %s", ""                },
    {"TRAP x%02X", "", ""                },
};

const char* TRAP_CASES[6] = {"GETC", "OUT", "PUTS", "IN", "PUTSP", "HALT"};

//nul(NOP), P, Z, ZP, N, NP, NZ, nul(NZP)
const char* BR_CASES[8] = {"", ">", "=", ">=", "<", "!=", "<=", ""};

const char* ADV_DISASSEMBLE_LOOKUP[16][8] =
{
    {"NOP%s", "if cc%s0, PC = %s", "PC = %s", "if cc%s0, PC %s= %d", "PC %s= %d"},
    {"R%d += R%d", "R%d = R%d + R%d", "TEST R%d", "R%d = R%d", "R%d++", "R%d--", "R%d %s= %d", "R%d = R%d %s %d"},
    {"R%d = %s", "R%d = (PC %s %d)", "R%d = PC"}, //LD
    {"%s = R%d", "(PC %s %d) = R%d", "PC = R%d"}, //ST
    {"CALL %s", "CALL PC %s %d", "CALL R%d", "CALL PC"}, //
    {"TEST R%d", "R%d = R%d", "R%d &= R%d", "R%d = R%d & R%d", "R%d = 0", "R%d &= %d", "R%d = R%d & %d"},
    {"R%d = R%d[%d]"},
    {"R%d[%d] = R%d"},
    {"RTI"},
    {"R%d = ~R%d"},
    {"R%d = *%s", "R%d = *(PC %s %d)", "R%d = *PC"}, // LDI
    {"*%s = R%d", "*(PC %s %d) = R%d", "*PC = R%d"}, // STI
    {"JUMP R%d", "RET"},
    {"ERROR"},
    {"R%d = &%s", "R%d = PC %s %d", "R%d = PC"}, // LEA
    {"GETC", "OUT", "PUTS", "IN", "PUTSP", "HALT", "TRAP x%02x"},
};

std::string lc3_basic_disassemble(lc3_state& state, uint16_t data, uint16_t /* unused pc */)
{
    char buf[128];

    lc3_instruction instr(data);
    uint8_t opcode = instr.opcode();

    switch(opcode)
    {
        case BR_INSTR:
            // If all flags are off or offset is 0
            if (!(instr.n() || instr.z() || instr.p()) || instr.pc_offset9() == 0)
            {
                if (!instr.n() && !instr.z() && !instr.p())
                {
                    // Hey do I have a printable character?
                    if (isprint(instr.pc_offset9()))
                    {
                        // Write the character
                        char minibuf[7];
                        snprintf(minibuf, 7, " ('%c')", instr.pc_offset9());
                        // NOP (with character)
                        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], minibuf);
                    }
                    else
                    {
                        // NOP no character
                        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], "");
                    }
                }
                else
                    // NOP2
                    sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], "");
            }
            // If not all flags are on
            else if (!(instr.n() && instr.z() && instr.p()))
            {
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.n() ? "N" : "",
                        instr.z() ? "Z" : "", instr.p() ? "P" : "", instr.pc_offset9());
            }
            // All flags are on then
            else
            {
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], "", "", "", instr.pc_offset9());
            }
            break;
        case ADD_INSTR:
        case AND_INSTR:
            if (instr.is_imm())
            {
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1],
                        instr.dr(), instr.sr1(), instr.imm5());
            }
            else
            {
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0],
                        instr.dr(), instr.sr1(), instr.sr2());
            }
            break;
        case NOT_INSTR:
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.sr1());
            break;
        case LD_INSTR:
        case ST_INSTR:
        case LEA_INSTR:
        case LDI_INSTR:
        case STI_INSTR:
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.dr(),
                    instr.pc_offset9());
            break;
        case LDR_INSTR:
        case STR_INSTR:
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.base_r(),
                    instr.offset6());
            break;
        case JSR_INSTR: // JSRR_INSTR
            if (instr.is_jsr())
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], instr.pc_offset11());
            else
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.base_r());
            break;
        case JMP_INSTR: // RET_INSTR
            if (instr.base_r() == 0x7)
                sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][1]);
            else
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.base_r());
            break;
        case TRAP_INSTR:
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.vector());
            break;
        case RTI_INSTR:
            sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        case ERROR_INSTR:
            if (state.instructionPlugin)
                return state.instructionPlugin->OnDisassemble(state, instr, LC3_BASIC_DISASSEMBLE);
            else
                sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        default:
            sprintf(buf, "Unknown instruction x%04x", data);
    }

    return buf;
}

std::string lc3_normal_disassemble(lc3_state& state, uint16_t data, uint16_t pc)
{
    lc3_instruction instr(data);
    uint32_t opcode = instr.opcode();
    char buf[128];
    int32_t offset;
    std::string label;

    switch(opcode)
    {
        case BR_INSTR:
            offset = instr.pc_offset9();
            label = lc3_sym_rev_lookup(state, pc + offset);
            // If all flags are off
            if (!(instr.n() || instr.z() || instr.p()) || instr.pc_offset9() == 0)
            {
                if (!instr.n() && !instr.z() && !instr.p())
                {
                    // Hey do I have a character?
                    if (isprint(instr.pc_offset9()))
                    {
                        char minibuf[7];
                        snprintf(minibuf, 7, " ('%c')", instr.pc_offset9());
                        // NOP (maybe with character)
                        sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], minibuf);
                    }
                    else
                    {
                        // NOP no character
                        sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], "");
                    }
                }
                else
                    // NOP2
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], "");
            }
            // If not all flags are on
            else if (!(instr.n() && instr.z() && instr.p()))
            {
                if (label.empty())
                {
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.n() ? "N" : "",
                            instr.z() ? "Z" : "", instr.p() ? "P" : "", instr.pc_offset9());
                }
                else
                {
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.n() ? "N" : "",
                            instr.z() ? "Z" : "", instr.p() ? "P" : "", label.c_str());
                }
            }
            // All flags are on then
            else
            {
                if (label.empty())
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], "", "", "", instr.pc_offset9());
                else
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], "", "", "", label.c_str());
            }
            break;
        case ADD_INSTR:
        case AND_INSTR:
            if (instr.is_imm())
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1],
                        instr.dr(), instr.sr1(), instr.imm5());
            }
            else
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0],
                        instr.dr(), instr.sr1(), instr.sr2());
            }
            break;
        case NOT_INSTR:
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.sr1());
            break;
        case LD_INSTR:
        case ST_INSTR:
        case LEA_INSTR:
        case LDI_INSTR:
        case STI_INSTR:
            offset = instr.pc_offset9();
            label = lc3_sym_rev_lookup(state, pc + offset);
            if (label.empty())
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.dr(),
                        instr.pc_offset9());
            }
            else
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.dr(), label.c_str());
            }
            break;
        case LDR_INSTR:
        case STR_INSTR:
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.base_r(),
                    instr.offset6());
            break;
        case JSR_INSTR: // JSRR_INSTR
            if (instr.is_jsr())
            {
                offset = instr.pc_offset11();
                label = lc3_sym_rev_lookup(state, pc + offset);

                if (label.empty())
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.pc_offset11());
                else
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], label.c_str());
            }
            else
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.base_r());
            break;
        case JMP_INSTR: // RET_INSTR
            if (instr.base_r() == 0x7)
                sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][1]);
            else
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.base_r());
            break;
        case TRAP_INSTR:
            data = instr.vector();
            if (data >= TRAP_GETC && data <= TRAP_HALT)
                sprintf(buf, "%s", TRAP_CASES[data - TRAP_GETC]);
            else
            {
                // Plugin check time!
                if (state.trapPlugins.find(data) != state.trapPlugins.end())
                    return state.trapPlugins[data]->GetTrapName();
                else
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.vector());
            }
            break;
        case RTI_INSTR:
            sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        case ERROR_INSTR:
            if (state.instructionPlugin)
                return state.instructionPlugin->OnDisassemble(state, instr, LC3_NORMAL_DISASSEMBLE);
            else
                sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        default:
            sprintf(buf, "Unknown instruction x%04x", data);
    }

    return buf;
}

std::string lc3_smart_disassemble(lc3_state& state, uint16_t instruction, uint16_t pc)
{
    lc3_instruction instr(instruction);
    int32_t offset;
    uint32_t opcode = instr.opcode();
    int32_t data;
    std::string label;
    char buf[128];

    switch(opcode)
    {
        case BR_INSTR:
            offset = instr.pc_offset9();
            label = lc3_sym_rev_lookup(state, pc + offset);
            // If no flags are on or offset == 0 its a NOP
            if (!(instr.n() || instr.z() || instr.p()) || offset == 0)
            {
                if (!(instr.n() || instr.z() || instr.p())/* && offset <= 256*/)
                {
                    if (isprint(offset))
                    {
                        char minibuf[7];
                        snprintf(minibuf, 7, " ('%c')", offset);
                        sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], minibuf);
                    }
                    else
                        sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], "");
                }
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], "");

            }
            // If all flags are on
            else if (instr.n() && instr.z() && instr.p())
            {
                // If we know the label
                if (!label.empty())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NZP_LABEL], label.c_str());
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NZP_OFF], SIGN_CHR(offset), ABS(offset));
            }
            // Cases BRN,BRZ,BRP,BRNZ,BRNP,BRZP
            else
            {
                // Data is the NZP flags
                data = instr.cc();
                if (!label.empty())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NORM_LABEL], BR_CASES[data], label.c_str());
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NORM_OFF], BR_CASES[data],
                            SIGN_CHR(offset), ABS(offset));
            }
            break;
        case ADD_INSTR:
            if (instr.is_imm())
            {
                // Data is imm
                data = instr.imm5();
                // TEST = ADD RX, RX, 0
                if (data == 0 && instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_TEST], instr.dr());
                // SET
                else if (data == 0 && instr.dr() != instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_SET], instr.dr(),
                            instr.sr1());
                // INC = ADD RX, RX, 1
                else if (data == 1 && instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_INC], instr.dr());
                // DEC = ADD RX, RX, -1
                else if (data == -1 && instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_DEC], instr.dr());
                // ADD_EQ = ADD RX, RX, NUM
                else if (instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_EQ_VAL], instr.dr(),
                            SIGN_CHR(data), ABS(data));
                // NORMAL ADD IMM
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_REG_VAL], instr.dr(),
                            instr.sr1(), SIGN_CHR(data), ABS(data));
            }
            else
            {
                // DR is the data here
                data = instr.dr();

                // ADD EQ
                if (data == instr.sr1() || data == instr.sr2())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_EQ_REG], data,
                            OTHER(data, instr.sr1(), instr.sr2()));
                // NORMAL ADD
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_TWO_REGS], data,
                            instr.sr1(), instr.sr2());
            }
            break;
        case AND_INSTR:
            if (instr.is_imm())
            {
                data = instr.imm5();
                // ZERO OUT
                if (data == 0)
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_ZERO], instr.dr());
                // TEST REG
                else if (data == -1 && instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TEST], instr.dr());
                // SET REG
                else if (data == -1 && instr.dr() != instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_SET], instr.dr(),
                            instr.sr1());
                // AND EQUALS
                else if (instr.dr() == instr.sr1())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_NUM], instr.dr(), data);
                // NORMAL IMM ADD
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_REG_NUM], instr.dr(),
                            instr.sr1(), data);
            }
            else
            {
                // Data is DR
                data = instr.dr();

                if (data == instr.sr1() && data == instr.sr2())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TEST], data);
                else if (instr.sr1() == instr.sr2())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_SET], data, instr.sr1());
                else if (data == instr.sr1() || data == instr.sr2())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_EQ_REG], data,
                            OTHER(data, instr.sr1(), instr.sr2()));
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TWO_REGS], data,
                            instr.sr1(), instr.sr2());
            }
            break;
        case NOT_INSTR:
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.sr1());
            break;
        case LD_INSTR:
        case LEA_INSTR:
        case LDI_INSTR:
            offset = instr.pc_offset9();
            label = lc3_sym_rev_lookup(state, pc + offset);
            if (!label.empty())
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_LABEL], instr.dr(), label.c_str());
            else if (offset != 0)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET], instr.dr(), SIGN_CHR(offset),
                        ABS(offset));
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET_0], instr.dr());
            break;
        case ST_INSTR:
        case STI_INSTR:
            offset = instr.pc_offset9();
            label = lc3_sym_rev_lookup(state, pc + offset);
            if (!label.empty())
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_LABEL], label.c_str(), instr.dr());
            else if (offset != 0)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET], SIGN_CHR(offset), ABS(offset),
                        instr.dr());
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET_0], instr.dr());
            break;
        case LDR_INSTR:
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.dr(), instr.base_r(),
                    instr.offset6());
            break;
        case STR_INSTR:
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.base_r(), instr.offset6(),
                    instr.dr());
            break;
        case JSR_INSTR:
            if (instr.is_jsr())
            {
                offset = instr.pc_offset11();
                label = lc3_sym_rev_lookup(state, pc + offset);

                if (!label.empty())
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_LABEL], label.c_str());
                else if (offset != 0)
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_OFFSET], SIGN_CHR(offset), ABS(offset));
                else
                    sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][JSR_OFFSET_0]);
            }
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_JSRR], instr.base_r());
            break;
        case JMP_INSTR:
            if (instr.base_r() == 0x7)
                sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][JMP_R7]);
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JMP_REG], instr.base_r());
            break;
        case TRAP_INSTR:
            data = instr.vector();
            if (data >= TRAP_GETC && data <= TRAP_HALT)
                sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][data - TRAP_GETC]);
            else
            {
                // Plugin check time!
                if (state.trapPlugins.find(data) != state.trapPlugins.end())
                    return state.trapPlugins[data]->GetTrapName();
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][TRAP_OTHER], static_cast<uint8_t>(data));
            }
            break;
        case RTI_INSTR:
            sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        case ERROR_INSTR:
            if (state.instructionPlugin)
                return state.instructionPlugin->OnDisassemble(state, instr, LC3_ADVANCED_DISASSEMBLE);
            else
                sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][0]);
            break;
        default:
            sprintf(buf, "Unknown instruction");
    }

    return buf;
}

std::string lc3_disassemble(lc3_state& state, uint16_t data, int32_t pc, int32_t level)
{
    uint16_t address = (pc == -1) ? state.pc : static_cast<uint16_t>(pc);

    std::string instr;
    switch(level)
    {
        case 0:
            instr = lc3_basic_disassemble(state, data, address);
            break;
        case 1:
            instr = lc3_normal_disassemble(state, data, address);
            break;
        case 2:
            instr = lc3_smart_disassemble(state, data, address);
            break;
        default:
            instr = "";
            break;
    }

    if (state.strict_execution && lc3_check_malformed_instruction(lc3_instruction(data)))
        instr += " *";

    return instr;
}

bool lc3_check_malformed_instruction(const lc3_instruction& instruction)
{
    switch(instruction.opcode())
    {
        case BR_INSTR:
            // 0000 <!= 000> xxxxxxxxx
            return instruction.cc() == 0;
        case ADD_INSTR:
             [[fallthrough]];
        case AND_INSTR:
            // 0001 xxx xxx 0 <00> xxx
            // 0101 xxx xxx 0 <00> xxx
            return !instruction.is_imm() && instruction.get(3, 2) != 0;
        case JSRR_INSTR:
            // 0100 0 <00> xxx <000000>
            return !instruction.is_jsr() && (instruction.get(9, 2) != 0 || instruction.get(0, 6) != 0);
        case RTI_INSTR:
            // 1000 <000000000000>
            return instruction.get(0, 12) != 0;
        case NOT_INSTR:
            // 1001 xxx xxx <111111>
            return instruction.get(0, 6) != 0x3F;
        case JMP_INSTR:
            // 1100 <000> xxx <000000>
            return instruction.get(9, 3) != 0 || instruction.get(0, 6) != 0;
        case TRAP_INSTR:
            // 1111 <0000> xxxxxxxx
            return instruction.get(8, 4) != 0;
        case ERROR_INSTR:
            /// TODO implement a malformed instruction checker for plugins.
            return false;
        default:
            return false;
    }
    return false;
}

int32_t lc3_load(lc3_state& state, std::istream& file, int32_t (*reader)(std::istream&))
{
    if (!file.good()) return -1;

    int32_t read_data = reader(file);

    while (read_data >= 0)
    {
        // Get Address Start from Reader
        uint16_t addr_start = read_data;
        uint16_t addr_size = reader(file);

        for (uint16_t i = 0; i < addr_size; i++)
            // Put it in memory
            state.mem[addr_start + i] = static_cast<int16_t>(reader(file));

        read_data = reader(file);
    }

    return 0;
}

int32_t lc3_reader_hex(std::istream& file)
{
    std::string line;
    if (file.eof()) return -1;
    getline(file, line);

    std::stringstream ss(line);
    uint32_t result;
    if (!(ss >> std::hex >> result))
        return -1;

    return result;
}

int32_t lc3_reader_obj(std::istream& file)
{
    uint16_t data;
    file.read(reinterpret_cast<char*>(&data), sizeof(data));
    if (file.eof()) return -1;
    return ((data >> 8) & 0xFF) | ((data & 0xFF) << 8);
}

int32_t lc3_read_char(lc3_state& state, std::istream& file)
{
    int8_t c = file.get();
    if (!file.good())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    return c;
}

int32_t lc3_peek_char(lc3_state& state, std::istream& file)
{
    int8_t c = file.peek();
    if (!file.good())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    return c;
}

int32_t lc3_write_char(lc3_state& state, std::ostream& file, int32_t chr)
{
    if (chr > 255 || !(isgraph(chr) || isspace(chr) || chr == '\b'))
        lc3_warning(state, LC3_INVALID_CHARACTER_WRITE, chr);
    return state.writer(state, file, chr);
}

int32_t lc3_do_write_char(lc3_state&, std::ostream& file, int32_t chr)
{
    if (!file.good()) return -1;
    file.put(static_cast<int8_t>(chr));
    return (!file.good()) ? -1 : 0;
}

int32_t lc3_write_str(lc3_state& state, const std::function<int32_t(lc3_state&, std::ostream&, int32_t)>& writer, std::ostream& file, const std::string& str)
{
    for (const auto& c : str)
    {
        if (writer(state, file, c))
            return -1;
    }
    return 0;
}

void lc3_randomize(lc3_state& state)
{
    for (uint32_t i = 0; i <= 0xFFFF; i++)
        state.mem[i] = lc3_random(state);

    if (state.true_traps)
        lc3_load_os(state);
}

void lc3_trace(lc3_state& state)
{
    char buf[128];
    std::ostream& stream = *state.trace;

    snprintf(buf, 128, "PC x%04x\n", state.pc);
    stream << buf;

    snprintf(buf, 128, "instr: %s", lc3_disassemble(state, state.mem[state.pc], state.pc, 1).c_str());
    stream << buf;

    snprintf(buf, 128, " (%04x)\n", static_cast<uint16_t>(state.mem[state.pc]));
    stream << buf;

    snprintf(buf, 128, "R0 %6d|x%04x\tR1 %6d|x%04x\tR2 %6d|x%04x\tR3 %6d|x%04x\n",
             state.regs[0], static_cast<uint16_t>(state.regs[0]),
             state.regs[1], static_cast<uint16_t>(state.regs[1]),
             state.regs[2], static_cast<uint16_t>(state.regs[2]),
             state.regs[3], static_cast<uint16_t>(state.regs[3]));
    stream << buf;


    snprintf(buf, 128, "R4 %6d|x%04x\tR5 %6d|x%04x\tR6 %6d|x%04x\tR7 %6d|x%04x\n",
             state.regs[4], static_cast<uint16_t>(state.regs[4]),
             state.regs[5], static_cast<uint16_t>(state.regs[5]),
             state.regs[6], static_cast<uint16_t>(state.regs[6]),
             state.regs[7], static_cast<uint16_t>(state.regs[7]));
    stream << buf;

    snprintf(buf, 128, "CC: %s\n\n", (state.n ? "N" : (state.z ? "Z" : "P")));
    stream << buf;
}
