#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_TASKS 10
#define LOG_SIZE 1000

typedef struct {
    int id;
    int period;
    int execution;
    int deadline;
    int priority;
} task_t;

typedef struct {
    int task_id;
    long long start;
    long long end;
} log_entry_t;

static log_entry_t logbuf[LOG_SIZE];
static int log_count = 0;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

static long long get_time_us(void) {
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        perror("gettimeofday");
        return 0;
    }
    return (long long)tv.tv_sec * 1000000LL + tv.tv_usec;
}

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

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-static") != 0) {
        fprintf(stderr, "Usage: %s -static <input_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[2], "r");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    task_t tasks[MAX_TASKS];
    int num_tasks = 0;
    char line[256];
    int start_time, stop_time;

    if (!fgets(line, sizeof(line), fp) || sscanf(line, "Start %d", &start_time) != 1) {
        fprintf(stderr, "Error: invalid or missing Start line\n");
        fclose(fp);
        return 1;
    }
    if (!fgets(line, sizeof(line), fp) || sscanf(line, "Stop %d", &stop_time) != 1) {
        fprintf(stderr, "Error: invalid or missing Stop line\n");
        fclose(fp);
        return 1;
    }
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "Error: missing blank line after Stop\n");
        fclose(fp);
        return 1;
    }
    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "Error: missing task header line\n");
        fclose(fp);
        return 1;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        int task_id, phase;

        if (num_tasks >= MAX_TASKS) {
            fprintf(stderr, "Warning: reached MAX_TASKS=%d, remaining tasks ignored\n", MAX_TASKS);
            break;
        }

        if (sscanf(line, "%d %d %d %d %d", &task_id, &phase, &tasks[num_tasks].period,
                   &tasks[num_tasks].execution, &tasks[num_tasks].deadline) == 5) {
            tasks[num_tasks].id = task_id;
            tasks[num_tasks].priority = 99 - num_tasks;
            num_tasks++;
        }
    }
    fclose(fp);

    if (num_tasks == 0) {
        fprintf(stderr, "Error: no tasks parsed from input file\n");
        return 1;
    }

    for (int i = 0; i < num_tasks; i++) {
        long long start = get_time_us();
        usleep((useconds_t)tasks[i].execution * 1000U);
        long long end = get_time_us();
        log_execution(tasks[i].id, start, end);
    }

    FILE *gantt_fp = fopen("gantt.dat", "w");
    if (!gantt_fp) {
        perror("fopen gantt.dat");
        return 1;
    }

    long long t0 = logbuf[0].start;
    int max_task_id = 0;

    for (int i = 0; i < log_count; i++) {
        long long rel_start = logbuf[i].start - t0;
        long long rel_end = logbuf[i].end - t0;

        /* Two points plus a blank line define one independent horizontal segment. */
        fprintf(gantt_fp, "%lld %d\n", rel_start, logbuf[i].task_id);
        fprintf(gantt_fp, "%lld %d\n\n", rel_end, logbuf[i].task_id);

        if (logbuf[i].task_id > max_task_id) {
            max_task_id = logbuf[i].task_id;
        }
    }
    fclose(gantt_fp);

    char gnuplot_cmd[2048];
    snprintf(
        gnuplot_cmd, sizeof(gnuplot_cmd),
        "gnuplot -e '"
        "set terminal pngcairo enhanced font \"Arial,11\" size 1400,500 background rgb \"white\"; "
        "set output \"static.png\"; "
        "set title \"Static-Priority Schedule (Gantt)\"; "
        "set xlabel \"Time (ms)\"; "
        "set ylabel \"Task ID\"; "
        "set grid xtics ytics; "
        "set ytics 1; "
        "set yrange [0.5:%d.5]; "
        "unset key; "
        "plot \"gantt.dat\" using ($1/1000.0):2 "
        "with lines lw 18 lc rgb \"#1f77b4\"'",
        max_task_id > 0 ? max_task_id : 1);

    int rc = system(gnuplot_cmd);
    if (rc != 0) {
        fprintf(stderr, "Error: gnuplot failed (exit code %d)\n", rc);
        return 1;
    }

    fprintf(stdout, "Generated gantt.dat and static.png\n");
    return 0;
}
