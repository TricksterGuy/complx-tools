#ifndef LC3_REPLAY_HPP
#define LC3_REPLAY_HPP

#include <lc3.hpp>

#include <exception>
#include <sstream>
#include <string>

/** Exception class for replay string errors */
class LC3ReplayStringException : public std::exception
{
public:
    /** Constructor
      *
      * @param str string causing the error.
      */
    LC3ReplayStringException(const std::string& replay_str, const std::string& msg) noexcept : replay(replay_str), message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string replay;
    std::string message;
};


void lc3_setup_replay(lc3_state& state, const std::string& filename, const std::string& replay_string, std::stringstream& newinput);
std::string lc3_describe_replay(const std::string& replay_string);

void lc3_run_verification(lc3_state& state, const std::string& verification_string);
std::string lc3_describe_verification(const std::string& verification_string);

#endif
