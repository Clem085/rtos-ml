#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "mytime.h" /* PLUS_NTIME, etc. */

/*
 * This program launches a small set of periodic worker threads.
 * Each thread:
 *   - is pinned to CPU core 1
 *   - wakes up every 30 ms using an absolute release time
 *   - performs a short CPU-bound loop ("simulated work")
 *   - logs timestamps (start/end) into a shared buffer
 *
 * The output at the end is a normalized trace:
 *   <thread_index> <start_us_since_t0> <end_us_since_t0>
 *
 * Functionality is unchanged; this refactor is organizational/commentary only.
 */

/* ------------------------------- Constants ------------------------------- */

enum {
    kNumThreads = 3,          /* how many worker threads to spawn */
    kIterationsPerThread = 20 /* how many periods each thread runs */
};

#define LOG_SIZE 1000 /* must hold (2 * kNumThreads * kIterationsPerThread) */

/* --------------------------- Error handling ----------------------------- */

static void die_errno_ret(int ret, const char *what)
{
    /* Preserve existing behavior: print errno + ret, then perror and exit. */
    printf("errno %d ret %d ", errno, ret);
    fflush(stdout);
    perror(what);
    exit(EXIT_FAILURE);
}

static void die_set_errno_ret(int ret, const char *what)
{
    /* Preserve existing behavior: overwrite errno with ret before perror. */
    printf("errno %d ret %d ", errno, ret);
    fflush(stdout);
    errno = ret;
    perror(what);
    exit(EXIT_FAILURE);
}

/* ------------------------------ Data types ------------------------------ */

typedef struct {
    int thread_index;           /* 1-based ID printed in logs */
    int iterations;             /* number of periods to execute */
    struct timespec abs_start;  /* first absolute release time */
} thread_arg_t;

typedef struct {
    int thread_index;
    uint64_t timestamp_us;
} log_entry_t;

/* ------------------------------ Logging --------------------------------- */

static log_entry_t g_logbuf[LOG_SIZE];
static int g_log_count = 0;
static pthread_mutex_t g_log_mutex;

/* Monotonic clock in microseconds (used only for trace timestamps). */
static uint64_t monotonic_us_now(void)
{
    struct timespec tv;
    if (clock_gettime(CLOCK_MONOTONIC, &tv) != 0) {
        return 0;
    }
    return (uint64_t)tv.tv_sec * 1000000ULL + (uint64_t)(tv.tv_nsec / 1000ULL);
}

/*
 * Records a timestamp for a given thread_index.
 *
 * The original behavior is preserved:
 * - entries are stored in pairs (start/end)
 * - if consecutive calls are for the same thread, the most recent entry is updated
 * - if the buffer is full, the call silently returns
 */
static void log_timestamp(int thread_index)
{
    const uint64_t now = monotonic_us_now();

    int ret = pthread_mutex_lock(&g_log_mutex);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutex_lock");

    if (g_log_count >= LOG_SIZE - 1) {
        /* keep behavior: no logging once near capacity */
        (void)pthread_mutex_unlock(&g_log_mutex);
        return;
    }

    if (g_log_count == 0 || g_logbuf[g_log_count - 1].thread_index != thread_index) {
        g_logbuf[g_log_count].thread_index = thread_index;
        g_logbuf[g_log_count].timestamp_us = now;
        g_log_count++;
    } else {
        /* same thread as last entry: update timestamp in place */
        g_logbuf[g_log_count - 1].timestamp_us = now;
    }

    ret = pthread_mutex_unlock(&g_log_mutex);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutex_unlock");
}

/* --------------------------- Worker behavior ---------------------------- */

static void pin_current_thread_to_core_1(void)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);

    const int ret = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    if (ret != 0) die_set_errno_ret(ret, "pthread_setaffinity_np");
}

static void do_simulated_work(void)
{
    /* preserve the same loop and volatility as the original */
    volatile int work = 0;
    for (int j = 0; j < 100000; j++) {
        work += j;
    }
    (void)work;
}

static void *task_main(void *arg)
{
    thread_arg_t *targ = (thread_arg_t *)arg;

    printf("Thread %d started (tid=%ld)\n",
           targ->thread_index,
           (long)syscall(SYS_gettid));

    pin_current_thread_to_core_1();

    /* 30 ms period, absolute sleeps based on CLOCK_REALTIME (unchanged). */
    const struct timespec period = {.tv_sec = 0, .tv_nsec = 30 * 1000000L};
    struct timespec next_release = targ->abs_start;

    for (int i = 0; i < targ->iterations; i++) {
        const int ret = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &next_release, NULL);
        if (ret != 0) die_set_errno_ret(ret, "clock_nanosleep");

        /* Start timestamp */
        log_timestamp(targ->thread_index);

        do_simulated_work();

        /* End timestamp */
        log_timestamp(targ->thread_index);

        /* next_release += period */
        PLUS_NTIME(next_release, next_release, period);
    }

    printf("Thread %d exiting\n", targ->thread_index);
    return NULL;
}

/* ------------------------------- main() --------------------------------- */

static void init_log_mutex_with_pi(void)
{
    pthread_mutexattr_t mattr;

    int ret = pthread_mutexattr_init(&mattr);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutexattr_init");

    ret = pthread_mutexattr_setprotocol(&mattr, PTHREAD_PRIO_INHERIT);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutexattr_setprotocol");

    ret = pthread_mutex_init(&g_log_mutex, &mattr);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutex_init");

    ret = pthread_mutexattr_destroy(&mattr);
    if (ret != 0) die_set_errno_ret(ret, "pthread_mutexattr_destroy");
}

static struct timespec realtime_now_or_die(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return now;
}

static thread_arg_t *alloc_thread_arg_or_die(void)
{
    thread_arg_t *p = (thread_arg_t *)malloc(sizeof(*p));
    if (!p) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    return p;
}

static void print_log_trace(void)
{
    if (g_log_count <= 0) {
        return;
    }

    const uint64_t t0 = g_logbuf[0].timestamp_us;

    /* Preserve the original output format and stepping by 2. */
    for (int i = 0; i < g_log_count; i += 2) {
        const int idx = g_logbuf[i].thread_index;
        const uint64_t start = g_logbuf[i].timestamp_us - t0;
        const uint64_t end =
            (i + 1 < g_log_count) ? (g_logbuf[i + 1].timestamp_us - t0) : start;

        printf("%d %lu %lu\n", idx, start, end);
    }
}

int main(void)
{
    pthread_t threads[kNumThreads];
    thread_arg_t *targs[kNumThreads];

    init_log_mutex_with_pi();

    const struct timespec now = realtime_now_or_die();

    /* Spawn threads with a 10 ms stagger in first release time (unchanged). */
    for (int i = 0; i < kNumThreads; i++) {
        thread_arg_t *targ = alloc_thread_arg_or_die();
        targs[i] = targ;

        targ->thread_index = i + 1;
        targ->iterations = kIterationsPerThread;

        const struct timespec offset = {.tv_sec = 0, .tv_nsec = i * 10 * 1000000L};
        PLUS_NTIME(targ->abs_start, now, offset);

        const int ret = pthread_create(&threads[i], NULL, task_main, targ);
        if (ret != 0) die_set_errno_ret(ret, "pthread_create");
    }

    /* Join threads and free their args. */
    for (int i = 0; i < kNumThreads; i++) {
        (void)pthread_join(threads[i], NULL);
        free(targs[i]);
    }

    print_log_trace();
    return 0;
}
