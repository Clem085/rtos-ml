# HW3 Problem 2: Static Priority Scheduling

This program implements a simulation of static priority scheduling for real-time tasks. In static priority scheduling, each task is assigned a fixed priority level, and the scheduler always executes the highest-priority ready task. This implementation assumes tasks are provided in priority order (highest first) in the input file and executes them sequentially to simulate the schedule.

The program uses SCHED_FIFO (First-In-First-Out) scheduling policy on Linux, which is a real-time scheduling class where tasks run until they block or complete without preemption by lower-priority tasks.

## Solution Overview

The solution consists of a C program that:
1. Parses an input file containing task parameters
2. Simulates task execution using `usleep` for timing
3. Logs execution intervals for each task
4. Generates a Gantt chart visualization using gnuplot

### Key Data Structures

```c
typedef struct {
    int id;
    int period;
    int execution;
    int deadline;
    int priority;
} task_t;
```

Each task has an ID, period, execution time, deadline, and assigned priority.

### Execution Logging

The program logs the start and end times of each task execution:

```c
static void log_execution(int task_id, long long start, long long end) {
    pthread_mutex_lock(&log_mutex);
    if (log_count < LOG_SIZE) {
        logbuf[log_count].task_id = task_id;
        logbuf[log_count].start = start;
        logbuf[log_count].end = end;
        log_count++;
    }
    pthread_mutex_unlock(&log_mutex);
}
```

### Task Execution

Tasks are executed sequentially in the order they appear in the input file:

```c
for (int i = 0; i < num_tasks; i++) {
    long long start = get_time_us();
    usleep((useconds_t)tasks[i].execution * 1000U);
    long long end = get_time_us();
    log_execution(tasks[i].id, start, end);
}
```

## Input File Format

The input file should have the following format:
```
Start <start_time>
Stop <stop_time>

TaskID Phase Period Execution Deadline
<task1_id> <phase> <period> <execution> <deadline>
<task2_id> <phase> <period> <execution> <deadline>
...
```

- Start/Stop: Simulation time bounds (currently not used in execution)
- Tasks: Listed in priority order (highest priority first)
- Phase: Not used in this implementation
- Priorities are assigned as 99 - index (higher index = lower priority)

## Build Instructions

```bash
make -f Makefile2
```

## Run Instructions

```bash
./rtsched -static <input_file>
```

Example:
```bash
./rtsched -static sample_input.txt
```

## Output

The program generates:
- `gantt.dat`: Data file with execution segments for gnuplot
- `static.png`: Gantt chart visualization of the schedule

## Notes

- Run on ARC cluster for correct real-time behavior and SCHED_FIFO support
- Tasks are assumed to be schedulable and non-preemptive in this simulation
- The Gantt chart shows task execution over time in milliseconds
- Thread-safe logging is implemented using mutexes for potential future multi-threaded extensions