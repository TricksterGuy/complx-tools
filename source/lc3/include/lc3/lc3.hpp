#ifndef LC3_HPP
#define LC3_HPP

#include <lc3/lc3_api.h>

#include <cstdint>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <functional>
#include <list>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <unordered_map>

/** Sign Character */
#define SIGN_CHR(x) (((x) < 0) ? "-" : "+")
/** Sign Character only if negative */
#define SIGN_NEG_CHR(x) (((x) < 0) ? "-" : "")
/** Absolute value */
#define ABS(x) (((x) < 0) ? -(x) : (x))

/**
  * Given 3 values when 2 of them are equal
  * Returns the not equal one
  * That is given 3, 7, 3 this will return 7
  */
#define OTHER(x, y, z) ((x) ^ (y) ^ (z))

/** Instruction opcodes */
enum LC3_API lc3_instruction_t
{
    BR_INSTR    = 0,
    ADD_INSTR   = 1,
    LD_INSTR    = 2,
    ST_INSTR    = 3,
    JSR_INSTR   = 4,
    JSRR_INSTR  = 4,
    AND_INSTR   = 5,
    LDR_INSTR   = 6,
    STR_INSTR   = 7,
    RTI_INSTR   = 8,
    NOT_INSTR   = 9,
    LDI_INSTR   = 10,
    STI_INSTR   = 11,
    RET_INSTR   = 12,
    JMP_INSTR   = 12,
    ERROR_INSTR = 13,
    LEA_INSTR   = 14,
    TRAP_INSTR  = 15
};

/** Predefined traps provided by LC3 OS */
enum LC3_API TRAPS
{
    TRAP_GETC  = 0x20,
    TRAP_OUT   = 0x21,
    TRAP_PUTS  = 0x22,
    TRAP_IN    = 0x23,
    TRAP_PUTSP = 0x24,
    TRAP_HALT  = 0x25
};

/** Predefined Device Registers */
enum LC3_API DEVICES
{
    DEV_KBSR = 0xFE00,
    DEV_KBDR = 0xFE02,
    DEV_DSR  = 0xFE04,
    DEV_DDR  = 0xFE06,
    // LC3 revision 2019.
    DEV_PSR = 0xFFFC,
    DEV_MCR  = 0xFFFE
};

/** Runtime warning types */
enum LC3_API WARNINGS
{
    LC3_OUT_OF_INPUT = 0,
    LC3_RESERVED_MEM_WRITE = 1,
    LC3_RESERVED_MEM_READ = 2,
    LC3_UNSUPPORTED_TRAP = 3,
    LC3_UNSUPPORTED_INSTRUCTION = 4,
    LC3_MALFORMED_INSTRUCTION = 5,
    LC3_USER_RTI = 6,
    LC3_INVALID_CHARACTER_WRITE = 7,
    LC3_PUTS_INVALID_MEMORY = 8,
    LC3_DISPLAY_NOT_READY = 9,
    LC3_KEYBOARD_NOT_READY = 10,
    LC3_TURN_OFF_VIA_MCR = 11,
    LC3_PUTSP_INVALID_MEMORY = 12,
    LC3_PUTSP_UNEXPECTED_NUL = 13,
    LC3_INVALID_PSR_VALUE = 14,
    LC3_WARNINGS               // Must be last.
};

/** Cases for Smart Disassembling */
enum DISASSEMBLE_CASES
{

    BR_NOP = 0,
    BR_NORM_LABEL = 1,
    BR_NZP_LABEL = 2,
    BR_NORM_OFF = 3,
    BR_NZP_OFF = 4,

    BR_N = 1,
    BR_Z = 2,
    BR_P = 4,
    BR_NZ = 3,
    BR_NP = 5,
    BR_ZP = 6,

    ADD_EQ_REG = 0,
    ADD_TWO_REGS = 1,
    ADD_TEST = 2,
    ADD_SET = 3,
    ADD_INC = 4,
    ADD_DEC = 5,
    ADD_EQ_VAL = 6,
    ADD_REG_VAL = 7,

    // Used for LD,LDI,ST,STI,LEA
    MEM_LABEL = 0,
    MEM_OFFSET = 1,
    MEM_OFFSET_0 = 2,

    JSR_LABEL = 0,
    JSR_OFFSET = 1,
    JSR_OFFSET_0 = 3,
    JSR_JSRR = 2,

    AND_TEST = 0,
    AND_SET = 1,
    AND_EQ_REG = 2,
    AND_TWO_REGS = 3,
    AND_ZERO = 4,
    AND_NUM = 5,
    AND_REG_NUM = 6,

    LDR_STR = 0,

    JMP_REG = 0,
    JMP_R7 = 1,

    TRAP_OTHER = 6
};

/** Disassemble levels */
enum LC3_API DISASSEMBLE_LEVELS
{
    LC3_BASIC_DISASSEMBLE,
    LC3_NORMAL_DISASSEMBLE,
    LC3_ADVANCED_DISASSEMBLE
};

#define DEFAULT_KEYBOARD_INTERRUPT_DELAY 1000

class Plugin;
class InstructionPlugin;
class DeviceRegisterPlugin;
class TrapFunctionPlugin;
class PluginParams;

using PluginCreateFunc = std::function<Plugin*(const PluginParams&)>;
using PluginDestroyFunc = std::function<void(Plugin*)>;

struct PluginInfo
{
    std::string filename;
    PluginCreateFunc create;
    PluginDestroyFunc destroy;
    void* handle;
    Plugin* plugin;
};

/** Arithmetic instruction type with registers. */
struct arithreg_instruction
{
    uint16_t opcode:4;
    uint16_t dr:3;
    uint16_t sr1:3;
    uint16_t is_imm:1;
    uint16_t unused:2;
    uint16_t sr2:3;
};

/** Arithmetic instruction type with immediate value. */
struct arithimm_instruction
{
    uint16_t opcode:4;
    uint16_t dr:3;
    uint16_t sr1:3;
    uint16_t is_imm:1;
    int16_t imm:5;
};

/** Not instruction type. */
struct not_instruction
{
    uint16_t opcode:4;
    uint16_t dr:3;
    uint16_t sr1:3;
    uint16_t unused:6;
};

/** Union for Arithmetic instruction type. */
union arith_instruction
{
    arithimm_instruction imm;
    arithreg_instruction reg;
    not_instruction inv;
};

/** Branch instruction type. */
struct br_instruction
{
    uint16_t opcode:4;
    uint16_t n:1;
    uint16_t z:1;
    uint16_t p:1;
    int16_t pc_offset:9;
};

/** Jump instruction type. */
struct jmp_instruction
{
    uint16_t opcode:4;
    uint16_t unused_3:3;
    uint16_t base_r:3;
    uint16_t unused_6:6;
};

/** Jump to subroutine type. */
struct jsr_instruction
{
    uint16_t opcode:4;
    uint16_t is_jsr:1;
    int16_t pc_offset:11;
};

/** Jump to subroutine within register type. */
struct jsrr_instruction
{
    uint16_t opcode:4;
    uint16_t is_jsr:1;
    uint16_t unused_2:2;
    uint16_t base_r:3;
    uint16_t unused_6:6;
};

/** Type for subroutine instruction. */
union subr_instruction
{
    jsr_instruction jsr;
    jsrr_instruction jsrr;
};

/** Type for memory instructions with offset. */
struct memoryoffset_instruction
{
    uint16_t opcode:4;
    uint16_t reg:3;
    int16_t pc_offset:9;
};

/** Type for memory instructions with base register and offset. */
struct memoryreg_instruction
{
    uint16_t opcode:4;
    uint16_t reg:3;
    uint16_t base_r:3;
    int16_t offset:6;
};

/** Type for all memory instructions */
union memory_instruction
{
    memoryoffset_instruction offset;
    memoryreg_instruction reg;
};

/** Type for TRAPs */
struct trap_instruction
{
    uint16_t opcode:4;
    uint16_t unused:4;
    uint16_t vector:8;
};

/** General instruction type for lc3 */
struct instruction_t
{
    uint16_t opcode:4;
    uint16_t data:12;
};

/**
  * Lc3 instruction type
  * contains the data for the different types of instructions
  * Note: the bits field != the instruction as data
  * The bits field gets it as how the system is representing
  * it in memory due to endianness not the actual LC3 view of it.
  * That is TRAP x25 = 0xF025 but could appear in memory as 0x250F
  */
/// TODO Consider getting rid of this setup.
union LC3_API lc3_instr
{
    arith_instruction  arith;
    br_instruction     br;
    jmp_instruction    jmp;
    subr_instruction   subr;
    memory_instruction mem;
    trap_instruction   trap;
    instruction_t      rti;
    instruction_t      data;
    uint16_t           bits;
};

/** Enumeration of possible things that can change as part of instruction execution. */
enum LC3_API lc3_change_t
{
    LC3_NO_CHANGE = 0,
    LC3_REGISTER_CHANGE = 1,
    LC3_MEMORY_CHANGE = 2,
    LC3_MULTI_CHANGE = 3,       // Multiple registers / memory addresses or both have changed.  For plugins.
    LC3_SUBROUTINE_BEGIN = 4,
    LC3_SUBROUTINE_END = 5,
    LC3_INTERRUPT_BEGIN = 6,    // Signals begin of interrupt can't backstep past this. (this will be in the undo stack while the interrupt is handled)
    LC3_INTERRUPT_END = 7,      // Signals end of interrupt (or end of trap in lc-3 revision) (this will never be in the undo stack)
    LC3_INTERRUPT = 8,          // Signals a processed interrupt. (LC3_INTERRUPT_BEGIN changes to this after its processed.
};

/** Caches the last value for a register/memory address as part of execution. */
struct LC3_API lc3_change_info
{
    bool is_reg;
    uint16_t location;
    uint16_t value;
};

/** Record of stats for a breakpoint. */
struct LC3_API lc3_breakpoint_info
{
    bool enabled;
    uint16_t addr;
    int32_t max_hits;
    int32_t hit_count;
    std::string label;
    std::string condition;
    bool operator==(const lc3_breakpoint_info& other) const
    {
        return addr == other.addr && condition == other.condition;
    }
};

/** Record of stats for a watchpoint. */
struct LC3_API lc3_watchpoint_info
{
    bool enabled;
    bool is_reg;
    uint16_t data;
    int32_t max_hits;
    int32_t hit_count;
    std::string label;
    std::string condition;
    bool operator==(const lc3_watchpoint_info& other) const
    {
        return is_reg == other.is_reg && data == other.data && condition == other.condition;
    }
};

/** Record of stats for a blackbox. */
struct LC3_API lc3_blackbox_info
{
    bool enabled;
    uint16_t addr;
    int32_t hit_count;
    std::string label;
    std::string condition;
    bool operator==(const lc3_blackbox_info& other) const
    {
        return addr == other.addr && condition == other.condition;
    }
};

/** Record of subroutine information. */
struct LC3_API lc3_subroutine_info
{
    uint16_t address;
    std::string name;
    int32_t num_params;
    // Will be empty if not used.
    std::vector<std::string> params;
};

/** Record of subroutine calls made in the program. */
struct LC3_API lc3_subroutine_call
{
    uint16_t address;
    uint16_t r6;
    bool is_trap;
};

/** Record of active subroutine call info for each call made */
struct LC3_API lc3_subroutine_call_info
{
	lc3_subroutine_call_info() : regs(8) {}
    uint16_t address = 0;
    uint16_t r6 = 0;
    std::vector<uint16_t> params;
    // This should be std::array<int16_t, 8> but due to a bug with py++ it doesn't work.
    std::vector<int16_t> regs;
    // For availability in pylc3 equality operator must be defined.
    bool operator==(const lc3_subroutine_call_info& other) const
    {
        return address == other.address && r6 == other.r6 && params == other.params && regs == other.regs;
    }

};

/** Record of a mocked subroutine call with return information */
struct LC3_API lc3_mock_subroutine_call_info
{
    uint16_t address;
    std::vector<uint16_t> params;
    int16_t answer;
};

/** Record of active trap call info for each trap called */
struct LC3_API lc3_trap_call_info
{
    lc3_trap_call_info() : regs(8) {}
    uint8_t vector = 0;
    // This should be std::array<int16_t, 8> but due to a bug with py++ it doesn't work.
    std::vector<int16_t> regs;
    // For availability in pylc3.
    bool operator==(const lc3_trap_call_info& other) const
    {
        return vector == other.vector && regs == other.regs;
    }
};

/** Record of stats per memory address */
struct LC3_API lc3_memory_stats
{
    uint64_t reads = 0;
    uint64_t writes = 0;
};

/** Record of active interrupt requests */
struct LC3_API lc3_interrupt_req
{
    uint8_t priority:3;
    uint8_t vector;
};

/** Changed information with previous value for undo stack */
struct LC3_API lc3_state_change
{
    uint16_t pc;
    int16_t r7; /* In the case of fake traps two registers can be modified. So we have a special place for r7*/
    uint8_t privilege:1;
    uint8_t n:1;
    uint8_t z:1;
    uint8_t p:1;
    uint8_t halted:1;
    uint8_t changes:4;
    uint16_t location;
    uint16_t value;
    uint16_t savedusp;
    uint16_t savedssp;
    uint32_t warnings;
    uint32_t executions;            // Only used for changes = LC3_INTERRUPT(_BEGIN) otherwise we know its changed by 1.
    lc3_subroutine_call subroutine;     // Only used for changes = LC3_SUBROUTINE_*
    std::vector<lc3_change_info> info;  // Only used for changes = LC3_MULTI_CHANGE
};

// Stack of RTI-able items for lc-3 revision.
typedef struct lc3_rti_stack_item
{
    bool is_interrupt;
} lc3_rti_stack_item;

/** Main type for a running lc3 machine */
struct LC3_API lc3_state
{
    int16_t regs[8];
    uint16_t pc;
    uint8_t privilege:1;
    uint8_t priority:3;
    uint8_t n:1;
    uint8_t z:1;
    uint8_t p:1;
    uint8_t halted:1;
    uint8_t true_traps:1;
    uint8_t interrupt_enabled:1;
    uint8_t strict_execution:1;
    int32_t lc3_version;
    uint32_t warnings;
    uint32_t executions;

    std::unordered_map<std::string, uint16_t> symbols;
    std::unordered_map<uint16_t, std::string> rev_symbols;

    int16_t mem[65536];

    // Stream for input
    std::istream* input;
    // Function to read and consume one character from stream
    std::function<int32_t(lc3_state&, std::istream&)> reader;
    // Function to peek at next character in stream
    std::function<int32_t(lc3_state&, std::istream&)> peek;

    // Stream for output
    std::ostream* output;
    // Function to write one character to stream
    std::function<int32_t(lc3_state&, std::ostream&, int32_t)> writer;

    // Stream for warnings
    std::ostream* warning;

    // Plugins loaded into the state
    InstructionPlugin* instructionPlugin;
    std::unordered_map<uint8_t, TrapFunctionPlugin*> trapPlugins;
    std::unordered_map<uint16_t, Plugin*> address_plugins;
    std::vector<Plugin*> plugins;

    // Plugin handle information
    std::unordered_map<std::string, PluginInfo> filePlugin;

    // Plugin interrupt information
    std::unordered_map<uint8_t, Plugin*> interruptPlugin;

    // Maximum undo stack size just here for people who like to infinite loop/recurse and don't want their computers to explode.
    uint32_t max_stack_size;
    std::deque<lc3_state_change> undo_stack;

    // Maximum call stack size just here for people who like to infinite loop/recurse and don't want their computers to explode.
    uint32_t max_call_stack_size;
    // Subroutine debugging info (again see note above)
    std::deque<lc3_subroutine_call> call_stack;
    // RTI stack to find out if the current RTI instruction targets a trap or an interrupt.
    std::deque<lc3_rti_stack_item> rti_stack;
    // First layer of calls for testing student code. (In case of multi recursion).
    std::vector<lc3_subroutine_call_info> first_level_calls;
    // First layer of trap calls (In case of multi recursion).
    std::vector<lc3_trap_call_info> first_level_traps;

    // Random number generator
    std::mt19937 rng;
    std::uniform_int_distribution<uint16_t> dist;
    uint32_t default_seed = 0;

    // Warn limit map
    std::unordered_map<int32_t, uint32_t> warn_stats;
    std::unordered_map<int32_t, uint32_t> warn_limits;

    // Interrupt support push things here to cause interrupt
    std::list<lc3_interrupt_req> interrupts;
    std::list<std::function<void(lc3_state&)>> interrupt_test; // Functions to be called at the end of each step.
    int32_t interrupt_vector; // Current interrupt vector that is being handled
    std::deque<int32_t> interrupt_vector_stack; // Current stack of interrupts being handled.
    uint16_t savedusp;
    uint16_t savedssp;
    uint32_t keyboard_int_delay; // Delay in instructions between keyboard interrupts
    uint32_t keyboard_int_counter; // Counter for the above delay.

    // Debugging information.
    std::unordered_map<uint16_t, lc3_breakpoint_info> breakpoints;
    std::unordered_map<uint16_t, lc3_blackbox_info> blackboxes;
    std::unordered_map<uint16_t, lc3_watchpoint_info> mem_watchpoints;
    std::unordered_map<uint16_t, lc3_watchpoint_info> reg_watchpoints;
    std::unordered_map<uint16_t, std::string> comments;
    std::unordered_map<uint16_t, lc3_subroutine_info> subroutines;

    // Statistics
    std::unordered_map<uint16_t, lc3_memory_stats> memory_ops;
    uint64_t total_reads;
    uint64_t total_writes;

    // Trace logging
    std::ostream* trace;

    // test_only mode
    // The only effect is that it records the first level subroutine/trap calls.
    bool in_lc3test;
};

/** lc3_basic_disassemble
  *
  * Disassemble the data passed in.
  * @param state LC3State object.
  * @param data Instruction data.
  * @return The disassembled instruction as a string.
  */
std::string lc3_basic_disassemble(lc3_state& state, uint16_t data);
/** lc3_disassemble
  *
  * Disassemble the data passed in with label information
  * This utilizes symbol table information in its output.
  * @param state LC3State object.
  * @param data Instruction data.
  * @return The disassembled instruction as a string.
  */
std::string lc3_normal_disassemble(lc3_state& state, uint16_t data);
/** lc3_smart_disassemble
  *
  * Disassembles the instruction into something a little more high level.
  * This utilizes symbol table information in its output.
  * @param state LC3State object.
  * @param data Instruction data.
  * @return The disassembled instruction as a string.
  */
std::string lc3_smart_disassemble(lc3_state& state, uint16_t data);

/** lc3_disassemble
  *
  * Entry function for disassembling instructions.
  * If state.strict_execution is enabled will append a * at the end of the instruction.
  *
  * @param state LC3State object.
  * @param data Instruction data.
  * @param pc PC value to interpret the instruction for symbols. Default is current PC value in the LC3State.
  * @param level disassemle level (0: basic, 1: normal, 2:high level). Default is normal.
  * @return The disassembled instruction as a string.
  */
std::string LC3_API lc3_disassemble(lc3_state& state, uint16_t data, int32_t pc = -1, int32_t level = LC3_NORMAL_DISASSEMBLE);

/** lc3_load
  *
  * Loads the given file into the machine.
  * @param state LC3State object.
  * @param file Input stream to read from.
  * @param reader is a function pointer to read in words into the machine.
  * @return Zero on success, nonzero for failure.
  */
int32_t lc3_load(lc3_state& state, std::istream& file, int32_t (*reader)(std::istream&));
/** lc3_reader_obj
  *
  * Reads in a file from a .obj file (output from as2obj).
  * @param file Input stream to read the obj file from.
  * @return Zero on success, nonzero for failure.
  */
int32_t lc3_reader_obj(std::istream& file);
/** lc3_reader_obj
  *
  * Reads in a file from a .hex file (instructions as hexadecimal).
  * @param file Input stream to read the hex file from.
  * @return Zero on success, nonzero for failure.
  */
int32_t lc3_reader_hex(std::istream& file);

/** lc3_read_char
  *
  * Reads in a character from the input stream passed in and consumes it.
  * @param state LC3State object.
  * @param file Stream to read from.
  * @return -1 if there is nothing in the input stream otherwise the character.
  */
int32_t lc3_read_char(lc3_state& state, std::istream& file);
/** lc3_peek_char
  *
  * Gets (Peek) next character from the input stream passed in without consuming it.
  * @param state LC3State object.
  * @param file Stream to read from.
  * @return -1 if there is nothing in the input stream otherwise the character.
  */
int32_t lc3_peek_char(lc3_state& state, std::istream& file);
/** lc3_write_char
  *
  * Checks and warns if a non printable character is printed, then prints it.
  * @param state LC3State object.
  * @param file Stream to write to.
  * @param chr Character to write.
  * @return Zero on success nonzero on failure.
  */
int32_t lc3_write_char(lc3_state& state, std::ostream& file, int32_t);
/** lc3_do_write_char
  *
  * Lc3Writer function that writes a character to the output.
  * @param state LC3State object.
  * @param file Stream to write to.
  * @param chr Character to write.
  * @return Zero on success nonzero on failure.
  */
int32_t lc3_do_write_char(lc3_state& state, std::ostream& file, int32_t chr);
/** lc3_write_str
  *
  * Writes a string to the output
  * @param state LC3State object.
  * @param writer Writer function
  * @param file Stream to write to.
  * @param str String to write.
  * @return Zero on success nonzero on failure.
  */
int32_t lc3_write_str(lc3_state& state, const std::function<int32_t(lc3_state&, std::ostream&, int32_t)>& writer, std::ostream& file, const std::string& str);

/** lc3_set_true_traps
  *
  * Enables or disables true traps, by default true traps is enabled.
  * The true traps setting enables actually running the code for the traps instead of emulating them in C.
  * @param state LC3State object.
  * @param value If true enable true traps, otherwise disable.
  */
inline void lc3_set_true_traps(lc3_state& state, bool value) {state.true_traps = value;}
/** Generate a random number LC-3 */
inline uint16_t lc3_random(lc3_state& state) { return state.dist(state.rng); }
/** lc3_randomize
  *
  * Randomizes LC3 Memory
  * @param state LC3State object.
  */
void lc3_randomize(lc3_state& state);

#endif
