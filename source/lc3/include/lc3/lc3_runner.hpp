#ifndef LC3_RUNNER_HPP
#define LC3_RUNNER_HPP

#include "lc3/lc3.hpp"

/** lc3_run
  *
  * Runs for X instructions or until the lc3 is halted.
  * @param state LC3State object.
  * @param num Number of instructions to execute.
  */
void LC3_API lc3_run(lc3_state& state, unsigned int num = -1);
/** lc3_step
  *
  * Executes one instruction.
  * @param state LC3State object.
  */
void LC3_API lc3_step(lc3_state& state);
/** lc3_back
  *
  * Attempts to backsteps (undoes) one instruction.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  */
void LC3_API lc3_back(lc3_state& state);
/** lc3_rewind
  *
  * Attempts to backstep a number of instructions.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  * @param num Number of instructions to back step (default max).
  */
void LC3_API lc3_rewind(lc3_state& state, unsigned int num = -1);
/** lc3_next_line
  *
  * Executes the next line and blackboxes any subroutines and traps.
  * @param state LC3State object.
  * @param num Maximum number of instructions to execute
  * @param depth Current subroutine depth in case num was provided
  * @return -1 if successfully executed previous line, otherwise the final subroutine depth if partially complete
  */
int LC3_API lc3_next_line(lc3_state& state, unsigned int num = -1, int depth = 0);
/** lc3_prev_line
  *
  * Goes back one line again blackboxes any subroutines and traps.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  * @param num Maximum number of instructions to execute
  * @param depth Current subroutine depth in case num was provided
  * @return -1 if successfully executed previous line, otherwise the final subroutine depth if partially complete
  */
int LC3_API lc3_prev_line(lc3_state& state, unsigned int num = -1, int depth = 0);
/** lc3_interrupt
  *
  * Checks for and processes a single pending interrupt.
  * @param state LC3State object.
  * @return true if an interrupt occurred.
  */
bool lc3_interrupt(lc3_state& state);
/** lc3_keyboard_interrupt
  *
  * Signals a keyboard interrupt.
  * @param state LC3State object.
  */
void LC3_API lc3_keyboard_interrupt(lc3_state& state);
/** lc3_signal_interrupt
  *
  * Adds an interrupt to be handled to the pending queue.
  * Will stack if called multiple times with the same priority and interrupt vector.
  * @param state LC3State object.
  * @param priority Priority of the interrupt.
  * @param vector Interrupt vector to be accessed when interrupt occurs.
  */
void LC3_API lc3_signal_interrupt(lc3_state& state, int priority, int vector);
/** lc3_signal_interrupt_once
  *
  * Adds an interrupt to be handled to the pending queue ONLY if it doesn't already exist.
  * @param state LC3State object.
  * @param priority Priority of the interrupt.
  * @param vector Interrupt vector to be accessed when interrupt occurs.
  * @return bool true if it was added false otherwise.
  */
bool LC3_API lc3_signal_interrupt_once(lc3_state& state, int priority, int vector);
/** lc3_signal_exception
  *
  * Signals an exception, exceptions are interrupts that are immediately processed.
  * @param state LC3State object.
  * @param vector Interrupt vector to be accessed when interrupt occurs.
  */
void LC3_API lc3_signal_exception(lc3_state& state, int vector);
/** lc3_check_keyboard_interrupt
  *
  * Checks if there is a character waiting and if we aren't in a keyboard interrupt
  * already and if so (with random jitter) signals a keyboard interrupt.
  * @param state LC3State object.
  */
void lc3_check_keyboard_interrupt(lc3_state& state);
/** lc3_tick_plugins
  *
  * Calls OnTick for all plugins.
  * This happens before the instruction is fetch.
  * @param state LC3State object.
  */
void lc3_tick_plugins(lc3_state& state);
/** lc3_tock_plugins
  *
  * Calls OnTock for all plugins.
  * This happens after the instruction is executed.
  * @param state LC3State object.
  */
void lc3_tock_plugins(lc3_state& state);


#endif
