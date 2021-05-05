#include "lc3/lc3_debug.hpp"

#include <cassert>
#include <sstream>

#include "lc3/lc3_execute.hpp"
#include "lc3/lc3_expressions.hpp"
#include "lc3/lc3_symbol.hpp"

bool lc3_add_subroutine(lc3_state& state, uint16_t address, const std::string& name, int num_params, const std::vector<std::string>& params);

bool lc3_has_breakpoint(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return false;

    return lc3_has_breakpoint(state, addr);
}

bool lc3_has_breakpoint(lc3_state& state, uint16_t addr)
{
    return state.breakpoints.find(addr) != state.breakpoints.end();
}

bool lc3_has_watchpoint(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return false;

    return lc3_has_watchpoint(state, false, addr);
}

bool lc3_has_watchpoint(lc3_state& state, bool is_reg, uint16_t data)
{
    if (is_reg)
        return state.reg_watchpoints.find(data) != state.reg_watchpoints.end();
    else
        return state.mem_watchpoints.find(data) != state.mem_watchpoints.end();
}

bool lc3_add_breakpoint(lc3_state& state, const std::string& symbol, const std::string& name, const std::string& message, const std::string& condition, int times)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_add_breakpoint(state, addr, name, message, condition, times);
}

bool lc3_add_breakpoint(lc3_state& state, uint16_t addr, const std::string& name, const std::string& message, const std::string& condition, int times)
{
    if (state.breakpoints.find(addr) != state.breakpoints.end()) return true;

    lc3_debug_info info;
    info.enabled = true;
    info.target = lc3_breakpoint_target{addr};
    info.max_hits = times;
    info.hit_count = 0;
    info.name = name;
    info.message = message;
    info.condition = condition;

    state.breakpoints[addr] = info;

    return false;
}

bool lc3_add_watchpoint(lc3_state& state, const std::string& symbol, const std::string& condition, const std::string& name, const std::string& message, int times)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_add_watchpoint(state, false, addr, condition, name, message, times);
}

bool lc3_add_watchpoint(lc3_state& state, bool is_reg, uint16_t data, const std::string& condition, const std::string& name, const std::string& message, int times)
{
    if (is_reg)
    {
        assert(data <= 7);
        if (state.reg_watchpoints.find(data) != state.reg_watchpoints.end()) return true;
    }
    else if (state.mem_watchpoints.find(data) != state.mem_watchpoints.end())
    {
        return true;
    }

    lc3_debug_info info;
    info.enabled = true;
    info.target = lc3_watchpoint_target{is_reg, data};
    info.max_hits = times;
    info.hit_count = 0;
    info.name = name;
    info.condition = condition;
    info.message = message;

    if (is_reg)
        state.reg_watchpoints[data] = info;
    else
        state.mem_watchpoints[data] = info;

    return false;
}

bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, const std::vector<std::string>& params)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;
    return lc3_add_subroutine(state, addr, name, params.size(), params);
}

bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, int num_params)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;
    std::vector<std::string> params;
    return lc3_add_subroutine(state, addr, name, num_params, params);
}

bool lc3_add_subroutine(lc3_state& state, uint16_t address, const std::string& name, const std::vector<std::string>& params)
{
    return lc3_add_subroutine(state, address, name, params.size(), params);
}

bool lc3_add_subroutine(lc3_state& state, uint16_t address, const std::string& name, int num_params)
{
    std::vector<std::string> params;
    return lc3_add_subroutine(state, address, name, num_params, params);
}

bool lc3_add_subroutine(lc3_state& state, uint16_t address, const std::string& name, int num_params, const std::vector<std::string>& params)
{
    if (state.subroutines.find(address) != state.subroutines.end()) return true;

    lc3_subroutine_info info;
    info.address = address;
    info.name = name;
    info.num_params = num_params;
    info.params = params;
    state.subroutines[address] = info;

    return false;
}

bool lc3_remove_breakpoint(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_remove_breakpoint(state, addr);
}

bool lc3_remove_breakpoint(lc3_state& state, uint16_t addr)
{
    if (state.breakpoints.find(addr) == state.breakpoints.end()) return true;

    state.breakpoints.erase(addr);

    return false;
}

bool lc3_remove_watchpoint(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    lc3_remove_watchpoint(state, false, addr);
    return false;
}

bool lc3_remove_watchpoint(lc3_state& state, bool is_reg, uint16_t data)
{
    if (is_reg)
    {
        if (state.reg_watchpoints.find(data) == state.reg_watchpoints.end()) return true;
        state.reg_watchpoints.erase(data);
    }
    else
    {
        if (state.mem_watchpoints.find(data) == state.mem_watchpoints.end()) return true;
        state.mem_watchpoints.erase(data);
    }

    return false;
}

std::string form_debug_message(lc3_state& state, lc3_debug_info& info)
{
    std::string_view message = info.message;

    std::stringstream msg;
    size_t i = 0;
    while (i < message.size())
    {
        size_t n = message.find_first_of("{{", i);
        msg << message.substr(i, n - i);
        if (n == std::string_view::npos)
            break;
        size_t z = message.find_first_of("}}", n);
        if (z == std::string_view::npos)
        {
            msg << message.substr(n);
            break;
        }

        try
        {
            msg << lc3_calculate(state, message.substr(n + 2, z - (n + 2)));
        }
        catch (const LC3CalculateException& e)
        {
            std::stringstream msg;
            msg << info.target_string() << " name: " << info.name << " " << e.what() << "\nHalting processor.";
            lc3_warning(state, msg.str());
            state.halted = 1;
            state.pc--;
            return "";
        }

        i = z + 2;
    }

    return msg.str();
}

void lc3_break_eval(lc3_state& state, lc3_debug_info& info)
{
        int triggered = 0;
        try
        {
            triggered = lc3_calculate(state, info.condition);
        }
        catch (const LC3CalculateException& e)
        {
            std::stringstream msg;
            msg << info.target_string() << " name: " << info.name << " " << e.what() << "\nHalting processor.";
            lc3_warning(state, msg.str());
            state.halted = 1;
            state.pc--;
            return;
        }

        if (triggered)
        {
            state.halted = 1;
            info.hit_count++;

            if (state.debug)
                (*state.debug) << form_debug_message(state, info);

            if (info.max_hits >= 0 && info.hit_count >= info.max_hits)
                info.enabled = false;
        }
}


bool lc3_break_test(lc3_state& state, const lc3_state_change* changes)
{
    // Test for breakpoints
    const auto& breakpoint = state.breakpoints.find(state.pc);
    if (breakpoint != state.breakpoints.end())
        lc3_break_eval(state, breakpoint->second);

    // Test for watchpoints
    if (changes->changes == LC3_REGISTER_CHANGE)
    {
        const auto& watchpoint = state.reg_watchpoints.find(changes->location);
        if (watchpoint != state.reg_watchpoints.end())
            lc3_break_eval(state, watchpoint->second);
    }
    else if (changes->changes == LC3_MEMORY_CHANGE)
    {
        const auto& watchpoint = state.mem_watchpoints.find(changes->location);
        if (watchpoint != state.mem_watchpoints.end())
            lc3_break_eval(state, watchpoint->second);
    }
    else if (changes->changes == LC3_MULTI_CHANGE)
    {
        for (const auto& info : changes->info)
        {
            if (info.is_reg)
            {
                const auto& watchpoint = state.reg_watchpoints.find(info.location);
                if (watchpoint != state.reg_watchpoints.end())
                    lc3_break_eval(state, watchpoint->second);
            }
            else
            {
                const auto& watchpoint = state.mem_watchpoints.find(info.location);
                if (watchpoint != state.mem_watchpoints.end())
                    lc3_break_eval(state, watchpoint->second);
            }
        }
    }

    if (state.regs[7] != changes->r7)
    {
        const auto& watchpoint = state.reg_watchpoints.find(7);
        if (watchpoint != state.reg_watchpoints.end())
            lc3_break_eval(state, watchpoint->second);
    }

    return state.halted;
}
