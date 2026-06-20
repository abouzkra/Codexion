*This project has been created as part of the 42 curriculum by [abouzkra]*

# Description

**codexion** is a 42 school project in C that revolves arround designing a concurrent system similar to the dining philosophers problem, reimagined as coders (philosophers) competing for shared dongles (forks) and performing three actions: compiling(eating), debugging(thinking) and refactoring. They report their activity through a serialized logger, and supervised by a monitor thread that detects termination conditions (burnout(death), completion...). The final goal in this project is to create a simulation that should be:

- Deadlock free
- Race condition free
- Starvation resistant
- Precisely monitored with no interleaving outputs
- Resource leak free (data, mutexes, condition variables)

# Instructions

- **Making the project (codexion program):**
```bash
make
```

- **Object file cleanup:**
```bash
make clean
```

- **Full cleanup:**
```bash
make fclean
```

- **Rebuilding the project:**
```bash
make re
```

## Usage

- Run the simulation using the expected CLI parameters:

```bash
./codexion <coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <nb_compiles_required> <dongle_cooldown> <scheduler>
```

| Argument | Meaning |
| --- | --- |
| `number_of_coders` | Number of coder threads, and also the number of dongles |
| `time_to_burnout` | Max time (ms) a coder can go without starting a compile before burning out |
| `time_to_compile` | Time (ms) spent holding both dongles while compiling |
| `time_to_debug` | Time (ms) spent debugging |
| `time_to_refactor` | Time (ms) spent refactoring |
| `number_of_compiles_required` | Simulation ends successfully once every coder reaches this compile count |
| `dongle_cooldown` | Time (ms) a dongle stays unavailable after being released |
| `scheduler` | Arbitration policy for dongle access: `fifo` or `edf` |

All arguments are mandatory and validated; invalid input (negative numbers, non-integers, overflow, or an unknown scheduler) causes the program to reject the launch.

- Example

```bash
./codexion 2 1410 100 100 100 2 600 fifo
```

# Resources

- [The Linux Programming Interface](https://broman.dev/download/The%20Linux%20Programming%20Interface.pdf)
- [Understanding Concurrency and Multi-Threading](https://medium.com/@drajput_14416/understanding-concurrency-and-multi-threading-8dc7873e5818)
-

## AI Usage

- Understanding the project and clarifying rules and requirements
- Understanding multi-threaded programming concepts such as threads, concurrency, mutexes, condition variables, atomicity...
- Recommending possible solutions to problems that come with multi-threaded programming
- Refining this documentation with grammatically correct phrases

No AI-generated code was used in the final submission; all code was written and implemented manually. AI assistance was limited to **conceptual guidance, debugging and explanations**.

# Blocking Cases Handled

## Deadlock prevention

At initialization time (`init_coders`) each coder is assigned a `first_dongle` and `second_dongle`, and to break the classical circular wait deadlock, **odd-indexed coders pick up their dongles in reversed order** compared to even-indexed coder.

On top of that, `try_acquire_dongles` only commits taking both dongles if **both** are available at the same time; otherwise it releases the both, so a coder can never hold a dongle while wating on the ohter.

The single-coder edge case is handled separately: since a lone coder can't compile with only one dongle, he is simply put to sleep for `t_burnout`.

## Coffman's conditions

The four Coffman conditions required for deadlock are addressed as follows:

| Condition | How it's broken |
| --- | --- |
| Mutual exclusion | a dongle can only held by one coder |
| Hold and wait | a coder never *holds* a dongle while *waiting* for the other as explained in the previous section |
| No preemption | since hold-and-wait is handled, preemption isn't required to recover from a deadlock |
| Circular wait | broken by asymmetric acquisition order (odd vs even coders) |

## Starvation prevention

Each dongle keeps its own priority queue with 2 as max size (at most two coders can be adjacent to a dongle) implemented as a binary heap in `queue.c`. `has_priority` decides the ordering:

- **FIFO:** requests are served by insertion order
- **EDF:** the coder with the earliest deadline: `last_compile_start + t_burnout`
- **Tie breaker:** if the metrics are identical the coder with the least compile count is granted the dongle, if the compile count is identical the coder with lower id wins.

This grants a fully deterministic order with no infinite tie.

## Cooldown handling

`release_dongle` (`dongle.c`) sets `cooldown_ts = get_time_in_ms() + cooldown` when a dongle is released. `is_available` checks both that the dongle is unheld (`held_by == -1`) **and** that the cooldown timestamp has passed before allowing any coder to acquire it again. This is checked atomically under the dongle's mutex inside `try_acquire_dongles`, so no coder can sneak in and grab a dongle mid-cooldown.

## Burnout detection

A dedicated `monitor_routine` thread (`monitor.c`) polls every coder's `last_compile` timestamp every 500µs (`usleep(500)`) and compares `get_time_in_ms() - last_compile` against `t_burnout`. As soon as a coder exceeds the threshold, the monitor immediately sets `sim_state = OVER`, broadcasts the state-change condition variables (`sim_cond`, and every dongle's `cond` via `broadcast_dongles`) to wake any coder sleeping or polling for a dongle, then logs the burnout. The 500µs polling interval keeps the detection comfortably within the 10ms precision window required by the subject. The monitor checks burnout **before** checking for overall completion each loop, so a last-moment burnout is never masked by a simultaneous "all compiles done" state.

## Log serialization

All logging goes through `log_state` (`sim.c`) and `log_burned_out` (`monitor.c`), both of which lock a single shared `logger_mut` mutex around the `printf` call. Since every thread (coders and monitor) must acquire this same mutex before writing a line, two log lines can never interleave because each `printf` call completes atomically with respect to the others. `log_state` also re-checks `sim_is_over` after acquiring the lock to avoid printing stale state-change messages after the simulation has already ended.

# Thread Synchronization mechanism

| Primitive | Protects | Used in |
| --- | --- | --- |
| `sim_mut` + `sim_cond` | Global simulation state (`sim_state`), per-coder `compile_count` / `last_compile` | `sim.c`, `utils.c`, `monitor.c`, `coder.c` |
| `logger_mut` | Standard output, so no two log lines interleave | `sim.c`, `monitor.c` |
| `dongle->mut` + `dongle->cond` | Each dongle's `held_by`, `cooldown_ts`, and its 2-slot priority queue | `dongle.c`, `queue.c` |
| `sleep_cond` | sleeping coders is much more efficient as they wait on the condition variable | `utils.c` |

## Coordination Examples:

- **Race-free dongle state:**  data of a given dongle is only ever read or written while locking that dongle's `mut`, this prevents coders from thinking they both acquired the same dongle.
- **Atomic two-dongle acquisition:** `try_acquire_dongle` locks the first dongle's mutex checks priority and availability then locks the second dongle's mutex while still holding this first lock, this makes acquiring both dongles atomic.
- **Coder-Monitor communication:** the monitor knows what each coder is doing by only reading `last_compile` and `compile_count` under `sim_mut`, which coders also lock whenever they update those same fields.
- **Responsive shutdown:** instead of busy-polling for simulation end, sleeping coders wait on `sleep_cond` via `pthread_cond_timedwait` inside `coder_sleep`, and dongle-polling coders periodically check `sim_is_over`. When the monitor detects termination, it broadcasts `sim_cond` and every dongle's `cond` (`broadcast_dongles`) so that no thread is left blocked after the simulation ends.