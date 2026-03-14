# Threads in C — A Comprehensive Guide

---

## Table of Contents

1. [What Are Threads? Threads vs Processes](#1-what-are-threads-threads-vs-processes)
2. [Race Conditions](#2-race-conditions)
3. [The `pthreads.h` Library — Comprehensive Guide](#3-the-pthreadsh-library--comprehensive-guide)

---

## 1. What Are Threads? Threads vs Processes

### What Is a Thread?

A **thread** (short for *thread of execution*) is the smallest unit of work that an operating system can schedule and run. Every program has at least one thread — the **main thread** — which starts when the program begins and ends when the program exits.

Threads live *inside* a process. A process can spawn multiple threads, all of which share the same memory space, file descriptors, and other OS-level resources belonging to that process, while each thread maintains its own:

- **Program counter** — tracks which instruction the thread is currently executing.
- **Stack** — stores local variables, function call frames, and return addresses.
- **Registers** — CPU state specific to that thread.
- **Thread ID** — a unique identifier assigned by the OS.

Because threads share the same address space as their parent process, they can read and write to the same global variables and heap-allocated memory directly — which is both their greatest strength and their most dangerous characteristic.

### What Is a Process?

A **process** is an independent program in execution. The OS gives each process its own isolated virtual address space, its own file descriptor table, and its own system resources. Processes do not share memory with each other by default — inter-process communication (IPC) requires explicit mechanisms like pipes, sockets, shared memory segments, or message queues.

When a process is forked (via `fork()`), the child receives a **copy** of the parent's memory (copy-on-write), not a reference to it.

### Why Use Threads?

- **Parallelism**: Split CPU-intensive work across multiple cores.
- **Concurrency**: Keep a program responsive (e.g., a server handling many clients simultaneously).
- **Efficiency**: Threads are cheaper to create and destroy than processes, and they communicate faster through shared memory.
- **Responsiveness**: In UI or I/O-heavy programs, a background thread can do work without freezing the main thread.

### Visual: Process with Multiple Threads

```
┌──────────────────────────────────────────────────┐
│                      PROCESS                     │
│                                                  │
│  ┌────────────┐  ┌────────────┐  ┌────────────┐  │
│  │  Thread 1  │  │  Thread 2  │  │  Thread 3  │  │
│  │ (main)     │  │            │  │            │  │
│  │ own stack  │  │ own stack  │  │ own stack  │  │
│  │ own PC     │  │ own PC     │  │ own PC     │  │
│  └────────────┘  └────────────┘  └────────────┘  │
│                                                  │
│            Shared: heap, globals, file           │
│             descriptors, code segment            │
└──────────────────────────────────────────────────┘
```

---

## 2. Race Conditions

### Definition

A **race condition** occurs when two or more threads access shared data concurrently and the final outcome depends on the **order** or **timing** of their execution — an order that is non-deterministic and controlled by the OS scheduler.

Race conditions are insidious bugs: they may not manifest every time the program runs, only appearing under specific timing circumstances (high load, a certain number of cores, a particular OS scheduler decision), making them extremely hard to reproduce and debug.

### A Classic Example

Consider a global counter being incremented by two threads:

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;  // Shared global variable

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // NOT atomic — this is 3 operations under the hood!
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Expected: 2000000 — Actual: unpredictable!
    printf("Counter: %d\n", counter);
    return 0;
}
```

**Why is `counter++` dangerous?** It looks like one operation, but at the machine level it compiles to three distinct steps:

```
1. LOAD  — read the value of counter from memory into a CPU register
2. ADD   — increment the value in the register by 1
3. STORE — write the new value back to memory
```

If two threads interleave these steps, increments get lost:

```
Thread 1: LOAD  counter → gets 42
Thread 2: LOAD  counter → gets 42   (reads before Thread 1 stores!)
Thread 1: ADD   42 + 1 = 43
Thread 2: ADD   42 + 1 = 43
Thread 1: STORE 43 → counter is now 43
Thread 2: STORE 43 → counter is now 43  (one increment was lost!)
```

Two threads did work, but the counter only moved from 42 to 43 instead of 44.

### Critical Sections

A **critical section** is any block of code that accesses shared data and must not be executed by more than one thread at a time. Protecting critical sections is the central challenge of multithreaded programming.

Race conditions arise when critical sections are left **unprotected**.

### Data Races vs Race Conditions

These terms are related but not identical:

- A **data race** is a low-level, well-defined situation: two threads access the same memory location concurrently, at least one access is a write, and there is no synchronization between them. Data races are undefined behavior in C.
- A **race condition** is a broader, higher-level concept: the program's correctness depends on non-deterministic ordering, even if individual memory accesses are technically safe.

All data races are race conditions, but not all race conditions involve data races (e.g., TOCTOU — time-of-check-to-time-of-use — bugs can be race conditions without being data races).

### How to Prevent Race Conditions

The solution is **synchronization** — using OS-provided primitives to enforce ordering and mutual exclusion:

| Mechanism | Purpose |
|---|---|
| **Mutex** | Ensures only one thread enters a critical section at a time |
| **Semaphore** | Controls access to a resource with a counter |
| **Condition variable** | Lets threads wait for a specific condition to become true |
| **Atomic operations** | Hardware-level guarantees for simple read-modify-write ops |
| **Read-write locks** | Allow concurrent reads but exclusive writes |

These are all covered in detail in the `pthreads.h` section below.

---

## 3. The `pthreads.h` Library — Comprehensive Guide

### Overview

`pthreads` (POSIX Threads) is the standard threading API for Unix-like systems (Linux, macOS, BSD). It is defined by the POSIX standard (IEEE Std 1003.1) and provides a complete set of functions for creating threads, synchronizing them, and managing their lifecycle.

To use it, include the header and link against the library at compile time:

```c
#include <pthread.h>
```

```bash
gcc my_program.c -o my_program -lpthread
```

---

### 3.1 Thread Lifecycle

#### Creating a Thread — `pthread_create`

```c
int pthread_create(
    pthread_t *thread,            // Output: the new thread's ID
    const pthread_attr_t *attr,   // Thread attributes (NULL = defaults)
    void *(*start_routine)(void*),// The function the thread will execute
    void *arg                     // Argument passed to start_routine
);
```

- Returns `0` on success, an error code on failure.
- The new thread starts executing `start_routine(arg)` immediately after creation.
- The thread function must have the signature `void *func(void *arg)`.

**Example:**

```c
#include <stdio.h>
#include <pthread.h>

void *say_hello(void *arg) {
    char *name = (char *)arg;
    printf("Hello from thread: %s\n", name);
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, say_hello, "Worker-1");
    pthread_join(tid, NULL);
    return 0;
}
```

#### Waiting for a Thread — `pthread_join`

```c
int pthread_join(pthread_t thread, void **retval);
```

- Blocks the calling thread until `thread` finishes.
- `retval` receives the value returned by `start_routine` (can be `NULL` if you don't care).
- A thread that is not joined is called **detached** — its resources are not freed until explicitly detached or the process exits.

#### Detaching a Thread — `pthread_detach`

```c
int pthread_detach(pthread_t thread);
```

Use this when you don't need to `join` the thread and want its resources to be automatically cleaned up on exit:

```c
pthread_detach(tid);  // Fire and forget
```

A thread can also detach itself:

```c
void *worker(void *arg) {
    pthread_detach(pthread_self());
    // ... do work ...
    return NULL;
}
```

#### Getting the Current Thread ID — `pthread_self`

```c
pthread_t pthread_self(void);
```

Returns the thread ID of the calling thread.

#### Terminating a Thread — `pthread_exit`

```c
void pthread_exit(void *retval);
```

Terminates the calling thread and returns `retval` to a waiting `pthread_join`. Calling this from `main()` terminates the main thread without killing other threads (unlike `return` or `exit()`).

---

### 3.2 Mutexes — `pthread_mutex_t`

A **mutex** (mutual exclusion lock) guarantees that only one thread at a time can execute the critical section it protects.

#### Initialization and Destruction

```c
// Static initialization (for global/static mutexes):
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Dynamic initialization:
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);  // NULL = default attributes

// Destroy when no longer needed:
pthread_mutex_destroy(&lock);
```

#### Locking and Unlocking

```c
pthread_mutex_lock(&lock);    // Block until the lock is acquired
// --- critical section ---
pthread_mutex_unlock(&lock);  // Release the lock
```

```c
// Non-blocking try:
int ret = pthread_mutex_trylock(&lock);
if (ret == 0) {
    // Got the lock
    pthread_mutex_unlock(&lock);
} else {
    // Lock was already held by another thread — do something else
}
```

#### Full Example — Fixing the Race Condition

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&lock);
        counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Counter: %d\n", counter);  // Always 2000000
    pthread_mutex_destroy(&lock);
    return 0;
}
```

#### Deadlocks

A **deadlock** occurs when two or more threads are each waiting for a lock held by the other, forming a cycle of waiting that never resolves.

```
Thread 1 holds Lock A, waits for Lock B
Thread 2 holds Lock B, waits for Lock A
→ Neither can proceed — deadlock!
```

**Rules to avoid deadlocks:**
- Always acquire multiple locks in a **consistent global order**.
- Use `pthread_mutex_trylock` with backoff to break potential cycles.
- Keep critical sections as short as possible.
- Avoid calling functions that may acquire locks while already holding one (lock ordering becomes hard to track).

---

### 3.3 Condition Variables — `pthread_cond_t`

A **condition variable** allows threads to efficiently wait for a specific condition to become true, rather than busy-spinning. Condition variables are **always used together with a mutex**.

#### Initialization and Destruction

```c
// Static:
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// Dynamic:
pthread_cond_t cond;
pthread_cond_init(&cond, NULL);
pthread_cond_destroy(&cond);
```

#### Core Functions

```c
// Atomically release the mutex and block until signaled:
pthread_cond_wait(&cond, &mutex);

// Wake up ONE waiting thread:
pthread_cond_signal(&cond);

// Wake up ALL waiting threads:
pthread_cond_broadcast(&cond);
```

> ⚠️ **Spurious wakeups**: `pthread_cond_wait` can return even without a signal being sent (a known behavior on some systems). Always re-check the condition in a `while` loop, never an `if`.

#### Example — Producer / Consumer

```c
#include <stdio.h>
#include <pthread.h>

int data_ready = 0;
int data = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    pthread_mutex_lock(&lock);
    data = 42;
    data_ready = 1;
    printf("Producer: data produced\n");
    pthread_cond_signal(&cond);        // Wake up the consumer
    pthread_mutex_unlock(&lock);
    return NULL;
}

void *consumer(void *arg) {
    pthread_mutex_lock(&lock);
    while (!data_ready) {              // Loop — guard against spurious wakeups
        pthread_cond_wait(&cond, &lock);
    }
    printf("Consumer: received data = %d\n", data);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void) {
    pthread_t prod, cons;
    pthread_create(&cons, NULL, consumer, NULL);
    pthread_create(&prod, NULL, producer, NULL);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
```

---

### 3.4 Read-Write Locks — `pthread_rwlock_t`

A **read-write lock** optimizes for the common case where data is read far more often than it is written. It allows **multiple simultaneous readers** but enforces **exclusive access for writers**.

```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

// OR:
pthread_rwlock_init(&rwlock, NULL);
pthread_rwlock_destroy(&rwlock);
```

```c
pthread_rwlock_rdlock(&rwlock);   // Acquire for reading (shared)
// ... read data ...
pthread_rwlock_unlock(&rwlock);

pthread_rwlock_wrlock(&rwlock);   // Acquire for writing (exclusive)
// ... modify data ...
pthread_rwlock_unlock(&rwlock);
```

Use read-write locks for data structures like caches, configuration tables, or any resource that is heavily read but rarely mutated.

---

### 3.5 Semaphores — `semaphore.h`

While not strictly part of `pthreads.h`, POSIX semaphores (`sem_t` from `<semaphore.h>`) complement pthreads and are commonly used alongside them.

A **semaphore** maintains an integer counter and blocks threads when the counter reaches zero:

```c
#include <semaphore.h>

sem_t sem;
sem_init(&sem, 0, 1);  // pshared=0 (thread-local), initial value=1

sem_wait(&sem);   // Decrement counter; block if counter == 0
// --- critical section ---
sem_post(&sem);   // Increment counter; wake a waiting thread

sem_destroy(&sem);
```

- A semaphore initialized to `1` behaves like a mutex.
- A semaphore initialized to `N` allows up to `N` threads to enter concurrently — useful for resource pooling (e.g., a pool of N database connections).

---

### 3.6 Thread Attributes — `pthread_attr_t`

Thread behavior can be customized before creation using a `pthread_attr_t` object:

```c
pthread_attr_t attr;
pthread_attr_init(&attr);

// Set stack size (in bytes):
pthread_attr_setstacksize(&attr, 2 * 1024 * 1024);  // 2 MB

// Create as detached (no need to call pthread_join):
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

pthread_create(&tid, &attr, worker_func, NULL);
pthread_attr_destroy(&attr);  // Always destroy after use
```

Common attribute setters:

| Function | Description |
|---|---|
| `pthread_attr_setstacksize` | Set the thread's stack size |
| `pthread_attr_setdetachstate` | `PTHREAD_CREATE_JOINABLE` (default) or `PTHREAD_CREATE_DETACHED` |
| `pthread_attr_setschedpolicy` | Scheduling policy (`SCHED_FIFO`, `SCHED_RR`, `SCHED_OTHER`) |
| `pthread_attr_setschedparam` | Set scheduling priority |

---

### 3.7 Thread-Local Storage — `__thread`

Sometimes you want a variable that is **global in scope but private per thread** — each thread gets its own copy. Use the `__thread` storage class specifier (GCC/Clang extension, standardized in C11 as `_Thread_local`):

```c
__thread int thread_error_code = 0;  // Each thread has its own copy

void *worker(void *arg) {
    thread_error_code = 42;  // Only affects this thread's copy
    return NULL;
}
```

This is how the standard C library implements `errno` — each thread has its own `errno` value to avoid race conditions in error reporting.

---

### 3.8 Common Pitfalls & Best Practices

| Pitfall | Description | Fix |
|---|---|---|
| **Forgetting to join/detach** | Thread resources leak | Always `pthread_join` or `pthread_detach` every thread |
| **Unlocking in error paths** | Lock held forever if function returns early | Use `goto cleanup` pattern or RAII-style wrappers |
| **Locking too much** | Serializes work, kills parallelism | Keep critical sections minimal |
| **Locking too little** | Race conditions and data corruption | Audit every shared variable access |
| **Deadlock** | Circular lock dependency | Enforce consistent lock ordering |
| **Stack overflow** | Default stack is small (~8 MB) | Increase with `pthread_attr_setstacksize` for deep recursion |
| **Passing stack addresses to threads** | The variable may go out of scope | Use heap allocation (`malloc`) for arguments passed to threads |

**Passing arguments to threads safely:**

```c
// WRONG — `i` may have changed by the time the thread reads it
for (int i = 0; i < 4; i++) {
    pthread_create(&tids[i], NULL, worker, &i);  // Dangerous!
}

// CORRECT — allocate a separate copy for each thread
for (int i = 0; i < 4; i++) {
    int *arg = malloc(sizeof(int));
    *arg = i;
    pthread_create(&tids[i], NULL, worker, arg);  // Thread frees it
}
```
