Markdown

*This project has been created as part of the 42 curriculum by vlourenc.*

# codexion

An advanced multi-threaded concurrency simulation in C modeling resource sharing, contention, and real-time task scheduling in a collaborative development hub.

---

## Description

The **codexion** project simulates a circular co-working space where multiple developers (`coders`) share a limited pool of specialized USB dongles required to compile quantum code. Each coder alternately enters three states: **compiling**, **debugging**, and **refactoring**.

Because quantum compilation requires two dongles simultaneously (one in each hand: the coder's left and right dongles), coders must compete for shared hardware without centralized control. If a coder cannot acquire both dongles and start compiling within `time_to_burnout` milliseconds, they suffer a fatal **burnout**, terminating the simulation.

The objective is to orchestrate thread concurrency, prevent deadlocks and starvation, enforce hardware cooldown intervals, and arbitrate competing requests using a custom Min-Heap Priority Queue running either **FIFO** (First-In, First-Out) or **EDF** (Earliest Deadline First) scheduling policies.

---

## Instructions

### Compilation

The project complies with 42's Norminette (C99 standard, functions $\le$ 25 lines, maximum 5 functions per file, zero global variables) and compiles with `cc` using the mandatory flags:

	make        # Compiles the codexion executable
	make clean  # Removes object files (*.o)
	make fclean # Removes object files and the codexion binary
	make re     # Cleans and recompiles the entire project

## Execution & Arguments
The program must be run with exactly 8 mandatory arguments:Bash./codexion <number_of_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <number_of_compiles_required> <dongle_cooldown> <scheduler>

## Blocking Cases Handled
The implementation addresses all classical concurrency and real-time synchronization hazards:
1. Deadlock Prevention & Coffman's ConditionsTo eliminate deadlocks, the four Coffman conditions are analyzed and broken:Circular Wait: Coders always acquire their two required dongles in strict order of their unique hardware IDs (min(left, right) followed by max(left, right)). This asymmetric ordering prevents cyclic dependency loops across the ring.Single Coder Edge Case: When number_of_coders == 1, the coder acquires their solitary dongle, emits the log, and waits passively for burnout without attempting to double-lock the same mutex (preventing self-deadlock).Hold and Wait: Coders only retain dongle locks during active state evaluations and compilation, releasing mutexes during idle cooldown intervals.
2. Starvation PreventionUnder high contention, threads are prevented from indefinitely overtaking peers through individual Min-Heap Priority Queues attached to each dongle:Simultaneous Queue Registration: When attempting to compile, a coder registers their request in both queues simultaneously with a synchronized timestamp/deadline, guaranteeing fair visibility.Under edf, coders closest to burnout automatically gain priority, preventing starvation of critical threads.
3. Cooldown HandlingWhen a coder finishes compiling and enters debugging, both dongles are timestamped with cooldown_until = current_time + dongle_cooldown. While in cooldown:Waiting coders release the dongle mutex (pthread_mutex_unlock) and execute a non-blocking precise_sleep for the remaining cooldown duration. This avoids busy-waiting and starvation of CPU cores.
4. Precise Burnout DetectionA dedicated background monitor thread continuously samples all coders' health:Burnout is evaluated using strict inequality (current_time - last_compile_start > time_to_burnout).When burnout occurs, the monitor triggers stop_simulation, prints the fatal message within the required 10 ms tolerance window, and broadcasts to all dongle condition variables to immediately release waiting threads.
5. Log SerializationAll standard output is strictly wrapped within pthread_mutex_lock(&sim->log_mutex). This guarantees that:No two status messages interleave on a single line.No further state changes are printed after the simulation has stopped.


## Thread Synchronization Mechanisms
### Primitives Used
pthread_mutex_t:

	dongle->mutex: Guarantees exclusive ownership and atomic inspection of each dongle's state and queue.

	coder->coder_muted: Protects thread-safe concurrent reads/writes to last_compile_start and compile_count.

	sim->finish_mutex: Protects the atomic state flag is_finished.
	
	sim->log_mutex: Serializes all writes to stdout.


pthread_cond_t:

	dongle->cond: Coordinates blocking and wake-up notifications for threads queued on a specific dongle.
	
### Resource Coordination & Race Condition Prevention

Thread-Safe Coder/Monitor Communication: When a coder starts compiling, it updates last_compile_start inside a coder_muted critical section. The monitor thread locks the same mutex before computing elapsed time, eliminating race conditions and dirty reads.

Condition Variable Broadcasting: Upon releasing a dongle in drop_dongles, the thread broadcasts (pthread_cond_broadcast(&dongle->cond)) to wake up all queued peers, prompting them to re-evaluate queue head positions and cooldown expirations.

Hang-Free Termination: When stop_simulation is called, all dongle condition variables are broadcasted so that threads sleeping on pthread_cond_wait wake up, evaluate is_simulation_over(), and terminate cleanly into pthread_join.

## Resources

Classic References

POSIX Threads: IEEE Std 1003.1 (POSIX.1-2017) pthreads specification

Concurrency & Deadlocks: Coffman, E. G., Elphick, M., & Shoshani, A. (1971). System Deadlocks. Computing Surveys (CSUR).

Dining Philosophers Problem: Dijkstra, E. W. (1965). Hierarchical ordering of sequential processes.

Real-Time Scheduling: Liu, C. L., & Layland, J. W. (1973). Scheduling Algorithms for Multiprogramming in a Hard-Real-Time Environment (Earliest Deadline First analysis).

Binary Heaps & Priority Queues: Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. Introduction to Algorithms (CLRS), Chapter 6.

## AI Usage Statement

In compliance with 42 curriculum guidelines regarding Generative AI:

Debugging Race Conditions: AI assisted in tracking edge-case deadlocks related to lost condition variable notifications during simulation startup and starvation scenarios under large cooldowns.

Refactoring: AI was employed to assist in restructuring helper routines to comply with 42's Norminette (25-line limit per function and 5 functions per file) while preserving thread-safety invariants.

All AI suggestions were reviewed, peer-tested, and independently validated through debugging and automated test runs.