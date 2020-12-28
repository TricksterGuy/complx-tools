#ifndef EXECUTE_OPTIONS_HPP
#define EXECUTE_OPTIONS_HPP

enum class RunMode
{
    INVALID = 0,
    STEP = 1,
    BACK = 2,
    NEXT_LINE = 3,
    PREV_LINE = 4,
    RUN = 5,
    FINISH = 6,
    REWIND = 7,
    RUNX = 8,
};

struct ExecuteOptions
{
    RunMode mode = RunMode::INVALID;
    unsigned long instructions = 1;
    unsigned long ips = 65536;
    unsigned long fps = 60;
};

struct ExecutionInfo
{
    ExecutionInfo(const ExecuteOptions& opts) : options(opts) {}
    ExecuteOptions options;
    double count = 0;
};

#endif
