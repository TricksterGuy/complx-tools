#include "multiply.hpp"

#include <memory>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& /*params*/)
{
    if (!instance)
        instance = std::make_unique<MultiplyPlugin>();

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

std::string MultiplyPlugin::GetOpcode() const
{
    return "MUL";
}

uint16_t MultiplyPlugin::DoAssembleOne(lc3_state& /*state*/, LC3AssembleContext& context)
{
    size_t pos = context.line.find_first_of(" \t");

    std::string opcode = context.line.substr(0, pos);
    trim(opcode);

    std::string line = (pos == std::string::npos) ? "" : context.line.c_str() + pos + 1;
    trim(line);

    tokenize(context.line, context.tokens, " \t,");

    std::vector<std::string> tokens;
    tokenize(line, tokens, ",");

    // Remove spaces
    for (auto& token : tokens)
        trim(token);

    if (tokens.size() < 3)
    {
        THROWANDDO(LC3AssembleException(context.line, "", SYNTAX_ERROR, context.lineno), return 0);
    }
    else if (tokens.size() > 3)
    {
        THROW(LC3AssembleException(context.line, "", EXTRA_INPUT, context.lineno));
    }

    bool is_reg;
    int dr, sr1, sr2_imm;
    uint16_t instruction = 0xD000;

    dr = get_register(tokens[0], context);
    sr1 = get_register(tokens[1], context);
    sr2_imm = get_register_imm5(tokens[2], is_reg, context);
    instruction |= (dr << 9) | (sr1 << 6) | (!is_reg << 5) | sr2_imm;

    return instruction;
}

void MultiplyPlugin::OnExecute(lc3_state& state, const lc3_instruction& instruction, lc3_state_change& changes)
{
    // Hey DR will change here save it
    changes.changes = LC3_REGISTER_CHANGE;
    changes.location = instruction.dr();
    changes.value = state.regs[changes.location];

    // Two modes immediate value and registers
    if (instruction.is_imm())
        state.regs[changes.location] = state.regs[instruction.sr1()] * instruction.imm5();
    else
        state.regs[changes.location] = state.regs[instruction.sr1()] * state.regs[instruction.sr2()];

    // Update NZP
    lc3_setcc(state, state.regs[changes.location]);
}

static const char* const MUL_DISASSEMBLE_LOOKUP[2] =
{
    "MUL R%d, R%d, R%d",
    "MUL R%d, R%d, #%d"
};

static const char* const MUL_ADVANCED_DISASSEMBLE[7] =
{
    "TEST R%d",         // MUL R0, R0, 1
    "R%d = R%d",        // MUL R0, R1, 1
    "R%d = 0",          // MUL R0, R0, 0
    "R%d *= %s%d",      // MUL R0, R0, 3
    "R%d = R%d * %s%d", // MUL R0, R1, 3
    "R%d *= R%d",       // MUL R0, R0, R1
    "R%d = R%d * R%d"   // MUL R0, R1, R2
};


std::string MultiplyPlugin::OnDisassemble(lc3_state& /*state*/, const lc3_instruction& instr, unsigned int level)
{
    char buf[128];
    int16_t data;

    // No difference between basic and normal
    if (level == LC3_BASIC_DISASSEMBLE || level == LC3_NORMAL_DISASSEMBLE)
    {
        if (instr.is_imm())
        {
            sprintf(buf, MUL_DISASSEMBLE_LOOKUP[1], instr.dr(), instr.sr1(), instr.imm5());
        }
        else
        {
            sprintf(buf, MUL_DISASSEMBLE_LOOKUP[0], instr.dr(), instr.sr1(), instr.sr2());
        }
    }
    else if (level == LC3_ADVANCED_DISASSEMBLE)
    {
        if (instr.is_imm())
        {
            // Data is imm
            data = instr.imm5();
            // TEST = MUL RX, RX, 1
            if (data == 1 && instr.dr() == instr.sr1())
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_TEST], instr.dr());
            // SET = MUL RX, RY, 1
            else if (data == 1 && instr.dr() != instr.sr1())
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_SET], instr.dr(), instr.sr1());
            // CLEAR = MUL RX, RANY, 0
            else if (data == 0)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_ZERO], instr.dr());
            // MUL_EQ = MUL RX, RX, NUM
            else if (instr.dr() == instr.sr1())
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_REG_NUM], instr.dr(), SIGN_NEG_CHR(data), ABS(data));
            // NORMAL MUL IMM
            else
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_NUM], instr.dr(), instr.sr1(), SIGN_NEG_CHR(data), ABS(data));
        }
        else
        {
            // DR is the data here
            data = instr.dr();

            // MUL EQ
            if (data == instr.sr1() || data == instr.sr2())
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_EQ_REG], data, OTHER(data, instr.sr1(), instr.sr2()));
            // NORMAL MUL
            else
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_TWO_REGS], data, instr.sr1(), instr.sr2());
        }
    }
    return buf;
}

static const RLEColorEntry mulColorings[2][4] =
{
    {{"DR_COLOR", 96, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}, {"UNUSED_BITS_COLOR", 0, 0, 0, 1}, {"IMM_COLOR", 0, 0, 80, 5}}, // IMM Version
    {{"DR_COLOR", 96, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}, {"UNUSED_BITS_COLOR", 0, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}},  // REG Version
};

std::list<RLEColorEntry> MultiplyPlugin::GetInstructionColoring(uint16_t instr) const
{
    // Just like the ADD instruction
    std::list<RLEColorEntry> answer;

    if (instr >> 5 & 0x1)
        answer.assign(mulColorings[0], mulColorings[0] + 4);
    else
        answer.assign(mulColorings[1], mulColorings[1] + 4);

    return answer;
}


