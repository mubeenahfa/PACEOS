# PACEOS

```text
██████╗  █████╗  ██████╗███████╗ ██████╗ ███████╗
██╔══██╗██╔══██╗██╔════╝██╔════╝██╔═══██╗██╔════╝
██████╔╝███████║██║     █████╗  ██║   ██║███████╗
██╔═══╝ ██╔══██║██║     ██╔══╝  ██║   ██║╚════██║
██║     ██║  ██║╚██████╗███████╗╚██████╔╝███████║
╚═╝     ╚═╝  ╚═╝ ╚═════╝╚══════╝ ╚═════╝ ╚══════╝

        Periodic Application Cooperative Execution Scheduler
```

> Tiny. Deterministic. Allocation-free.  
> A cooperative task scheduler for bare-metal embedded C.

![Language](https://img.shields.io/badge/language-C-555555)
![Type](https://img.shields.io/badge/type-cooperative_scheduler-555555)
![Allocation](https://img.shields.io/badge/heap-none-555555)
![Status](https://img.shields.io/badge/status-experimental-orange)

---

## `> what is paceos`

PACEOS is a lightweight cooperative scheduler designed for small embedded systems.

It provides periodic task execution, priorities, task activation/deactivation, task removal, and callback context passing without requiring dynamic memory allocation, threads, or an RTOS.

The entire scheduler is built around a simple idea:

```text
tick
 │
 ▼
scheduler_run()
 │
 ├── find ready tasks
 ├── select according to priority
 └── execute callback
```

PACEOS is currently a learning and experimentation project focused on low-level C, embedded scheduling, API design, and deterministic execution.

---

## `> features`

```text
[+] Fixed-size task table
[+] Periodic task execution
[+] Priority-based scheduling
[+] Callback functions
[+] User-defined callback context
[+] Runtime activation / deactivation
[+] Task removal
[+] Reusable task slots
[+] Explicit scheduler status codes
[+] No heap allocation
[+] Unit tests

[ ] Generation-safe task handles
[ ] Argument validation
[ ] Multiple ready-task execution
[ ] One-shot tasks
[ ] Runtime scheduler statistics
[ ] Hardware timer integration
```

---

## `> architecture`

```text
                        ┌───────────────────┐
                        │   Application     │
                        └─────────┬─────────┘
                                  │
                         add_task(...)
                                  │
                                  ▼
                    ┌─────────────────────────┐
                    │        PACEOS API       │
                    │                         │
                    │  add_task()             │
                    │  remove_task()          │
                    │  activate_task()        │
                    │  deactivate_task()      │
                    │  scheduler_run()        │
                    └────────────┬────────────┘
                                 │
                                 ▼
                  ┌─────────────────────────────┐
                  │       Scheduler Core        │
                  │                             │
                  │   fixed task table          │
                  │   priority selection        │
                  │   periodic timing           │
                  │   task state management     │
                  └──────────────┬──────────────┘
                                 │
                                 ▼
                         task_callback(ctx)
```

Tasks are represented internally by the scheduler. Applications interact with them through handles rather than accessing scheduler state directly.

---

## `> example`

```c
#include "scheduler.h"

void blink_led(void *context)
{
    // Toggle GPIO
}

void sample_sensor(void *context)
{
    // Read ADC
}

int main(void)
{
    scheduler_init();

    add_task(blink_led, NULL, 500, 2);
    add_task(sample_sensor, NULL, 10, 1);

    while (1)
    {
        scheduler_run();
    }
}
```

A hardware timer or SysTick interrupt can increment the scheduler tick:

```c
void timer_interrupt(void)
{
    increment_tick();
}
```

Which gives us something conceptually like:

```text
TIME ──────────────────────────────────────────────>

sensor   █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █ █
LED      █                   █                   █

priority
   1       sensor
   2       LED
```

---

## `> design philosophy`

PACEOS follows a few deliberately boring rules.

### No dynamic allocation

```text
malloc()  -> forbidden
free()    -> unnecessary
```

Tasks live inside a statically allocated task table.

This keeps memory usage predictable and makes the scheduler suitable for small bare-metal targets.

### Cooperative execution

PACEOS does not preempt tasks.

A callback runs until it returns.

```text
scheduler
    │
    ├── task A ────────────┐
    │                      │ return
    ◄──────────────────────┘
    │
    ├── task B ─────┐
    │                │ return
    ◄────────────────┘
```

Therefore:

> Tasks should be short, bounded, and non-blocking.

### Explicit state

The scheduler does not hide timing or task state behind threads, operating-system primitives, or background services.

What runs is determined entirely by:

```text
period
priority
active state
scheduler tick
```

---

## `> task model`

Internally, a task contains roughly:

```c
typedef struct {
    task_callback callback;
    void *context;

    uint32_t period_ticks;
    uint32_t lastcall_tick;

    uint8_t priority;
    bool active;
} Task;
```

The scheduler owns these structures.

Applications receive a task handle when registering a task.

---

## `> priorities`

PACEOS uses numeric priorities.

```text
lower number = higher priority

priority 0  ██████████
priority 1  ████████
priority 2  ██████
priority 3  ████
priority 4  ██
```

When multiple tasks are ready, the scheduler uses task priority to decide which task should execute.

Priority semantics are still evolving while the scheduler API is being developed.

---

## `> repository`

```text
PACEOS/
│
├── include/
│   └── scheduler.h
│
├── source/
│   └── scheduler.c
│
├── tests/
│   └── test_scheduler.c
│
└── README.md
```

Planned:

```text
examples/
Makefile
LICENSE
.gitignore
```

---

## `> build`

For now, the scheduler can be built directly with GCC.

```bash
gcc \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Iinclude \
    source/scheduler.c \
    tests/test_scheduler.c \
    -o test_scheduler
```

Run:

```bash
./test_scheduler
```

Recommended debug build:

```bash
gcc \
    -std=c11 \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Wconversion \
    -g \
    -Iinclude \
    source/scheduler.c \
    tests/test_scheduler.c \
    -o test_scheduler
```

---

## `> current status`

```text
PACEOS VERSION: 0.1
STATE         : EXPERIMENTAL
API           : UNSTABLE
TARGET        : BARE-METAL C
HEAP          : 0 BYTES
THREADS       : 0
MAGIC         : MINIMAL
```

PACEOS is under active development.

APIs may change while task semantics, handle safety, scheduling behavior, and hardware integration are being explored.

---

## `> why`

Because this:

```c
while (1)
{
    do_everything();
}
```

eventually becomes this:

```text
"why is my ADC sampling late"
"why did UART block everything"
"why is this LED timing somehow load-bearing"
```

PACEOS exists as an experiment in replacing increasingly cursed super-loops with a tiny explicit scheduler.

---

## `> license`

PACEOS is released under the [MIT License](LICENSE).

```text
use it.
modify it.
ship it.
just keep the copyright notice.

---

```text
$ ./paceos

scheduler initialized
heap usage:        0 bytes
tasks registered:  ready
preemption:        absolutely not
status:            pacing
_
```
