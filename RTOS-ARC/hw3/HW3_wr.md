# HW3 Submission Requirements and Current Status

This document lists the **required deliverables** for HW3 and the **current status** in this repository.

## Global Requirements (applies to all programming problems)

- [ ] Code is in required language (Lustre / C / Python as specified)
- [ ] Required Makefiles exist and build on ARC
- [ ] Programs run on ARC
- [ ] Source is commented (comments are graded)
- [ ] README files include build/run commands and brief explanation
- [] Group Assignments (P5) requires Peer_Evaluation.txt

## Problem 1 (10 pts)

### Required submission
- [ ] `problem1.txt`

### Current status in repo
- [x] `problem1.txt` not found

## Problem 2 (20 pts)

### Required submission
- [ ] `rtsched.c`
- [ ] `Makefile2`
- [ ] `static.png`
- [ ] `README`

### Required behavior
- [ ] Build target produces executable `rtsched`
- [ ] Invocation format: `rtsched -static {inputFileName}`
- [ ] Uses static-priority scheduling via `SCHED_FIFO` and explicit thread scheduling attributes

### Current status in repo
- [x] Files exist: `hw3/p2/rtsched.c`, `hw3/p2/Makefile2`, `hw3/p2/README`
- [ ] All three existing files are currently empty (0 bytes)
- [ ] `static.png` not found

## Problem 3 (10 pts)

### Required submission
- [x] `verify4.lus`
- [ ] `README` (no Makefile)

### Required content
- [ ] Properties `ok1`, `ok2`, `ok3`, `ok4` encoded and checked with Luke
- [ ] Brief write-up of proof/disproof results and tool limitations

### Current status in repo
- [x] `hw3/p3/verify4.lus` exists and contains `ok1`/`ok2`/`ok3`/`ok4`
- [x] `hw3/p3/README2.md` exists with detailed Luke workflow/results
- [ ] Required filename says `README`; current file is named `README2.md` (rename or duplicate as `README` before submit)

## Problem 4 (30 pts)

### Required submission
- [ ] `cruisectr2.lus`
- [ ] `Makefile3` with targets `cruisectr2-sim` and `cruisectr2-exe`
- [ ] `README3`

### Required content
- [ ] PID controller in `controlSpeed()` with provided parameter/state structure
- [ ] Experimental coefficient tuning
- [ ] Old vs new controller comparison (CLI or Lucille evidence)

### Current status in repo
- [ ] `cruisectr2.lus` not found
- [ ] `Makefile3` not found
- [ ] `README3` not found

## Problem 5 (30 pts, group)

### Required submission
- [x] `my_automatic_control.py`
- [ ] `local_planner.py`
- [ ] `README4`

### Required content
- [ ] Custom `MyPID` agent path and PID parameter plumbing into planner
- [ ] Start location and destination exactly as specified
- [ ] Output format includes start time and final summary with time/invasions/collisions
- [ ] Penalty counters for collisions and lane invasions (excluding Broken lane markings)

### Current status in repo
- [x] `hw3/p5/my_automatic_control.py` exists
- [x] `hw3/p5/MyPID.py` exists
- [x] Start/destination coordinates are present in `my_automatic_control.py`
- [x] Start-time and final summary print statements are present
- [ ] `local_planner.py` not found in this HW3 folder
- [ ] `README4` not found
- [ ] Code prints `world.lane_invasion_sensor.penalty` and `world.collision_sensor.penalty`, but sensor classes currently define `lane_invasion_counter` / `collision_counter` (field mismatch to fix)

## Final Pre-Submission Checklist

- [ ] Ensure exact required filenames are present
- [ ] Ensure all required files are non-empty and runnable/buildable on ARC
- [ ] Resolve naming mismatch (`README2.md` vs required `README` for Problem 3)
- [ ] Resolve Problem 5 counter-field mismatch (`penalty` vs actual counter attribute names)
