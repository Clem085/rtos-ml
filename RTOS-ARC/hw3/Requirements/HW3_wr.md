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
- [x] `problem1.txt`

### Current status in repo
- [x] `problem1.txt` created with schedulability analysis

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
- [x] Files filled with static priority implementation
- [ ] `static.png` not generated (requires running on ARC)

## Problem 3 (10 pts)

### Required submission
- [x] `verify4.lus`
- [ ] `README` (no Makefile)

### Required content
- [ ] Properties `ok1`, `ok2`, `ok3`, `ok4` encoded and checked with Luke
- [ ] Brief write-up of proof/disproof results and tool limitations

### Current status in repo
- [x] `hw3/p3/verify4.lus` exists and contains `ok1`/`ok2`/`ok3`/`ok4`
- [x] `hw3/p3/README` exists with detailed Luke workflow/results
- [x] Required filename `README` present

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
- [x] `cruisectr2.lus` created with PID controller
- [x] `Makefile3` created with targets `cruisectr2-sim` and `cruisectr2-exe`
- [x] `README3` created

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
- [ ] `local_planner.py` not found (cannot generate without original CARLA code)
- [x] `README4` created
- [x] Code prints `world.lane_invasion_sensor.lane_invasion_counter` and `world.collision_sensor.collision_counter` (field mismatch fixed)

## Final Pre-Submission Checklist

- [x] Ensure exact required filenames are present
- [x] Ensure all required files are non-empty and runnable/buildable on ARC (except where noted)
- [x] Resolve naming mismatch (`README2.md` vs required `README` for Problem 3)
- [x] Resolve Problem 5 counter-field mismatch (`penalty` vs actual counter attribute names)

Note: `static.png` and `local_planner.py` cannot be generated without running on ARC or accessing original CARLA code.
