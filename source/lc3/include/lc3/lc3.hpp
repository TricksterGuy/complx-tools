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
#include <sstream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>

/** Sign Character */
#define SIGN_CHR(x) (((x) < 0) ? "-" : "+")
/** Sign Character only if negative */
#define SIGN_NEG_CHR(x) (((x) < 0) ? "-" : "")
/** Absolute value */
#define ABS(x) (((x) < 0) ? -(x) : (x))
/** Sign extension */
static inline short sext(int n, int size)
{
    constexpr unsigned short extend = 0xFFFFU;
    int b = size - 1;
    if (n & (1 << b))
        return n | (extend << b);
    else
        return n;
}

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
enum LC3_API TRAP_VECTORS
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

enum LC3_API INTERRUPT_VECTORS
{
    INTERRUPT_PRIVILEGE = 0x00,
    INTERRUPT_ILLEGAL_OPCODE = 0x01,
    INTERRUPT_ACCCESS_CONTROL_VIOLATION = 0x02,
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
    LC3_EXECUTE_TVT = 15,
    LC3_EXECUTE_IVT = 16,
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

/** LC3 event id type */
enum class lc3_event_id
{
    INVALID = 0,
    OUTPUT,
    OUTPUT_STRING,
    INPUT,
    INTERRUPT,
    EXIT_INTERRUPT,
    TRAP,
    EXIT_TRAP,
    SUBROUTINE,
    EXIT_SUBROUTINE,
    MEMORY_READ,
    MEMORY_WRITE,
    WARNING,
    BREAKPOINT,
    WATCHPOINT,
};

#define DEFAULT_KEYBOARD_INTERRUPT_DELAY 1000

class Plugin;
class InstructionPlugin;
class DeviceRegisterPlugin;
class TrapFunctionPlugin;
class PluginParams;
class lc3_state;

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

/** General instruction type for lc3 */
class lc3_instruction
{
    public:
        explicit lc3_instruction(uint16_t _data) { data = _data; }
        uint8_t opcode() const      { return  data >> 12 & 0xF; }
        uint16_t operands() const   { return  data       & 0xFFF; }

        uint8_t dr()  const         { return (data >> 9) & 0x7; }
        uint8_t sr1() const         { return (data >> 6) & 0x7; }
        uint8_t base_r() const      { return (data >> 6) & 0x7; }
        uint8_t sr2() const         { return  data       & 0x7; }
        bool is_imm() const         { return (data >> 5) & 0x1; }
        int8_t imm5() const         { return sext(data & 0x1F, 5); }

        int8_t offset6() const      { return sext(data & 0x3F, 6); };
        int16_t pc_offset9() const  { return sext(data & 0x1FF, 9); }

        bool is_jsr() const        { return (data >> 11) & 0x1; }
        int16_t pc_offset11() const { return sext(data & 0x7FF, 11); }

        // TRAP
        uint8_t vector() const    { return data & 0xFF; }

        bool n() const { return (data >> 11) & 0x1; }
        bool z() const { return (data >> 10) & 0x1; }
        bool p() const { return (data >>  9) & 0x1; }
        uint8_t cc() const {return (data >> 9) & 0x7; }

        uint16_t get(int i, int length) const { return (data >> i) & ((1 << length) - 1); }
        int16_t bits() const { return data; }

    private:
        uint16_t data;
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

struct LC3_API lc3_breakpoint_target
{
    uint16_t address;
    bool operator==(const lc3_breakpoint_target& other) const {return address == other.address;}
};

struct LC3_API lc3_watchpoint_target
{
    bool is_reg;
    uint16_t target;
    bool operator==(const lc3_watchpoint_target& other) const {return target == other.target && is_reg == other.is_reg;}
};

/** Record of stats for a breakpoint/watchpoint. */
struct LC3_API lc3_debug_info
{
    bool enabled;
    std::variant<std::monostate, lc3_breakpoint_target, lc3_watchpoint_target> target;
    int32_t max_hits;
    int32_t hit_count;
    std::string name;
    std::string condition;
    std::string message;
    bool is_breakpoint() const {return std::holds_alternative<lc3_breakpoint_target>(target);}
    bool is_watchpoint() const {return std::holds_alternative<lc3_watchpoint_target>(target);}
    std::string target_string() const
    {
        std::stringstream msg;
        if (is_breakpoint())
        {
            auto& breakpoint_info = std::get<lc3_breakpoint_target>(target);
            msg << "Breakpoint address: x" << std::hex << breakpoint_info.address;
        }
        else if (is_watchpoint())
        {
            auto& watchpoint_info = std::get<lc3_watchpoint_target>(target);
            msg << "Watchpoint target: ";
            if (watchpoint_info.is_reg)
                msg << "R" << watchpoint_info.target;
            else
                msg << "x" << std::hex << watchpoint_info.target;
        }
        else
        {
            msg << "Unknown debug type";
        }
        return msg.str();
    }
    bool operator==(const lc3_debug_info& other) const
    {
        return target == other.target && condition == other.condition;
    }
};

/** Record of subroutine information. */
struct LC3_API lc3_subroutine_info
{
    uint16_t address;
    std::string name;
    int32_t num_params;
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

using lc3_output_event          = std::function<void(lc3_state&, char)>;
using lc3_puts_event            = std::function<void(lc3_state&, const std::string&)>;
using lc3_input_event           = std::function<void(lc3_state&, uint8_t)>;
using lc3_interrupt_event       = std::function<void(lc3_state&, uint8_t)>;
using lc3_exit_interrupt_event  = std::function<void(lc3_state&, uint8_t)>;
using lc3_trap_event            = std::function<void(lc3_state&, uint8_t)>;
using lc3_exit_trap_event       = std::function<void(lc3_state&, uint8_t)>;
using lc3_subroutine_event      = std::function<void(lc3_state&, uint8_t)>;
using lc3_exit_subroutine_event = std::function<void(lc3_state&, uint8_t)>;
using lc3_memory_read_event     = std::function<void(lc3_state&, uint16_t)>;
using lc3_memory_write_event    = std::function<void(lc3_state&, uint16_t, int16_t)>;
using lc3_warning_event         = std::function<void(lc3_state&, int32_t)>;
using lc3_breakpoint_event      = std::function<void(lc3_state&, const lc3_debug_info&)>;
using lc3_watchpoint_event      = std::function<void(lc3_state&, const lc3_debug_info&)>;

//  LC3 event function type
using lc3_event_function = std::variant<
    std::monostate,
    lc3_output_event,
    lc3_puts_event,
    lc3_input_event,
    lc3_interrupt_event,
    lc3_exit_interrupt_event,
    lc3_trap_event,
    lc3_exit_trap_event,
    lc3_subroutine_event,
    lc3_exit_subroutine_event,
    lc3_memory_read_event,
    lc3_memory_write_event,
    lc3_warning_event,
    lc3_breakpoint_event,
    lc3_watchpoint_event
>;


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

    // Stream for debug messages
    std::ostream* debug = nullptr;

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
    // Event table
    std::unordered_map<lc3_event_id, std::list<lc3_event_function>> event_table;

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
    std::unordered_map<uint16_t, lc3_debug_info> breakpoints;
    std::unordered_map<uint16_t, lc3_debug_info> mem_watchpoints;
    std::unordered_map<uint8_t , lc3_debug_info> reg_watchpoints;
    std::unordered_map<uint16_t, std::string> comments;
    std::unordered_map<uint16_t, lc3_subroutine_info> subroutines;

    // Statistics
    std::unordered_map<uint16_t, lc3_memory_stats> memory_ops;
    uint64_t total_reads;
    uint64_t total_writes;

    // Trace logging
    std::ostream* trace = nullptr;

    // test_only mode
    // The only effect is that it records the first level subroutine/trap calls.
    bool in_lc3test;
};

/** lc3_init
  *
  * Initializes the state of the lc3.
  * @param state LC3State object.
  * @param randomize_registers if true randomizes registers.
  * @param randomize_memory if true randomizes memory.
  * @param register_fill_value ignored if randomize_registers is true otherwise sets registers to this value.
  * @param memory_fill_value ignored if randomize_memory is true otherwise sets memory to this value (except for TVT, IVT and lc3 os code).
  */
void LC3_API lc3_init(lc3_state& state, bool randomize_registers = true, bool randomize_memory = true, int16_t register_fill_value = 0, int16_t memory_fill_value = 0);
/** lc3_set_version
  * Sets the lc3's version should be done after lc3_init.
  * This function will overwrite the LC3OS code with the proper OS for that version.
  * @param version. LC-3 Version. Valid values are 0 for original LC-3 and 1 for 2019's revision.
  */
void LC3_API lc3_set_version(lc3_state& state, int version);
/** lc3_remove_plugins
  *
  * Removes all installed lc3 plugins.
  * @param state LC3State object.
  */
void LC3_API lc3_remove_plugins(lc3_state& state);
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

/** lc3_check_malformed_instruction
  *
  * Checks if the instruction given is malformed.
  * A malformed instruction is an instruction that doesn't follow its bit layout from the ISA.
  *
  * @param instruction lc3_instruction object.
  * @return True if the instruction is malformed.
  */
bool lc3_check_malformed_instruction(const lc3_instruction& instruction);

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
/** Get the value of the PSR */
inline uint16_t lc3_psr(lc3_state& state) { return (state.privilege << 15) | (state.priority << 8) | (state.n << 2) | (state.z << 1) | state.p; }
/** lc3_randomize
  *
  * Randomizes LC3 Memory
  * @param state LC3State object.
  */
void lc3_randomize(lc3_state& state);
/** lc3_trace
  *
  * Prints current lc3 state.
  * @param state LC3State object.
  */
void lc3_trace(lc3_state& state);

#endif
