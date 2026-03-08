# HW 3 - Assignment Checklist

**Deadline:** see web page  
**Note:** The last problem is a **group assignment**.

All programs have to be written in **Lustre, C, or Python** (depends on the problem) and turned in with a corresponding **Makefile**. They will be tested and graded on the **ARC cluster**, where they have to run. If they do not compile (with the Makefile) and run, there will be **no points**.

---

## Global “Before You Submit” Checklist

- [ ] Confirm each required program is written in the correct language (Lustre / C / Python) for that problem
- [ ] Confirm each required Makefile exists (when required) and builds successfully on ARC
- [ ] Confirm each executable/script runs on ARC (not just locally)
- [ ] Confirm code is commented (comments count **15%** of points for programming assignments)
- [ ] Confirm README(s) include commands to install/execute and brief documentation
- [ ] Confirm any required input files and test programs are included (when required)

What to turn in for programming assignments (general):
- [ ] Commented program(s) as source code (**comments count 15% of the points**)
- [ ] Makefiles (if required)
- [ ] Test programs as source (and input files, if required)
- [ ] README (documentation to outline solution and list commands to install/execute)

---

# Problem 1 (10 points)

### Task
Determine if the task set is schedulable according to:
- (a) **Deadline monotone scheduling** under the **system utilization test**
- (b) **GTDA (Generalized Timing Demand Analysis)**, but with the priority assignment in the listed order **(T1, T3, T2)**
Also calculate the **in-phase busy period**.

Task set:
- T1 = (7, 1)
- T2 = (12, 5)
- T3 = (9, 3, 11)

### Checklist
- [ ] Perform deadline monotone schedulability check via utilization test
- [ ] Perform GTDA with priority order (T1, T3, T2)
- [ ] Calculate the in-phase busy period
- [ ] Format final answers clearly for submission

### Turn in
- [ ] `problem1.txt`

---

# Problem 2 (20 points)

### Task
Extend your prior EDF scheduling program to support **static priority scheduling** under Linux on a **uniprocessor (single core)**. Create a task set and plot its execution in a Gantt chart (**static.png**).

Additional details (as before, plus the following):

#### Build / invocation requirement
Your `Makefile2` should generate a binary executable file named:
- `rtsched`

It must accept the name of the input file as its command argument in this form:
- `rtsched -static {inputFileName}`

A sample task set is provided as input (numbers are in milliseconds).

#### Input file requirement (priority order)
For static priority scheduling, the input file shall list tasks in **priority order** (highest priority first).  
If you list them in increasing period, they should be scheduled in **rate (or deadline) monotone priority order**.

---

## Step-by-step inside `main`

- [ ] Initialize a thread attribute with `pthread_attr_init()`
- [ ] Use `pthread_attr_setschedpolicy()` to indicate the `SCHED_FIFO` policy
- [ ] Use `pthread_attr_setinheritsched()` with `PTHREAD_EXPLICIT_SCHED`
- [ ] Pass the attribute at thread creation time
- [ ] Keep all other behavior the same as in EDF

## Step-by-step inside the thread

- [ ] For FIFO, `sched_setattr()` does **not** need to be called

---

## Outputs / Evidence

- [ ] Create a task set (or use the sample set as a baseline)
- [ ] Produce a Gantt chart image of execution named `static.png`

### Turn in
- [ ] `rtsched.c` (updated)
- [ ] `Makefile2`
- [ ] `static.png`
- [ ] `README`

---

# Problem 3 (10 points)

### Task
Change your Lustre switch program into a **verifier** and check for correctness using **Luke**.  
Try to prove/disprove the following properties with the Luke model checker based on the “temporal” property templates of the referenced paper:

Properties to prove/disprove:
- ok1: After the light has been on two ticks in sequence, **x becomes positive** at some point.
- ok2: After **x is positive**, the disjunction of **(light is on) OR (x is zero)** holds forever.
- ok3: After there is **no light**, **x is zero** at some point.
- ok4: Come up with **another time-based invariant** of similar complexity and show that it holds.

Documentation requirement:
- Keep it brief, but document observations and comment on correctness as well as tool limitations in the README.

### Checklist
- [x] Convert the switch program into a verifier Lustre program
- [ ] Encode ok1 and attempt proof/disproof in Luke
- [ ] Encode ok2 and attempt proof/disproof in Luke
- [ ] Encode ok3 and attempt proof/disproof in Luke
- [ ] Define ok4 (your own time-based invariant) and attempt proof/disproof
- [ ] Write brief observations in README, including tool limitations

### Turn in
- [ ] `verify4.lus`
- [ ] `README` (no Makefile)

---

# Problem 4 (30 points)

### Task
Enhance your Lustre program for cruise control to use a **PID controller** in `controlSpeed()` modeling with the specified data types and the corresponding controller code.

You must:
- Experimentally determine good PID coefficients
- Demonstrate them by comparing your **old solution** to your **new PID solution**
- You may use **command line output** or **graphical output from Lucille**

---

## PID data types to model (as provided)

### `pid_params_t`

- [ ] Implement gain parameters:
  - [ ] `Kgain` (Loop gain parameter)
  - [ ] `Ti` (Integrator time constant)
  - [ ] `Td` (Differentiator time constant)
  - [ ] `delT` (Update time interval)
- [ ] Implement setpoint parameters:
  - [ ] `setpt` (Regulated distance to maintain)
  - [ ] `input` (Actual distance)
- [ ] Implement limits:
  - [ ] `highlim` (Upper bound)
  - [ ] `lowlim` (Lower bound)

### `pid_state_t`

- [ ] Implement controller state:
  - [ ] `proportional`
  - [ ] `integral` (Summation of setpoint errors)
  - [ ] `deriv` (Previous setpoint error)
  - [ ] `output` (Output of PID controller)

---

## PID controller algorithm steps (as provided)

- [ ] Compute error: `seterr = setpt - input`

Proportional response:
- [ ] `pidout = seterr`
- [ ] `pid_state->proportional = seterr`

Integral response:
- [ ] `pidout += (pid_state->integral * delT / Ti)`
- [ ] `pid_state->integral += seterr`

Derivative response:
- [ ] `change = seterr - pid_state->deriv`
- [ ] `pidout += change * Td / delT`
- [ ] `pid_state->deriv = seterr`

Gain scaling:
- [ ] `pidout = pidout * Kgain`

Enforce output limits:
- [ ] If `pidout > highlim` then clamp to `highlim`
- [ ] If `pidout < lowlim` then clamp to `lowlim`

Store output:
- [ ] `pid_state->output = pidout`

---

## Demonstration / comparison requirements

- [ ] Choose initial PID coefficients (Kgain, Ti, Td, delT, limits)
- [ ] Run old controller and capture output (CLI or Lucille)
- [ ] Run PID controller and capture output (CLI or Lucille)
- [ ] Compare old vs new behavior (briefly, but specifically)
- [ ] Adjust coefficients until performance is “good” experimentally

### Turn in
- [ ] `cruisectr2.lus`
- [ ] `Makefile3` with targets:
  - [ ] `cruisectr2-sim`
  - [ ] `cruisectr2-exe`
- [ ] `README3`

---

# Problem 5 (30 points, group assignment)

### Task
Given the **Carla simulator** and its GitHub repo, develop your own PID-controlled autonomous driving agent, and send it (use the **Jeep again**) on a **race drive through town** with the objective to be **fast instead of safe**. Tweak the existing PID controller to this end. You may use additional hard-coded tricks for driving control. See prior HW1 for CARLA setup details.

You should start with a copy of the automatic control and add a new agent called **"MyPID"**. Start out with the same code as for the **"Basic"** agent. Then make the following changes:

---

## Required changes

### Agent + parameter plumbing
- [ ] Extend the `BasicAgent` to take additional initialization parameters (or provide new interfaces) so a different set of PID control parameters can be relayed from `my_automatic_control.py`
- [ ] Modify `PythonAPI/carla/agents/navigation/local_planner.py` to support relaying these new PID parameters
- [ ] Focus on longitudinal control (most receptive to changes)

### Rules / driving constraints
- [ ] You may disregard traffic signals and signs, and speed limits

### Start and destination
- [ ] Start at spawn location:
  - [ ] x = -45.235935
  - [ ] y = -36.500095
  - [ ] z = 0.600000
- [ ] Destination:
  - [ ] x = 67.659737
  - [ ] y = 69.835068
  - [ ] z = 0.000000
- [ ] Use default `Town10HD`

### Timing and result printing (format requirement)
Add start/end timing exactly in the described manner:

- [ ] Set start time:
  - [ ] `start_time = world.hud.simulation_time`  (new code)
  - [ ] `print("start time", start_time)`         (new code)
- [ ] Keep existing clock logic:
  - [ ] `clock = pygame.time.Clock()`             (old code)
- [ ] When target reached, print final line in the required format and stop:
  - [ ] Print: total time = `world.hud.simulation_time - start_time`
  - [ ] Print: invasions = `world.lane_invasion_sensor.penalty`
  - [ ] Print: collisions = `world.collision_sensor.penalty`
  - [ ] Break out / end simulation

### Penalty counters (collision + lane invasion)
- [ ] Add counters to collision and lane invasion classes
- [ ] Collision counter increments on collision **once per colliding object**
- [ ] Lane invasion counter increments for all invasions **except**:
  - [ ] `carla.libcarla.LaneMarkingType.Broken` (crossing broken lines is fine)
- [ ] Do count invasions for solid lines and type NONE, among others (except where explicitly allowed)
- [ ] Write messages to stdout and document trial runs

### Optimization objective
- [ ] Aim for the shortest total time with the smallest number of penalties/collisions
- [ ] Each penalty/collision counts for **5 seconds**
- [ ] Exception: the last two `NONE` penalties may be ignored

### Helpful files to consult
- [ ] `PythonAPI/carla/agents/navigation/controller.py` (longitudinal/lateral PID control)
- [ ] `PythonAPI/carla/agents/navigation/base_agent.py` (starting point for an agent)

---

## Output formatting example (use same formatting)

- [ ] Ensure output follows this shape:

```

start time 2708.4803261295892
invasion {carla.libcarla.LaneMarkingType.Broken}
invasion {carla.libcarla.LaneMarkingType.Broken}
invasion {carla.libcarla.LaneMarkingType.Broken}
invasion {carla.libcarla.LaneMarkingType.Broken}
invasion {carla.libcarla.LaneMarkingType.NONE}
invasion {carla.libcarla.LaneMarkingType.NONE}
The target has been reached, stopping the simulation, total time is 23.88469760864973 invasions 2 collisions 0

```

---

### Turn in
- [ ] `my_automatic_control.py`
- [ ] `local_planner.py`
- [ ] `README4` containing race results (brief but specific)

---

## Final Submission Sweep

- [ ] Problem 1: `problem1.txt`
- [ ] Problem 2: `rtsched.c`, `Makefile2`, `static.png`, `README`
- [ ] Problem 3: `verify4.lus`, `README` (no Makefile)
- [ ] Problem 4: `cruisectr2.lus`, `Makefile3` (targets `cruisectr2-sim`, `cruisectr2-exe`), `README3`
- [ ] Problem 5 (group): `my_automatic_control.py`, `local_planner.py`, `README4`
- [ ] Confirm everything builds/runs on ARC where applicable
- [ ] Confirm naming matches exactly (filenames, targets, and output formatting)


 
