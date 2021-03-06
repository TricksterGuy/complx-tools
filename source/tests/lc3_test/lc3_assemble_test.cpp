#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>
#include <lc3.hpp>

#define IS_EXCEPTION(type) [](const LC3AssembleException& e) {return e.get_id() == type;}

struct LC3AssembleTest
{
    lc3_state state;
    LC3AssembleOptions options;

    LC3AssembleTest()
    {
        lc3_init(state, false, false);
        options.multiple_errors = false;
    }
};

BOOST_FIXTURE_TEST_CASE(AddImmediateTest, LC3AssembleTest)
{
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 0"), 0x1020);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 1"), 0x1021);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, -1"), 0x103F);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 8"), 0x1028);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, -16"), 0x1030);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 15"), 0x102F);

    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, xF"), 0x102F);

    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 16", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x1F", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x10", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, xFFFF", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
}

BOOST_FIXTURE_TEST_CASE(AndImmediateTest, LC3AssembleTest)
{
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 0"), 0x5020);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 1"), 0x5021);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, -1"), 0x503F);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 8"), 0x5028);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, -16"), 0x5030);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 15"), 0x502F);

    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, xF"), 0x502F);

    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, 16", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, x1F", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, x10", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, xFFFF", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
}

BOOST_FIXTURE_TEST_CASE(FillTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ".fill 0\n"
        ".fill 1\n"
        ".fill 10\n"
        ".fill xFFFF\n"
        ".fill -1\n"
        ".fill 'A'\n"
        ".fill -32768\n"
        ".fill x8000\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 1);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 10);
    BOOST_CHECK_EQUAL(state.mem[0x3003], -1);
    BOOST_CHECK_EQUAL(state.mem[0x3004], -1);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 'A');
    BOOST_CHECK_EQUAL(state.mem[0x3006], -32768);
    BOOST_CHECK_EQUAL(state.mem[0x3007], -32768);

    std::istringstream bad_file(
        ".orig x3000\n"
        ".fill 32768\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, bad_file, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));

}

BOOST_FIXTURE_TEST_CASE(StringzTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        "A .stringz \"Hello!\"\n"
        "B .stringz \"World\"\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 'H');
    BOOST_CHECK_EQUAL(state.mem[0x3001], 'e');
    BOOST_CHECK_EQUAL(state.mem[0x3002], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x3003], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x3004], 'o');
    BOOST_CHECK_EQUAL(state.mem[0x3005], '!');
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0);
    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "A"), 0x3000);

    BOOST_CHECK_EQUAL(state.mem[0x3007], 'W');
    BOOST_CHECK_EQUAL(state.mem[0x3008], 'o');
    BOOST_CHECK_EQUAL(state.mem[0x3009], 'r');
    BOOST_CHECK_EQUAL(state.mem[0x300A], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x300B], 'd');
    BOOST_CHECK_EQUAL(state.mem[0x300C], 0);
    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "B"), 0x3007);

    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz lol\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));

    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz lol\"lol\"\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));

    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz \"lol\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(UNTERMINATED_STRING));

    // "lol\" Stray escape character.
    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz \"lol\\\"\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));

    // Unknown escape sequence \z.
    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz \"lol\\z\"\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));

    file.clear();
    file.str(
        ".orig x3000\n"
        ".stringz \"lol\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(UNTERMINATED_STRING));

    file.clear();
    file.str(
        ".orig xfff0\n"
        ".stringz \"I am a long strong at the end of memory.\"\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(MEMORY_OVERFLOW));
}

BOOST_FIXTURE_TEST_CASE(InvalidSymbolTest, LC3AssembleTest)
{
    std::istringstream long_sym(
        ".orig x3000\n"
        "IAMAVERYLONGSYMBOLGUIZE .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, long_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream register_sym(
        ".orig x3000\n"
        "R0 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, register_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream hexa_sym(
        ".orig x3000\n"
        "x8 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, hexa_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream bin_sym(
        ".orig x3000\n"
        "b101 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, bin_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream num_sym(
        ".orig x3000\n"
        "8HIYA .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, num_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream invalid_sym(
        ".orig x3000\n"
        "DOLLADOLLA$$$ .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, invalid_sym, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));
}

BOOST_FIXTURE_TEST_CASE(BinaryLiteralTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ".fill b0\n"
        ".fill b1\n"
        ".fill b0111000011101111\n"
        "ADD R0, R1, b01111\n"
        "LEA R0, b011110000\n"
        "JSR b01111000001\n"
        "LDR R0, R1, b011111\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 1);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 0x70EF);
    BOOST_CHECK_EQUAL(state.mem[0x3003], 0x106F);
    BOOST_CHECK_EQUAL(static_cast<unsigned short>(state.mem[0x3004]), 0xE0F0);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 0x4BC1);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0x605F);
}

BOOST_FIXTURE_TEST_CASE(DebugCommentsBreakpointTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ";@break"
        "HALT\n"
        ";@break address=x2fff name=backwards condition=R0==3 times=6 message=\"hey triggered\"\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_REQUIRE(lc3_has_breakpoint(state, 0x3000));
    BOOST_REQUIRE(lc3_has_breakpoint(state, 0x2fff));

    BOOST_REQUIRE_EQUAL(state.breakpoints.size(), 2);

    const auto& first = state.breakpoints[0x3000];
    const auto& second = state.breakpoints[0x2fff];

    BOOST_CHECK(first.enabled);
    auto first_target = std::get<lc3_breakpoint_target>(first.target);
    BOOST_CHECK_EQUAL(first_target.address, 0x3000);
    BOOST_CHECK_EQUAL(first.name, "");
    BOOST_CHECK_EQUAL(first.condition, "1");
    BOOST_CHECK_EQUAL(first.max_hits, -1);
    BOOST_CHECK_EQUAL(first.message, "");

    BOOST_CHECK(second.enabled);
    auto second_target = std::get<lc3_breakpoint_target>(second.target);
    BOOST_CHECK_EQUAL(second_target.address, 0x2fff);
    BOOST_CHECK_EQUAL(second.name, "backwards");
    BOOST_CHECK_EQUAL(second.condition, "R0==3");
    BOOST_CHECK_EQUAL(second.max_hits, 6);
    BOOST_CHECK_EQUAL(second.message, "hey triggered");
}

BOOST_FIXTURE_TEST_CASE(DebugCommentsWatchpointTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ";@watch"
        "HALT\n"
        ";@watch target=R0 condition=R0==3 name=R0three message=\"R0 is 3\" times=6\n"
        ";@watch target=x3030 condition=1\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_REQUIRE(lc3_has_watchpoint(state, false, 0x3000));
    BOOST_REQUIRE(lc3_has_watchpoint(state, false, 0x3030));
    BOOST_REQUIRE(lc3_has_watchpoint(state, true, 0));

    BOOST_REQUIRE_EQUAL(state.reg_watchpoints.size(), 1);
    BOOST_REQUIRE_EQUAL(state.mem_watchpoints.size(), 2);

    const auto& first = state.mem_watchpoints[0x3000];
    const auto& second = state.mem_watchpoints[0x3030];
    const auto& third = state.reg_watchpoints[0];

    BOOST_CHECK(first.enabled);
    auto first_target = std::get<lc3_watchpoint_target>(first.target);
    BOOST_CHECK_EQUAL(first_target.is_reg, false);
    BOOST_CHECK_EQUAL(first_target.target, 0x3000);
    BOOST_CHECK_EQUAL(first.name, "");
    BOOST_CHECK_EQUAL(first.condition, "1");
    BOOST_CHECK_EQUAL(first.max_hits, -1);
    BOOST_CHECK_EQUAL(first.message, "");

    BOOST_CHECK(second.enabled);
    auto second_target = std::get<lc3_watchpoint_target>(second.target);
    BOOST_CHECK_EQUAL(second_target.is_reg, false);
    BOOST_CHECK_EQUAL(second_target.target, 0x3030);
    BOOST_CHECK_EQUAL(second.name, "");
    BOOST_CHECK_EQUAL(second.condition, "1");
    BOOST_CHECK_EQUAL(second.max_hits, -1);
    BOOST_CHECK_EQUAL(second.message, "");

    BOOST_CHECK(third.enabled);
    auto third_target = std::get<lc3_watchpoint_target>(third.target);
    BOOST_CHECK_EQUAL(third_target.is_reg, true);
    BOOST_CHECK_EQUAL(third_target.target, 0);
    BOOST_CHECK_EQUAL(third.name, "R0three");
    BOOST_CHECK_EQUAL(third.condition, "R0==3");
    BOOST_CHECK_EQUAL(third.max_hits, 6);
    BOOST_CHECK_EQUAL(third.message, "R0 is 3");
}

BOOST_FIXTURE_TEST_CASE(InvalidRegisterTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        "ADD R8, R0, R0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(INVALID_REGISTER));

    file.clear();
    file.str(
        ".orig x3000\n"
        "ADD R1000, R0, R0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(INVALID_REGISTER));

    file.clear();
    file.str(
        ".orig x3000\n"
        "ADD SYM, R0, R0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(INVALID_REGISTER));
}

BOOST_FIXTURE_TEST_CASE(InvalidImmediateOffsetTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        "ADD R0, R0, 17\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));

    file.clear();
    file.str(
        ".orig x3000\n"
        "LDR R0, R0, 65\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(OFFSET_OVERFLOW));
}
