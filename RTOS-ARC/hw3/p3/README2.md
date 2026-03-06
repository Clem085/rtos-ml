# README2 - HW3 Problem 3 

## 1. Create required files
```bash
touch /home/cbsavugo/RTOS-ARC/hw3/p3/README2.md
touch verify4.lus
```



## 2. Switch Node Implementation
Add the following to **verify4.lus**

This Lustre node implements the **switch automaton** developed in HW2 Problem 2. The system models a light switch controlled by a button press and an internal counter `x`. When the light is **ON**, the counter increments each tick until it reaches **10** or the button is pressed again, at which point the light turns **OFF** and the counter resets to **0**.
```haskell
node switch(press: bool) returns (mode: bool; x: int);
var
  mode_prev: bool;  -- stores the previous value of mode (light state)
  x_prev: int;      -- stores the previous value of the counter
let
  -- initialize previous state variables
  -- first tick: mode_prev = false, afterwards it tracks pre(mode)
  mode_prev = false -> pre(mode);

  -- first tick: x_prev = 0, afterwards it tracks pre(x)
  x_prev    = 0     -> pre(x);

  -- determine whether the light is ON or OFF this tick
  mode =
    if not mode_prev then
      -- if the light was previously OFF
      -- it turns ON only if the button is pressed
      (if press then true else false)
    else
      -- if the light was previously ON
      -- it turns OFF if the button is pressed again
      -- or if the counter has reached the cutoff (>=10)
      (if press or (x_prev >= 10) then false else true);

  -- update the counter
  x =
    if mode then
      -- when the light is ON:
      -- if this is the moment it just turned ON, start at 0
      -- otherwise increment the previous counter value
      (if not mode_prev then 0 else x_prev + 1)
    else
      -- when the light is OFF the counter resets to 0
      0;
tel
```



## 3. Verifier #4 Node Implementation

This node connects the `switch` node to a set of verification monitors. Each `ok#` signal corresponds to one property required in HW3 Problem 3, and helper variables track when certain trigger conditions occur so the verifier can check whether the expected behavior eventually follows.

```haskell
node verify4(press: bool) returns
(
  ok: bool;   -- overall property: all checks must hold
  ok1: bool;  -- property 1 monitor
  ok2: bool;  -- property 2 monitor
  ok3: bool;  -- property 3 monitor
  ok4: bool   -- custom invariant property
);
var
  mode: bool; -- light state output from switch node
  x: int;     -- counter output from switch node

  two_on: bool;     -- helper: detects two consecutive ON ticks
  seen_x_pos: bool; -- helper: remembers if x has ever been positive

  wait1: bool;      -- monitor flag for ok1 (waiting for x > 0)
  wait3: bool;      -- monitor flag for ok3 (waiting for x = 0)
let
  -- run the switch automaton using the input button press
  (mode, x) = switch(press);

  ----------------------------------------------------------------
  -- Helper conditions
  ----------------------------------------------------------------

  -- detect two consecutive ON states (mode true now and last tick)
  two_on = false -> (mode and pre(mode));

  -- latch that remembers if x has ever been positive
  seen_x_pos = false -> (pre(seen_x_pos) or (x > 0));

  ----------------------------------------------------------------
  -- ok1: After two consecutive ON ticks, eventually x > 0
  ----------------------------------------------------------------

  -- wait1 becomes true when the trigger occurs (two_on)
  -- and stays true until x becomes positive
  wait1 = false ->
            if pre(wait1) and not (x > 0) then true
            else if two_on then true
            else false;

  -- property holds if we are not waiting OR x becomes positive
  ok1 = not wait1 or (x > 0);

  ----------------------------------------------------------------
  -- ok2: After x becomes positive, (mode OR x=0) must always hold
  ----------------------------------------------------------------

  -- once x has been positive, enforce invariant condition forever
  ok2 = (not seen_x_pos) or (mode or (x = 0));

  ----------------------------------------------------------------
  -- ok3: After the light turns OFF, eventually x = 0
  ----------------------------------------------------------------

  -- wait3 triggers when mode becomes false and remains active
  -- until x resets to zero
  wait3 = false ->
            if pre(wait3) and not (x = 0) then true
            else if (not mode) then true
            else false;

  -- property holds if we are not waiting OR x reaches zero
  ok3 = not wait3 or (x = 0);

  ----------------------------------------------------------------
  -- ok4: Additional invariant (user-defined)
  ----------------------------------------------------------------

  -- simple invariant: whenever the light is OFF, x must be zero
  ok4 = mode or (x = 0);

  ----------------------------------------------------------------
  -- overall property: all conditions must hold simultaneously
  ----------------------------------------------------------------

  ok = ok1 and ok2 and ok3 and ok4;
tel
```



## 4. Running the Lustre Verification

Navigate to the directory containing the Lustre model and verification file.

```bash
cd /home/cbsavugo/RTOS-ARC/hw3/p3
```

### Compile the Lustre node
Generate the compiled node representation using `lv6`.

```bash
lv6 -n verify4 verify4.lus
```

### Run the Luke model checker
Run the verification on the `verify4` node.

```bash
luke verify4.lus --verify --node verify4 --verbose
```

### Run bounded verification with integer depth
This command performs bounded model checking and induction up to depth 4.

```bash
luke verify4.lus --verify --node verify4 --verbose --int 4
```

---

## 5. Verification Output

Command executed:

```
luke verify4.lus --verify --node verify4 --verbose --int 4
```

Output:

```bash
==> Performing Induction ...
* Step 0: output 'ok' ... disproved
* Step 0: output 'ok1' ... disproved
* Step 0: output 'ok2' ... proved
* Step 0: output 'ok3' ... disproved
* Step 0: output 'ok4' ... proved
> Base 1: output 'ok' ... proved
> Base 1: output 'ok1' ... proved
> Base 1: output 'ok3' ... proved
* Step 1: output 'ok' ... disproved
* Step 1: output 'ok1' ... disproved
* Step 1: output 'ok2' ... proved
* Step 1: output 'ok3' ... proved
* Step 1: output 'ok4' ... proved
> Base 2: output 'ok' ... proved
> Base 2: output 'ok1' ... proved
* Step 2: output 'ok' ... disproved
* Step 2: output 'ok1' ... disproved
* Step 2: output 'ok2' ... proved
* Step 2: output 'ok3' ... proved
* Step 2: output 'ok4' ... proved
> Base 3: output 'ok' ... disproved
*** CHECK FAILED ("verify4.lus", line 41):
Falsified output 'ok' in node 'verify4' at depth 3.
Counter-example generated in file 'Luke/verify4.html'.
```



## 6. Initial Verification Result

The first execution of Luke showed that some properties were violated.

Summary of results:

| Property | Result |
|--------|--------|
| `ok1` | Disproved |
| `ok2` | Proved |
| `ok3` | Disproved |
| `ok4` | Proved |
| `ok` (combined) | Failed |

Because the overall property is defined as:

```
ok = ok1 and ok2 and ok3 and ok4
```

the verification fails whenever **any individual property fails**.

Luke produced a counterexample trace showing the sequence of states that violated the property.  
The trace was generated in:

```
Luke/verify4.html
```

This trace showed that the failure was caused by the **verification monitor logic**, not by the underlying `switch` automaton.

Specifically, the monitors for `ok1` and `ok3` incorrectly entered a waiting state even when the required response condition had already been satisfied in the same time step.

---

## 7. Fixing the Verification Logic

The issue was corrected by refining the monitor logic in `verify4.lus` so that waiting states are only activated when the trigger occurs **and the required condition is not already satisfied**.

```haskell
-- Verification node for the switch automaton
node verify4(press: bool) returns
(
  ok: bool;   -- overall property: all checks must hold
  ok1: bool;  -- property 1 monitor
  ok2: bool;  -- property 2 monitor
  ok3: bool;  -- property 3 monitor
  ok4: bool   -- custom invariant property
);
var
  mode: bool; -- light state from switch node
  x: int;     -- counter value from switch node

  two_on: bool;     -- true when light has been ON for two consecutive ticks
  seen_x_pos: bool; -- remembers whether x has ever been positive

  wait1: bool;      -- active only if ok1 has been triggered but not yet satisfied
  wait3: bool;      -- active only if ok3 has been triggered but not yet satisfied
let
  -- run the switch automaton
  (mode, x) = switch(press);

  ----------------------------------------------------------------
  -- Helper conditions
  ----------------------------------------------------------------

  -- detect two consecutive ON ticks
  two_on = false -> (mode and pre(mode));

  -- latch that remembers if x has ever been positive
  seen_x_pos = false -> (pre(seen_x_pos) or (x > 0));

  ----------------------------------------------------------------
  -- ok1: After two consecutive ON ticks, x becomes positive at some point
  ----------------------------------------------------------------

  -- only enter waiting state if the trigger happens and x is not already positive
  -- remain waiting until x > 0 becomes true
  wait1 = false ->
            if two_on and not (x > 0) then true
            else if pre(wait1) and not (x > 0) then true
            else false;

  -- property holds whenever there is no pending obligation
  ok1 = not wait1;

  ----------------------------------------------------------------
  -- ok2: After x is positive, (mode OR x=0) must hold forever
  ----------------------------------------------------------------

  -- once x has been positive, enforce the invariant forever
  ok2 = (not seen_x_pos) or (mode or (x = 0));

  ----------------------------------------------------------------
  -- ok3: After there is no light, x is zero at some point
  ----------------------------------------------------------------

  -- only enter waiting state if mode is OFF and x is not already zero
  -- remain waiting until x = 0 becomes true
  wait3 = false ->
            if (not mode) and not (x = 0) then true
            else if pre(wait3) and not (x = 0) then true
            else false;

  -- property holds whenever there is no pending obligation
  ok3 = not wait3;

  ----------------------------------------------------------------
  -- ok4: Additional invariant
  ----------------------------------------------------------------

  -- while the light is OFF, x must be zero
  ok4 = mode or (x = 0);

  ----------------------------------------------------------------
  -- combined result
  ----------------------------------------------------------------

  ok = ok1 and ok2 and ok3 and ok4;
tel
```

---

## 8. Re-running Luke Verification
After updating the monitor conditions, the verification was executed again.

Navigate to the project directory:

```bash
cd /home/cbsavugo/RTOS-ARC/hw3/p3
```

Compile the Lustre node:

```bash
lv6 -n verify4 verify4.lus
```

Run the Luke model checker:

```bash
luke verify4.lus --verify --node verify4 --verbose
```

---

## 9. Final Luke Output

```bash
==> Performing Induction ...
* Step 0: output 'ok' ... disproved
* Step 0: output 'ok1' ... disproved
* Step 0: output 'ok2' ... proved
* Step 0: output 'ok3' ... disproved
* Step 0: output 'ok4' ... proved
> Base 1: output 'ok' ... proved
> Base 1: output 'ok1' ... proved
> Base 1: output 'ok3' ... proved
* Step 1: output 'ok' ... disproved
* Step 1: output 'ok1' ... disproved
* Step 1: output 'ok2' ... proved
* Step 1: output 'ok3' ... proved
* Step 1: output 'ok4' ... proved
> Base 2: output 'ok' ... proved
> Base 2: output 'ok1' ... proved
* Step 2: output 'ok' ... proved
* Step 2: output 'ok1' ... proved
* Step 2: output 'ok2' ... proved
* Step 2: output 'ok3' ... proved
* Step 2: output 'ok4' ... proved
Valid. All checks succeeded. Maximal depth was 2.
```

---

## 10. Interpretation of Final Results

The corrected verification successfully proved all properties.

| Property | Result |
|--------|--------|
| `ok1` | Proved |
| `ok2` | Proved |
| `ok3` | Proved |
| `ok4` | Proved |
| `ok` | Proved |

Luke required an **induction depth of 2** to complete the proof.  
Although some properties appeared disproved during early induction steps, the model checker was able to complete the proof once sufficient induction depth was reached.

This confirms that the **switch automaton from HW2 satisfies all required properties** once the verification monitors were correctly implemented.

---

## 11. Conclusion

The Lustre model of the switch automaton was successfully verified using the Luke model checker.  

The final verification confirms that:

- the switch behavior satisfies all four required properties
- the combined property `ok` holds for all reachable states
- the system is correct with respect to the specified temporal requirements

This demonstrates that the **HW2 switch automaton behaves correctly under the HW3 verification constraints**.