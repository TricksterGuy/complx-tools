#define BOOST_TEST_MODULE LC3_Test_Suite
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <istream>
#include <fstream>
#include <vector>
#include <lc3.hpp>
#include <lc3/ExpressionEvaluator.hpp>

struct LC3BasicTest
{
    lc3_state state;
    std::stringstream warnings;

    LC3BasicTest()
    {
        lc3_init(state, false, false);
        state.warning = &warnings;
        state.lc3_version = 0;
    }
};


unsigned char allinstrs[] = {
  0x30, 0x00, 0x00, 0x12, 0x0e, 0x01, 0x10, 0x42, 0x10, 0x62, 0x21, 0xff,
  0x31, 0xff, 0x4f, 0xff, 0x40, 0x00, 0x50, 0x42, 0x50, 0x62, 0x60, 0x42,
  0x70, 0x42, 0x90, 0x7f, 0xa1, 0xff, 0xb1, 0xff, 0xc1, 0xc0, 0xc0, 0x00,
  0xe1, 0xff, 0xf0, 0x25
};
unsigned int allinstrs_len = 40;

unsigned char disassemble[] = {
  0x30, 0x00, 0x00, 0x3a, 0x50, 0x00, 0x50, 0x41, 0x50, 0x01, 0x50, 0x42,
  0x50, 0x3f, 0x50, 0x7f, 0x51, 0xe0, 0x50, 0x27, 0x50, 0x67, 0x10, 0x01,
  0x10, 0x42, 0x10, 0x20, 0x10, 0x60, 0x10, 0x21, 0x10, 0x3f, 0x10, 0x27,
  0x10, 0x67, 0x90, 0x3f, 0x90, 0x7f, 0x0e, 0x00, 0x00, 0x30, 0x00, 0x06,
  0x08, 0x22, 0x04, 0x21, 0x02, 0x20, 0x0c, 0x1f, 0x0a, 0x1e, 0x06, 0x1d,
  0x0e, 0x1c, 0x0e, 0x1b, 0x0e, 0x01, 0x07, 0xfa, 0x48, 0x18, 0x48, 0x01,
  0x40, 0x40, 0xc1, 0xc0, 0xc0, 0x00, 0x20, 0x13, 0x30, 0x12, 0x60, 0x40,
  0x60, 0x47, 0x70, 0x40, 0x70, 0x47, 0xa0, 0x0d, 0xb0, 0x0c, 0xb1, 0xfc,
  0xe0, 0x0a, 0xe0, 0x01, 0xf0, 0x20, 0xf0, 0x21, 0xf0, 0x22, 0xf0, 0x23,
  0xf0, 0x24, 0xf0, 0x25, 0xf0, 0xff, 0x80, 0x00, 0xd3, 0x92, 0x23, 0x28
};
unsigned int disassemble_len = 120;

const std::string disassemble_sym = "LABEL\t3039";

unsigned char simple[] = {
  0x30, 0x00, 0x00, 0x06, 0x20, 0x03, 0x22, 0x03, 0x14, 0x01, 0xf0, 0x25,
  0x00, 0x3c, 0x00, 0x06
};
unsigned int simple_len = 16;

const std::string simple_hex = R"(3000
0006
2003
2203
1401
F025
003C
0006)";

const unsigned char simplesubr[] = {
    0x30, 0x00, 0x00, 0x09, 0x48, 0x02, 0x00, 0x00, 0xf0, 0x25, 0x20, 0x03,
    0x22, 0x03, 0x14, 0x01, 0xc1, 0xc0, 0x00, 0x3c, 0x00, 0x06
};
const unsigned int simplesubr_len = 22;



BOOST_FIXTURE_TEST_CASE(InstructionDecodeTest2, LC3BasicTest)
{
    lc3_instruction instr(0x1008);
    BOOST_REQUIRE_EQUAL(instr.opcode(), ADD_INSTR);
    BOOST_REQUIRE(!instr.is_imm());
    BOOST_CHECK_EQUAL(instr.dr(), 0);
    BOOST_CHECK_EQUAL(instr.sr1(), 0);
    BOOST_CHECK_EQUAL(instr.sr2(), 0);

    instr = lc3_instruction(0);
    BOOST_REQUIRE_EQUAL(instr.opcode(), BR_INSTR);
    BOOST_CHECK_EQUAL(instr.n(), 0);
    BOOST_CHECK_EQUAL(instr.z(), 0);
    BOOST_CHECK_EQUAL(instr.p(), 0);
    BOOST_CHECK_EQUAL(instr.pc_offset9(), 0);

    instr = lc3_instruction(1);
    BOOST_REQUIRE_EQUAL(instr.opcode(), BR_INSTR);
    BOOST_CHECK_EQUAL(instr.n(), 0);
    BOOST_CHECK_EQUAL(instr.z(), 0);
    BOOST_CHECK_EQUAL(instr.p(), 0);
    BOOST_CHECK_EQUAL(instr.pc_offset9(), 1);

    instr = lc3_instruction(0xC200);
    BOOST_REQUIRE_EQUAL(instr.opcode(), JMP_INSTR);
    BOOST_CHECK_EQUAL(instr.base_r(), 0);

    instr = lc3_instruction(0xC001);
    BOOST_REQUIRE_EQUAL(instr.opcode(), JMP_INSTR);
    BOOST_CHECK_EQUAL(instr.base_r(), 0);

    instr = lc3_instruction(0x4800);
    BOOST_REQUIRE_EQUAL(instr.opcode(), JSR_INSTR);
    BOOST_REQUIRE(instr.is_jsr());
    BOOST_CHECK_EQUAL(instr.pc_offset11(), 0);

    instr = lc3_instruction(0x4200);
    BOOST_REQUIRE_EQUAL(instr.opcode(), JSRR_INSTR);
    BOOST_REQUIRE(!instr.is_jsr());
    BOOST_CHECK_EQUAL(instr.base_r(), 0);

    instr = lc3_instruction(0x4001);
    BOOST_REQUIRE_EQUAL(instr.opcode(), JSRR_INSTR);
    BOOST_REQUIRE(!instr.is_jsr());
    BOOST_CHECK_EQUAL(instr.base_r(), 0);

    instr = lc3_instruction(0x5008);
    BOOST_REQUIRE_EQUAL(instr.opcode(), AND_INSTR);
    BOOST_REQUIRE(!instr.is_imm());
    BOOST_CHECK_EQUAL(instr.dr(), 0);
    BOOST_CHECK_EQUAL(instr.sr1(), 0);
    BOOST_CHECK_EQUAL(instr.sr2(), 0);

    instr = lc3_instruction(0x903F);
    BOOST_REQUIRE_EQUAL(instr.opcode(), NOT_INSTR);
    BOOST_CHECK_EQUAL(instr.dr(), 0);
    BOOST_CHECK_EQUAL(instr.sr1(), 0);

    instr = lc3_instruction(0xF100);
    BOOST_REQUIRE_EQUAL(instr.opcode(), TRAP_INSTR);
    BOOST_CHECK_EQUAL(instr.vector(), 0);
}

BOOST_FIXTURE_TEST_CASE(MalformedInstructionTest, LC3BasicTest)
{
    const std::vector<unsigned short> malformed_instructionuctions = {0, 1, 0x1008, 0x4001, 0x4200, 0x5008, 0x8001, 0x903E, 0xC001, 0xC200, 0xF100};

    for (const auto& data : malformed_instructionuctions)
    {
        BOOST_TEST_MESSAGE("data = x" << std::hex << data);
        BOOST_REQUIRE(lc3_check_malformed_instruction(lc3_instruction(data)));
        lc3_state_change change = lc3_execute(state, data);
        BOOST_CHECK(state.halted);
        BOOST_CHECK_EQUAL(state.warnings, 1);
        BOOST_CHECK_EQUAL(state.pc, 0x2FFF);
        BOOST_CHECK_EQUAL(change.pc, 0x3000);
        BOOST_CHECK(!change.halted);
        lc3_init(state, false, false);
        state.warning = &warnings;
    }
}

BOOST_FIXTURE_TEST_CASE(MalformedInstructionDisassemble, LC3BasicTest)
{
    const std::vector<unsigned short> malformed_instructionuctions = {0, 1, 0x1008, 0x4001, 0x4200, 0x5008, 0x8001, 0x903E, 0xC001, 0xC200, 0xF100};
    const std::vector<std::string> answers = {"NOP *", "NOP *", "ADD R0, R0, R0 *", "JSRR R0 *", "JSRR R0 *", "AND R0, R0, R0 *", "RTI *", "NOT R0, R0 *", "JMP R0 *", "JMP R0 *", "TRAP x00 *"};


    for (unsigned int i = 0; i < malformed_instructionuctions.size(); i++)
    {
        BOOST_TEST_MESSAGE("data = x" << std::hex << malformed_instructionuctions[i]);
        BOOST_CHECK_EQUAL(lc3_disassemble(state, malformed_instructionuctions[i], -1, 0), answers[i]);
    }

    // This should not be considered malformed
    BOOST_CHECK_EQUAL(lc3_disassemble(state, 0x0E00, -1, 0), "NOP");
}


BOOST_FIXTURE_TEST_CASE(InstructionBasicDisassembleTest, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(allinstrs), allinstrs_len));
    file.ignore(4);
    unsigned short data;

    const std::string answers[18] = {
        "BR #1",
        "ADD R0, R1, R2",
        "ADD R0, R1, #2",
        "LD R0, #-1",
        "ST R0, #-1",
        "JSR #-1",
        "JSRR R0",
        "AND R0, R1, R2",
        "AND R0, R1, #2",
        "LDR R0, R1, #2",
        "STR R0, R1, #2",
        "NOT R0, R1",
        "LDI R0, #-1",
        "STI R0, #-1",
        "RET",
        "JMP R0",
        "LEA R0, #-1",
        "TRAP x25",
    };

    for (unsigned int i = 0; i < 18; i++)
    {
        file.read((char*) &data, sizeof(unsigned short));
        data = ((data >> 8) & 0xFF) | ((data & 0xFF) << 8);
        std::string instruct = lc3_disassemble(state, data, -1, LC3_BASIC_DISASSEMBLE);
        BOOST_CHECK_EQUAL(instruct, answers[i]);
    }

    // Test special cases
    const std::string more_answers[6] =
    {
        "BRN #3",
        "NOP *",
        "RTI",
        "ERROR",
        "NOP ('0') *",
        "NOP",
    };

    // BRN 3
    std::string brn   = lc3_disassemble(state, 0x0803, -1, LC3_BASIC_DISASSEMBLE);
    std::string nop   = lc3_disassemble(state, 0x0000, -1, LC3_BASIC_DISASSEMBLE);
    std::string rti   = lc3_disassemble(state, 0x8000, -1, LC3_BASIC_DISASSEMBLE);
    std::string error = lc3_disassemble(state, 0xD392, -1, LC3_BASIC_DISASSEMBLE);
    std::string nop0  = lc3_disassemble(state, 0x0030, -1, LC3_BASIC_DISASSEMBLE);
    std::string nop2  = lc3_disassemble(state, 0x0E00, -1, LC3_BASIC_DISASSEMBLE);

    BOOST_CHECK_EQUAL(brn, more_answers[0]);
    BOOST_CHECK_EQUAL(nop, more_answers[1]);
    BOOST_CHECK_EQUAL(rti, more_answers[2]);
    BOOST_CHECK_EQUAL(error, more_answers[3]);
    BOOST_CHECK_EQUAL(nop0, more_answers[4]);
    BOOST_CHECK_EQUAL(nop2, more_answers[5]);

}

BOOST_FIXTURE_TEST_CASE(TestDisassemble, LC3BasicTest)
{
    unsigned short data;

    std::stringstream file(std::string(reinterpret_cast<char*>(disassemble), disassemble_len));
    file.ignore(4);

    std::stringstream sym_file(disassemble_sym);
    lc3_load_sym(state, sym_file);

    const std::vector<std::string> answers = {
        "AND R0, R0, R0",
        "AND R0, R1, R1",
        "AND R0, R0, R1",
        "AND R0, R1, R2",
        "AND R0, R0, #-1",
        "AND R0, R1, #-1",
        "AND R0, R7, #0",
        "AND R0, R0, #7",
        "AND R0, R1, #7",
        "ADD R0, R0, R1",
        "ADD R0, R1, R2",
        "ADD R0, R0, #0",
        "ADD R0, R1, #0",
        "ADD R0, R0, #1",
        "ADD R0, R0, #-1",
        "ADD R0, R0, #7",
        "ADD R0, R1, #7",
        "NOT R0, R0",
        "NOT R0, R1",
        "NOP",
        "NOP ('0') *",
        "NOP *",
        "BRN LABEL",
        "BRZ LABEL",
        "BRP LABEL",
        "BRNZ LABEL",
        "BRNP LABEL",
        "BRZP LABEL",
        "BR LABEL",
        "BR LABEL",
        "BR #1",
        "BRZP #-6",
        "JSR LABEL",
        "JSR #1",
        "JSRR R1",
        "RET",
        "JMP R0",
        "LD R0, LABEL",
        "ST R0, LABEL",
        "LDR R0, R1, #0",
        "LDR R0, R1, #7",
        "STR R0, R1, #0",
        "STR R0, R1, #7",
        "LDI R0, LABEL",
        "STI R0, LABEL",
        "STI R0, #-4",
        "LEA R0, LABEL",
        "LEA R0, #1",
        "GETC",
        "OUT",
        "PUTS",
        "IN",
        "PUTSP",
        "HALT",
        "TRAP xFF",
        "RTI",
        "ERROR",
    };

    for (unsigned int i = 0; i < answers.size(); i++)
    {
        state.pc += 1;
        file.read((char*) &data, sizeof(unsigned short));
        data = ((data >> 8) & 0xFF) | ((data & 0xFF) << 8);
        std::string instruct = lc3_disassemble(state, data, -1, LC3_NORMAL_DISASSEMBLE);
        BOOST_CHECK_EQUAL(instruct,  answers[i]);
    }
}

BOOST_FIXTURE_TEST_CASE(TestSmartDisassemble, LC3BasicTest)
{
    unsigned short data;

    std::stringstream file(std::string(reinterpret_cast<char*>(disassemble), disassemble_len));
    file.ignore(4);

    std::stringstream sym_file(disassemble_sym);
    lc3_load_sym(state, sym_file);

    const std::vector<std::string> answers = {
        "TEST R0",
        "R0 = R1",
        "R0 &= R1",
        "R0 = R1 & R2",
        "TEST R0",
        "R0 = R1",
        "R0 = 0",
        "R0 &= 7",
        "R0 = R1 & 7",

        "R0 += R1",
        "R0 = R1 + R2",
        "TEST R0",
        "R0 = R1",
        "R0++",
        "R0--",
        "R0 += 7",
        "R0 = R1 + 7",

        "R0 = ~R0",
        "R0 = ~R1",

        "NOP",
        "NOP ('0') *",
        "NOP *",
        "if cc<0, PC = LABEL",
        "if cc=0, PC = LABEL",
        "if cc>0, PC = LABEL",
        "if cc<=0, PC = LABEL",
        "if cc!=0, PC = LABEL",
        "if cc>=0, PC = LABEL",
        "PC = LABEL",
        "PC = LABEL",
        "PC += 1",
        "if cc>=0, PC -= 6",

        "CALL LABEL",
        "CALL PC + 1",
        "CALL R1",

        "RET",
        "JUMP R0",

        "R0 = LABEL",
        "LABEL = R0",

        "R0 = R1[0]",
        "R0 = R1[7]",

        "R1[0] = R0",
        "R1[7] = R0",

        "R0 = *LABEL",
        "*LABEL = R0",
        "*(PC - 4) = R0",

        "R0 = &LABEL",
        "R0 = PC + 1",

        "GETC",
        "OUT",
        "PUTS",
        "IN",
        "PUTSP",
        "HALT",
        "TRAP xff",

        "RTI",
        "ERROR",
    };

    for (unsigned int i = 0; i < answers.size(); i++)
    {
        state.pc += 1;
        file.read((char*) &data, sizeof(unsigned short));
        data = ((data >> 8) & 0xFF) | ((data & 0xFF) << 8);
        std::string instruct = lc3_disassemble(state, data, -1, LC3_ADVANCED_DISASSEMBLE);
        BOOST_CHECK_EQUAL(instruct,  answers[i]);
    }

}

BOOST_FIXTURE_TEST_CASE(TestArithInstructions, LC3BasicTest)
{
    // ADD R0, R1, R2
    state.regs[0] = 23;
    state.regs[1] = 27;
    state.regs[2] = 43;
    lc3_execute(state, 0x1042);
    BOOST_CHECK_EQUAL(state.regs[0], 70);
    BOOST_CHECK_EQUAL(state.regs[1], 27);
    BOOST_CHECK_EQUAL(state.regs[2], 43);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    // ADD R0, R1, #2
    state.regs[0] = 23;
    state.regs[1] = -666;
    lc3_execute(state, 0x1062);
    BOOST_CHECK_EQUAL(state.regs[0], -664);
    BOOST_CHECK_EQUAL(state.regs[1], -666);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);

    // AND R0, R1, R2
    state.regs[0] = 23;
    state.regs[1] = -1;
    state.regs[2] = 0x0;
    lc3_execute(state, 0x5042);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], -1);
    BOOST_CHECK_EQUAL(state.regs[2], 0x0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);

    // AND R0, R1, #2
    state.regs[0] = 23;
    state.regs[1] = -1;
    lc3_execute(state, 0x5062);
    BOOST_CHECK_EQUAL(state.regs[0], 2);
    BOOST_CHECK_EQUAL(state.regs[1], -1);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    // NOT R0, R1
    state.regs[0] = 23;
    state.regs[1] = 0x0;
    lc3_execute(state, 0x907F);
    BOOST_CHECK_EQUAL(state.regs[0], -1);
    BOOST_CHECK_EQUAL(state.regs[1], 0x0);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);
}

BOOST_FIXTURE_TEST_CASE(TestControlInstructions, LC3BasicTest)
{
    // BR #1 (Taken)
    lc3_execute(state, 0x0E01);
    BOOST_CHECK_EQUAL(state.pc, 0x3001U);

    // BRN #1 (Not taken)
    state.pc = 0x3000U;
    lc3_execute(state, 0x0801);
    BOOST_CHECK_EQUAL(state.pc, 0x3000U);

    // BRZ #-11 (taken)
    lc3_execute(state, 0x05F5);
    BOOST_CHECK_EQUAL(state.pc, 0x2FF5);

    // BRP #1 (Not taken)
    state.pc = 0x3000U;
    lc3_execute(state, 0x0201);
    BOOST_CHECK_EQUAL(state.pc, 0x3000U);

    // .fill #1 (Not taken / This is also a NOP)
    state.strict_execution = 0;
    state.pc = 0x3000U;
    lc3_execute(state, 1);
    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    state.strict_execution = 1;

    // JSR #-1
    state.pc = 0x3000U;
    lc3_execute(state, 0x4FFF);
    BOOST_CHECK_EQUAL(state.pc, 0x2FFFU);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3000);

    // JSRR R0
    state.pc = 0x3000U;
    state.regs[0] = 0x6000;
    lc3_execute(state, 0x4000);
    BOOST_CHECK_EQUAL(state.pc, 0x6000U);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3000);

    // JSRR R7
    state.pc = 0x3000U;
    state.regs[7] = 0x6000;
    lc3_execute(state, 0x41C0);
    BOOST_CHECK_EQUAL(state.pc, 0x6000U);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3000);

    // JMP R0
    state.pc = 0x3000U;
    state.regs[0] = 0x4000;
    state.regs[7] = 0x2000;
    lc3_execute(state, 0xC000);
    BOOST_CHECK_EQUAL(state.pc, 0x4000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0x4000);
    BOOST_CHECK_EQUAL(state.regs[7], 0x2000);

    // RET
    state.pc = 0x3000U;
    state.regs[7] = 0x2000;
    lc3_execute(state, 0xC1C0);
    BOOST_CHECK_EQUAL(state.pc, 0x2000U);
    BOOST_CHECK_EQUAL(state.regs[7], 0x2000);

    // TRAP x25 aka HALT
    state.pc = 0x3000U;
    lc3_execute(state, 0xF025);
    BOOST_CHECK_EQUAL(state.halted, 1);

    // TEST 2 with TRAP x25 (True traps)
    state.true_traps = 1;
    state.pc = 0x3000U;
    state.mem[0x25] = 0x0490;
    lc3_execute(state, 0xF025);
    BOOST_CHECK_EQUAL(state.pc, 0x0490U);
    BOOST_CHECK_EQUAL(state.regs[0x7], 0x3000);
}

BOOST_FIXTURE_TEST_CASE(TestErrorInstructions, LC3BasicTest)
{
    state.pc = 0x3000U;
    lc3_execute(state, 0xD000);
    BOOST_CHECK_EQUAL(state.pc, 0x2FFFU);
    BOOST_CHECK_EQUAL(state.halted, 1);
    BOOST_CHECK_EQUAL(state.warnings, 1U);

    state.pc = 0x3000U;
    state.halted = 0;
    state.warnings = 0;
    lc3_execute(state, 0x8000);
    BOOST_CHECK_EQUAL(state.pc, 0x2FFFU);
    BOOST_CHECK_EQUAL(state.halted, 1);
    BOOST_CHECK_EQUAL(state.warnings, 1U);

    state.pc = 0x3000U;
    state.halted = 0;
    state.warnings = 0;
    lc3_execute(state, 0xF000);
    BOOST_CHECK_EQUAL(state.pc, 0x2FFFU);
    BOOST_CHECK_EQUAL(state.halted, 1);
    BOOST_CHECK_EQUAL(state.warnings, 1U);
}

BOOST_FIXTURE_TEST_CASE(TestMemoryInstructions, LC3BasicTest)
{
    // LD R0, #-1
    state.mem[0x2FFF] = 23;
    lc3_execute(state, 0x21FF);
    BOOST_CHECK_EQUAL(state.regs[0], 23);
    BOOST_CHECK_EQUAL(state.mem[0x2FFF], 23);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    // ST R0, #-1
    state.regs[0] = 0;
    lc3_execute(state, 0x31FF);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.mem[0x2FFF], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    // LDI R0, #-1
    state.mem[0x2FFF] = 0x4000;
    state.mem[0x4000] = -102;
    lc3_execute(state, 0xA1FF);
    BOOST_CHECK_EQUAL(state.regs[0], -102);
    BOOST_CHECK_EQUAL(state.mem[0x2FFF], 0x4000);
    BOOST_CHECK_EQUAL(state.mem[0x4000], -102);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);

    // STI R0, #-1
    state.regs[0] = 1337;
    state.mem[0x2FFF] = 0x4000;
    state.mem[0x4000] = -102;
    lc3_execute(state, 0xB1FF);
    BOOST_CHECK_EQUAL(state.regs[0], 1337);
    BOOST_CHECK_EQUAL(state.mem[0x2FFF], 0x4000);
    BOOST_CHECK_EQUAL(state.mem[0x4000], 1337);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);

    // LEA R0, #-1
    lc3_execute(state, 0xE1FF);
    BOOST_CHECK_EQUAL(state.regs[0], 0x2FFF);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    // LDR R0, R1, #2
    state.regs[0] = 1337;
    state.regs[1] = 0x3004;
    state.mem[0x3006] = -7331;
    lc3_execute(state, 0x6042);
    BOOST_CHECK_EQUAL(state.regs[0], -7331);
    BOOST_CHECK_EQUAL(state.regs[1], 0x3004);
    BOOST_CHECK_EQUAL(state.mem[0x3006], -7331);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);

    // STR R0, R1, #2
    state.regs[0] = 1337;
    state.regs[1] = 0x3004;
    state.mem[0x3006] = -7331;
    lc3_execute(state, 0x7042);
    BOOST_CHECK_EQUAL(state.regs[0], 1337);
    BOOST_CHECK_EQUAL(state.regs[1], 0x3004);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 1337);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);
}

BOOST_FIXTURE_TEST_CASE(TestLoadObj, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));

    lc3_load(state, file, lc3_reader_obj);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0x2003);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 0x2203);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 0x1401);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3003]), 0xF025);
    BOOST_CHECK_EQUAL(state.mem[0x3004], 0x003C);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 0x0006);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0x0000);
}

BOOST_FIXTURE_TEST_CASE(TestLoadHex, LC3BasicTest)
{
    std::stringstream file(simple_hex);
    lc3_load(state, file, lc3_reader_hex);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0x2003);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 0x2203);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 0x1401);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3003]), 0xF025);
    BOOST_CHECK_EQUAL(state.mem[0x3004], 0x003C);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 0x0006);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0x0000);
}

BOOST_FIXTURE_TEST_CASE(TestLoadSym, LC3BasicTest)
{
    std::stringstream file(std::string(
    "DDR\t3005\n"
    "DSR\t3004\n"
    "KBDR\t3028\n"
    "KBSR\t3027\n"
    "MCR\t503\n"
    "POLLIN\t3020\n"
    "POLLIN2\t3023\n"
    "POLLOUT\t3000\n"));

    lc3_load_sym(state, file);

    BOOST_CHECK_EQUAL(state.symbols["MCR"],     0x0503);
    BOOST_CHECK_EQUAL(state.symbols["KBDR"],    0x3028);
    BOOST_CHECK_EQUAL(state.symbols["KBSR"],    0x3027);
    BOOST_CHECK_EQUAL(state.symbols["POLLIN2"], 0x3023);
    BOOST_CHECK_EQUAL(state.symbols["POLLIN"],  0x3020);
    BOOST_CHECK_EQUAL(state.symbols["DDR"],     0x3005);
    BOOST_CHECK_EQUAL(state.symbols["DSR"],     0x3004);
    BOOST_CHECK_EQUAL(state.symbols["POLLOUT"], 0x3000);
}

BOOST_FIXTURE_TEST_CASE(TestRunOne, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_step(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3001U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.executions, 1U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 1U);
}

BOOST_FIXTURE_TEST_CASE(TestRunMany, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_run(state, 3);

    BOOST_CHECK_EQUAL(state.pc, 0x3003U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.regs[1], 6);
    BOOST_CHECK_EQUAL(state.regs[2], 66);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 3U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 3U);
}

BOOST_FIXTURE_TEST_CASE(TestRun, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_run(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3003U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.regs[1], 6);
    BOOST_CHECK_EQUAL(state.regs[2], 66);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.halted, 1);
    BOOST_CHECK_EQUAL(state.executions, 4U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 4U);
}

BOOST_FIXTURE_TEST_CASE(TestBack, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_step(state);
    lc3_back(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestBackMany, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_run(state, 3);
    lc3_rewind(state, 3);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestBackFull, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_run(state);
    lc3_rewind(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestBackInvalid, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_back(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestBackStore, LC3BasicTest)
{
    const unsigned char store[] = {
        0x30, 0x00, 0x00, 0x04, 0x10, 0x2f, 0x30, 0x01, 0xf0, 0x25, 0x1e, 0xd4
    };

    const unsigned int store_len = 12;

    std::stringstream file(std::string(reinterpret_cast<const char*>(store), store_len));
    lc3_load(state, file, lc3_reader_obj);

    std::stringstream sym_file("HELLO\t3003");
    lc3_load_sym(state, sym_file);


    state.regs[0] = 0;
    // ADD R0, R0, 15
    lc3_step(state);
    // ST R0, HELLO
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.mem[state.symbols["HELLO"]], 15);

    // Go back.
    lc3_back(state);
    BOOST_CHECK_EQUAL(state.mem[state.symbols["HELLO"]], 7892);
}

BOOST_FIXTURE_TEST_CASE(TestBackR7, LC3BasicTest)
{
    const unsigned char simple2[] = {
        0x30, 0x00, 0x00, 0x02, 0x1e, 0x01, 0xf0, 0x25
    };

    const unsigned int simple2_len = 8;

    std::stringstream file(std::string(reinterpret_cast<const char*>(simple2), simple2_len));
    lc3_load(state, file, lc3_reader_obj);

    state.regs[0] = 25;
    state.regs[1] = 75;
    state.regs[7] = 0x490;

    lc3_step(state);
    lc3_back(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 25);
    BOOST_CHECK_EQUAL(state.regs[1], 75);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.regs[7], 0x490);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestNextLine, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<const char*>(simplesubr), simplesubr_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_next_line(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3001U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.regs[1], 6);
    BOOST_CHECK_EQUAL(state.regs[2], 66);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 5U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 5U);
}

BOOST_FIXTURE_TEST_CASE(TestPrevLine, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<const char*>(simplesubr), simplesubr_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_next_line(state);
    lc3_prev_line(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3000U);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 0U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);
}

BOOST_FIXTURE_TEST_CASE(TestFinish, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<const char*>(simplesubr), simplesubr_len));
    lc3_load(state, file, lc3_reader_obj);

    lc3_step(state);
    lc3_next_line(state, -1, 1);
    //lc3_finish(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3001U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.regs[1], 6);
    BOOST_CHECK_EQUAL(state.regs[2], 66);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.halted, 0);
    BOOST_CHECK_EQUAL(state.executions, 5U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 5U);
}

BOOST_FIXTURE_TEST_CASE(TestUndoStack, LC3BasicTest)
{
    std::stringstream file(std::string(reinterpret_cast<char*>(simple), simple_len));
    lc3_load(state, file, lc3_reader_obj);

    state.max_stack_size = 1;

    lc3_step(state);
    lc3_step(state);

    BOOST_CHECK_EQUAL(state.undo_stack.size(), 1U);

    lc3_back(state);
    lc3_back(state);

    BOOST_CHECK_EQUAL(state.pc, 0x3001U);
    BOOST_CHECK_EQUAL(state.regs[0], 60);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);
    BOOST_CHECK_EQUAL(state.executions, 1U);
    BOOST_CHECK_EQUAL(state.undo_stack.size(), 0U);

}

BOOST_FIXTURE_TEST_CASE(TestTrapInstructions, LC3BasicTest)
{
    const unsigned char traps[] = {
        0x30, 0x00, 0x00, 0x1b, 0xf0, 0x21, 0xf0, 0x23, 0xf0, 0x20, 0xe0, 0x04,
        0xf0, 0x22, 0xe0, 0x0e, 0xf0, 0x24, 0xf0, 0x25, 0x00, 0x48, 0x00, 0x45,
        0x00, 0x4c, 0x00, 0x4c, 0x00, 0x4f, 0x00, 0x20, 0x00, 0x57, 0x00, 0x4f,
        0x00, 0x52, 0x00, 0x4c, 0x00, 0x44, 0x00, 0x00, 0x31, 0x30, 0x33, 0x32,
        0x35, 0x34, 0x37, 0x36, 0x39, 0x38, 0x00, 0x30, 0x00, 0x00
    };
    const unsigned int traps_len = 58;

    std::stringstream file(std::string(reinterpret_cast<const char*>(traps), traps_len));
    lc3_load(state, file, lc3_reader_obj);

    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3000]), 0xF021);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3001]), 0xF023);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3002]), 0xF020);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3003]), 0xE004);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3004]), 0xF022);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3005]), 0xE00E);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3006]), 0xF024);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3007]), 0xF025);
    BOOST_CHECK_EQUAL(state.mem[0x3008], 'H');
    BOOST_CHECK_EQUAL(state.mem[0x3009], 'E');
    BOOST_CHECK_EQUAL(state.mem[0x300A], 'L');
    BOOST_CHECK_EQUAL(state.mem[0x300B], 'L');
    BOOST_CHECK_EQUAL(state.mem[0x300C], 'O');
    BOOST_CHECK_EQUAL(state.mem[0x300D], ' ');
    BOOST_CHECK_EQUAL(state.mem[0x300E], 'W');
    BOOST_CHECK_EQUAL(state.mem[0x300F], 'O');
    BOOST_CHECK_EQUAL(state.mem[0x3010], 'R');
    BOOST_CHECK_EQUAL(state.mem[0x3011], 'L');
    BOOST_CHECK_EQUAL(state.mem[0x3012], 'D');
    BOOST_CHECK_EQUAL(state.mem[0x3013], 0x0000);
    BOOST_CHECK_EQUAL(state.mem[0x3014], '1' << 8 | '0');
    BOOST_CHECK_EQUAL(state.mem[0x3015], '3' << 8 | '2');
    BOOST_CHECK_EQUAL(state.mem[0x3016], '5' << 8 | '4');
    BOOST_CHECK_EQUAL(state.mem[0x3017], '7' << 8 | '6');
    BOOST_CHECK_EQUAL(state.mem[0x3018], '9' << 8 | '8');
    BOOST_CHECK_EQUAL(state.mem[0x3019], '0');
    BOOST_CHECK_EQUAL(state.mem[0x301A], 0x0000);


    std::istringstream proginput("BC");
    std::ostringstream progoutput;

    state.input = &proginput;
    state.output = &progoutput;

    // OUT;
    state.regs[0] = 65;
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3001);

    // IN
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.regs[0], 66);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3002);

    // GETC
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.regs[0], 67);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3003);

    // LEA R0, HELLOWORLD; PUTS;
    lc3_run(state, 2);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3005);

    // LEA R0, PUTSPSTR; PUTSP;
    lc3_run(state, 2);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3007);

    // HALT
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.halted, 0x1);
    BOOST_CHECK_EQUAL(state.pc, 0x3007);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3008);

    state.input = &std::cin;
    state.output = &std::cout;

    std::istringstream verify(progoutput.str());
    BOOST_CHECK_EQUAL(verify.get(), 'A');

    const std::string answers[3] = {"Input character: ", "HELLO WORLD", "01234567890"};
    char str[18];
    std::string val;

    verify.get(str, 18);
    str[17] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[0]);

    BOOST_CHECK_EQUAL(verify.get(), 66);

    verify.get(str, 12);
    str[12] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[1]);

    verify.get(str, 12);
    str[12] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[2]);
}

BOOST_FIXTURE_TEST_CASE(TestTrueTraps, LC3BasicTest)
{
    std::istringstream file(
        ";@version 0\n"
        ".orig x3000\n"
        "OUT\n"
        "IN\n"
        "GETC\n"
        "LEA R0, HW\n"
        "PUTS\n"
        "LEA R0, PUTSP_STR\n"
        "PUTSP\n"
        "HALT\n"
        "HW .stringz \"HELLO WORLD\"\n"
        "PUTSP_STR .fill x3130\n"
        ".fill x3332\n"
        ".fill x3534\n"
        ".fill x3736\n"
        ".fill x3938\n"
        ".fill x0030\n"
        ".fill x0000\n"
        ".end"
    );

    try
    {
        LC3AssembleOptions options;
        options.multiple_errors = false;
        lc3_assemble(state, file, options);
    }
    catch (const LC3AssembleException& e)
    {
        BOOST_FAIL(e.what());
    }

    lc3_set_true_traps(state, true);

    std::istringstream proginput("BC");
    std::ostringstream progoutput;

    state.input = &proginput;
    state.output = &progoutput;

    short r1 = state.regs[1];
    short r2 = state.regs[2];
    short r3 = state.regs[3];
    short r4 = state.regs[4];
    short r5 = state.regs[5];
    short r6 = state.regs[6];

    // OUT;
    state.regs[0] = 65;
    lc3_next_line(state);
    BOOST_REQUIRE_EQUAL(state.pc, 0x3001);
    BOOST_CHECK_EQUAL(state.regs[0], 65);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3001);

    // IN
    lc3_next_line(state);
    BOOST_CHECK_EQUAL(state.regs[0], 66);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3002);

    // GETC
    lc3_next_line(state);
    BOOST_CHECK_EQUAL(state.regs[0], 67);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3003);

    // LEA R0, HELLOWORLD; PUTS;
    lc3_next_line(state);
    lc3_next_line(state);
    BOOST_CHECK_EQUAL(state.regs[0], 0x3008);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3005);

    // LEA R0, PUTSPSTR; PUTSP;
    lc3_next_line(state);
    lc3_next_line(state);
    BOOST_CHECK_EQUAL(state.regs[0], 0x3014);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3007);

    // HALT
    lc3_next_line(state);
    BOOST_CHECK_EQUAL(state.halted, 0x1);
    BOOST_CHECK_EQUAL(state.regs[0], 0x3014);
    BOOST_CHECK_EQUAL(state.regs[1], r1);
    BOOST_CHECK_EQUAL(state.regs[2], r2);
    BOOST_CHECK_EQUAL(state.regs[3], r3);
    BOOST_CHECK_EQUAL(state.regs[4], r4);
    BOOST_CHECK_EQUAL(state.regs[5], r5);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.mem[0xFFFE], 0);

    state.input = &std::cin;
    state.output = &std::cout;

    std::istringstream verify(progoutput.str());
    BOOST_CHECK_EQUAL(verify.get(), 'A');

    const std::string answers[3] = {"Input character: ", "HELLO WORLD", "01234567890"};
    char str[18];
    std::string val;

    verify.get(str, 18);
    str[17] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[0]);

    BOOST_CHECK_EQUAL(verify.get(), 66);

    verify.get(str, 12);
    str[12] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[1]);

    verify.get(str, 12);
    str[12] = 0;
    val = str;
    BOOST_CHECK_EQUAL(val, answers[2]);
}


BOOST_FIXTURE_TEST_CASE(TestDeviceRegisters, LC3BasicTest)
{
    const unsigned char devreg[] = {
        0x30, 0x00, 0x00, 0x06, 0xa0, 0x03, 0x07, 0xfe, 0xb2, 0x02, 0xf0, 0x25,
        0xfe, 0x04, 0xfe, 0x06, 0x30, 0x20, 0x00, 0x09, 0xa0, 0x06, 0x07, 0xfe,
        0xa2, 0x05, 0xa0, 0x03, 0x07, 0xfe, 0xa4, 0x02, 0xf0, 0x25, 0xfe, 0x00,
        0xfe, 0x02, 0x05, 0x00, 0x00, 0x04, 0x50, 0x20, 0xa2, 0x01, 0xb0, 0x00,
        0xff, 0xfe
    };
    const unsigned int devreg_len = 50;

    std::stringstream file(std::string(reinterpret_cast<const char*>(devreg), devreg_len));
    lc3_load(state, file, lc3_reader_obj);

    state.regs[1] = 65;

    std::istringstream proginput("BC");
    std::ostringstream progoutput;

    state.input = &proginput;
    state.output = &progoutput;

    // Execute DSR/DDR test
    lc3_run(state);

    // Execute KBSR/KBDR test
    state.halted = 0;
    state.pc = 0x3020;
    lc3_run(state);

    BOOST_CHECK_EQUAL(state.regs[1], 66);
    BOOST_CHECK_EQUAL(state.regs[2], 67);

    // Execute MCR Test
    state.halted = 0;
    state.pc = 0x500;
    lc3_run(state);

    BOOST_CHECK_EQUAL(state.halted, 1);
    BOOST_CHECK_EQUAL(state.pc, 0x502);
    BOOST_CHECK_EQUAL(state.warnings, 0U); // No warning since its in the OS MEM.
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.regs[1]), 0x8000);

    state.input = &std::cin;
    state.output = &std::cout;

    std::istringstream verify(progoutput.str());
    BOOST_CHECK_EQUAL(verify.get(), 65);
}

BOOST_FIXTURE_TEST_CASE(TestSymbolTable, LC3BasicTest)
{
    // LOL is not in the symbol table.
    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "LOL"), -1);
    // 0x3000 is not referenced in the symbol table
    BOOST_CHECK_EQUAL(lc3_sym_rev_lookup(state, 0x3000), "");

    // Add LOL to the symbol table
    lc3_sym_add(state, "LOL", 0x3000);


    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "LOL"), 0x3000);
    BOOST_CHECK_EQUAL(lc3_sym_rev_lookup(state, 0x3000), "LOL");

    lc3_sym_delete(state, "LOL");

    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "LOL"), -1);
    BOOST_CHECK_EQUAL(lc3_sym_rev_lookup(state, 0x3000), "");

    lc3_sym_add(state, "LOL_LOL", 0x3000);

    state.regs[0] = 0;
    state.regs[1] = 0;
    state.regs[2] = 0;
    state.regs[3] = 0;
    state.regs[4] = 0;
    state.regs[5] = 0;
    state.regs[6] = 0;
    state.regs[7] = 0;

    state.mem[0] = 0;

    BOOST_CHECK_EQUAL(lc3_calculate(state, "*LOL_LOL + R0 + R1 + R2+R3 + R4 + R5 + R6 + R7"), state.mem[0x3000]);
    BOOST_CHECK_EQUAL(lc3_calculate(state, "&LOL_LOL + LOL_LOL"), 0);

    auto undefined_symbol = [](const LC3CalculateException& e) {return e.get_id() == ExpressionEvaluator::eval_undefinedsymbol;};
    BOOST_CHECK_EXCEPTION(lc3_calculate(state, "NOTHERE"), LC3CalculateException, undefined_symbol);
    // References
    BOOST_CHECK_EQUAL(lc3_calculate(state, "R0[0] + R1[0] + R2[0] + R3[0] + R4[0] + R5[0] + R6[0] + R7[0] + MEM[0] + LOL_LOL[0]"), state.mem[0x3000]);
    BOOST_CHECK_EQUAL(lc3_calculate(state, "PC + PC[0]"), state.pc);

    lc3_sym_clear(state);

    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "LOL"), -1);
    BOOST_CHECK_EQUAL(lc3_sym_rev_lookup(state, 0x3000), "");
}

BOOST_FIXTURE_TEST_CASE(TestBreakpoints, LC3BasicTest)
{
    state.strict_execution = 0;
    lc3_add_breakpoint(state, 0x3012);
    lc3_add_breakpoint(state, 0x3016, "", "", "1");
    lc3_add_breakpoint(state, 0x3020, "", "", "0");
    lc3_add_breakpoint(state, 0x3040, "", "", "1", 3);
    state.mem[0x3060] = (short)0xF025;

    lc3_run(state);
    BOOST_CHECK_EQUAL(state.pc, 0x3012);

    state.halted = 0;

    lc3_run(state);
    BOOST_CHECK_EQUAL(state.pc, 0x3016);

    state.halted = 0;
    lc3_run(state, 5);
    BOOST_CHECK_EQUAL(state.halted, 0);

    for (int i = 0; i < 4; i++)
    {
        state.halted = 0;
        lc3_run(state);
        state.pc = 0x3025;
    }

    const auto& breakpoint = state.breakpoints[0x3040];
    BOOST_CHECK(!breakpoint.enabled);
}

BOOST_FIXTURE_TEST_CASE(TestBreakpointMessages, LC3BasicTest)
{
    state.strict_execution = 0;
    lc3_add_breakpoint(state, 0x3001, "", "{{PC}} R0 is {{R0}} ok {{R0==1}}");
    std::ostringstream out;
    state.debug = &out;

    state.regs[0] = 3;
    lc3_run(state);

    BOOST_REQUIRE_EQUAL(state.pc, 0x3001);
    auto& breakpoint = state.breakpoints[0x3001];
    BOOST_REQUIRE_EQUAL(breakpoint.hit_count, 1);
    BOOST_CHECK_EQUAL(out.str(), "12289 R0 is 3 ok 0\n");
}

BOOST_FIXTURE_TEST_CASE(InstructionBasicAssembleTest, LC3BasicTest)
{
    const std::vector<std::string> instruct = {
        "BR #1",
        "ADD R0, R1, R2",
        "ADD R0, R1, #2",
        "LD R0, #-1",
        "ST R0, #-1",
        "JSR #-1",
        "JSRR R0",
        "AND R0, R1, R2",
        "AND R0, R1, #2",
        "LDR R0, R1, #2",
        "STR R0, R1, #2",
        "NOT R0, R1",
        "LDI R0, #-1",
        "STI R0, #-1",
        "RET",
        "JMP R0",
        "LEA R0, #-1",
        "TRAP x25",
        "BR 012",
        "GETC",
        "OUT",
        "PUTS",
        "IN",
        "PUTSP",
        "HALT",
        "RTI",
        "BRN #0",
        "BRNZ #0",
        "BRNP #0",
        "BRZ #0",
        "BRZP #0",
        "BRP #0",
        "BRNZP #0",
        "TRAP xFF"
    };

    std::vector<unsigned short> answers = {
        0x0E01U,
        0x1042U,
        0x1062U,
        0x21FFU,
        0x31FFU,
        0x4FFFU,
        0x4000U,
        0x5042U,
        0x5062U,
        0x6042U,
        0x7042U,
        0x907FU,
        0xA1FFU,
        0xB1FFU,
        0xC1C0U,
        0xC000U,
        0xE1FFU,
        0xF025U,
        0x0E0AU,
        0xF020U,
        0xF021U,
        0xF022U,
        0xF023U,
        0xF024U,
        0xF025U,
        0x8000U,
        0x0800U,
        0x0C00U,
        0x0A00U,
        0x0400U,
        0x0600U,
        0x0200U,
        0x0E00U,
        0xF0FFU,
    };

    for (unsigned int i = 0; i < instruct.size(); i++)
    {
        try
        {
            std::string instruction = instruct[i];
            unsigned short assembled = lc3_assemble_one(state, 0, instruction);
            //BOOST_TEST_MESSAGE(instruction.c_str());
            BOOST_CHECK_EQUAL(assembled,  answers[i]);
        }
        catch (const LC3AssembleException& e)
        {
            BOOST_FAIL(e.what());
        }
    }
}

BOOST_FIXTURE_TEST_CASE(InstructionAssembleTest, LC3BasicTest)
{
    std::istringstream file(
        ".orig x3000\n"
        "\tLD R0, A\n"
        "\tLD R1, B\n"
        "\tADD R2, R0, R1\n"
        "\tHALT\n"
        "\tA .fill 60\n"
        "\tB .fill 6\n"
        ".end"
    );

    try
    {
        LC3AssembleOptions options;
        options.multiple_errors = false;
        lc3_assemble(state, file, options);
    }
    catch (const LC3AssembleException& e)
    {
        BOOST_FAIL(e.what());
    }

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0x2003);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 0x2203);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 0x1401);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3003]), 0xF025);
    BOOST_CHECK_EQUAL(state.mem[0x3004], 0x003C);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 0x0006);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0x0000);

    BOOST_CHECK_EQUAL(state.symbols["A"], 0x3004);
    BOOST_CHECK_EQUAL(state.symbols["B"], 0x3005);
}

BOOST_FIXTURE_TEST_CASE(InitRandomTest, LC3BasicTest)
{
    state.default_seed = 1000;

    lc3_init(state, false, false);

    unsigned short first = lc3_random(state);

    lc3_init(state, false, false);

    unsigned short second = lc3_random(state);

    BOOST_CHECK_EQUAL(first, second);
}

BOOST_FIXTURE_TEST_CASE(TestCustomTrapV0, LC3BasicTest)
{
    std::istringstream file(
        ".orig x30\n"
        ".fill x1000\n"
        ".end\n\n"

        ".orig x1000\n"
        "AND R0, R0, 0\n"
        "RET\n"
        ".end\n\n"

        ".orig x3000\n"
        "TRAP x30\n"
        ".end\n\n"
    );
    try
    {
        LC3AssembleOptions options;
        options.multiple_errors = false;
        lc3_assemble(state, file, options);
    }
    catch (const LC3AssembleException& e)
    {
        BOOST_FAIL(e.what());
    }

    lc3_set_true_traps(state, true);
    state.n = 0;
    state.z = 0;
    state.p = 1;

    BOOST_REQUIRE_EQUAL(state.privilege, 1);
    // ========
    // TRAP x30
    // ========
    short r6 = state.regs[6];

    lc3_step(state);
    // PSR check.
    BOOST_CHECK_EQUAL(state.privilege, 1);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    BOOST_CHECK_EQUAL(state.pc, 0x1000);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.regs[7], 0x3001);

    // Internal state checks
    BOOST_REQUIRE_EQUAL(state.undo_stack.size(), 1);
    lc3_state_change change = state.undo_stack.back();
    BOOST_CHECK_EQUAL(change.changes, LC3_SUBROUTINE_BEGIN);
    BOOST_CHECK_EQUAL(change.privilege, 1);
    BOOST_CHECK_EQUAL(change.pc, 0x3001);
    BOOST_CHECK_EQUAL(change.subroutine.is_trap, true);
    BOOST_CHECK_EQUAL(change.subroutine.address, 0x1000);

    BOOST_REQUIRE(state.rti_stack.empty());

    BOOST_REQUIRE_EQUAL(state.call_stack.size(), 1);
    lc3_subroutine_call call = state.call_stack.back();
    BOOST_CHECK_EQUAL(call.address, 0x1000);
    BOOST_CHECK_EQUAL(call.is_trap, true);

    // =============
    // AND R0, R0, 0
    // =============
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.pc, 0x1001);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);

    // ===
    // RET
    // ===
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.privilege, 1);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);

    BOOST_CHECK_EQUAL(state.pc, 0x3001);
    BOOST_CHECK_EQUAL(state.regs[6], r6);

    // Internal state checks
    BOOST_REQUIRE_EQUAL(state.undo_stack.size(), 3);
    change = state.undo_stack.back();
    BOOST_CHECK_EQUAL(change.changes, LC3_SUBROUTINE_END);
    BOOST_CHECK_EQUAL(change.privilege, 1);
    BOOST_CHECK_EQUAL(change.pc, 0x1002);

    BOOST_CHECK(state.rti_stack.empty());
    BOOST_CHECK(state.call_stack.empty());
}

BOOST_FIXTURE_TEST_CASE(TestCustomMultiTrapV0, LC3BasicTest)
{
    std::istringstream file(
        ";@version 0\n"
        ".orig x30\n"
        ".fill x1000\n"
        ".fill x1010\n"
        ".fill x1020\n"
        ".end\n\n"

        ".orig x1000\n"
        "AND R0, R0, 0\n"
        "TRAP x31\n"
        "RTI\n"
        ".end\n\n"

        ".orig x1010\n"
        "ADD R0, R0, -1\n"
        "TRAP x32\n"
        "RTI\n"
        ".end\n\n"

        ".orig x1020\n"
        "RTI\n"
        ".end\n\n"

        ".orig x3000\n"
        "TRAP x30\n"
        ".end\n\n"
    );
    try
    {
        LC3AssembleOptions options;
        options.multiple_errors = false;
        lc3_assemble(state, file, options);
    }
    catch (const LC3AssembleException& e)
    {
        BOOST_FAIL(e.what());
    }

    lc3_set_true_traps(state, true);
    state.n = 0;
    state.z = 0;
    state.p = 1;

    BOOST_REQUIRE_EQUAL(state.privilege, 1);
    lc3_run(state, 5); // x1020.

    BOOST_CHECK_EQUAL(state.pc, 0x1020);

    // Internal state checks
    BOOST_CHECK(state.rti_stack.empty());
    BOOST_CHECK_EQUAL(state.call_stack.size(), 3);

    BOOST_CHECK_EQUAL(state.call_stack[0].address, 0x1000);
    BOOST_CHECK_EQUAL(state.call_stack[0].is_trap, true);
    BOOST_CHECK_EQUAL(state.call_stack[1].address, 0x1010);
    BOOST_CHECK_EQUAL(state.call_stack[1].is_trap, true);
    BOOST_CHECK_EQUAL(state.call_stack[2].address, 0x1020);
    BOOST_CHECK_EQUAL(state.call_stack[2].is_trap, true);
}

