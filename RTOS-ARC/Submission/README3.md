# HW3 Problem 4: Cruise Control with PID

This Lustre program implements a cruise control system enhanced with a Proportional-Integral-Derivative (PID) controller. The PID controller regulates the vehicle's speed by adjusting the pressure force applied to the accelerator/brake system to maintain the target cruise speed.

## Solution Overview

The system consists of several Lustre nodes:
- `measureSpeed`: Converts wheel rotation events to natural speed
- `setSpeed`: Manages cruise control activation and target speed setting
- `controlSpeed`: Implements the PID control algorithm
- `changeSpeed`: Integrates pressure force into rotation when cruising
- `cruisectr2`: Main controller node integrating all components

### PID Controller Implementation

The PID controller is implemented in the `controlSpeed` node:

```haskell
node controlSpeed(pid_params: pid_params_t; pid_state: pid_state_t) returns (pressure_force: real; updated_pid_state: pid_state_t);
var
    seterr: real;
    pidout: real;
let
    -- Compute error
    seterr = pid_params.setpt - pid_params.input;

    -- Proportional
    pidout = seterr;
    updated_pid_state.proportional = seterr;

    -- Integral
    pidout = pidout + (pid_state.integral * pid_params.delT / pid_params.Ti);
    updated_pid_state.integral = pid_state.integral + seterr;

    -- Derivative
    pidout = pidout + (seterr - pid_state.deriv) * pid_params.Td / pid_params.delT;
    updated_pid_state.deriv = seterr;

    -- Gain
    pidout = pidout * pid_params.Kgain;

    -- Limits
    pidout = if pidout > pid_params.highlim then pid_params.highlim else if pidout < pid_params.lowlim then pid_params.lowlim else pidout;

    -- Output
    updated_pid_state.output = pidout;
    pressure_force = pidout;
tel
```

### Cruise Control Logic

The cruise control toggles on/off and adjusts target speed:

```haskell
node setSpeed(
    cruise_toggle: bool;
    event_increment: bool;
    event_decrement: bool;
    nat_speed: real;
    prev_state: bool
) returns (event_cruise: bool; cruise_speed: real);
let
    -- Cruise latch is toggled on demand
    event_cruise =
        false ->
        if cruise_toggle then not pre(prev_state)
        else pre(prev_state);

    -- Cruise target initializes when entering cruise
    cruise_speed =
        0.0 ->
        if event_cruise and not pre(prev_state) then nat_speed
        else if event_cruise and event_increment then pre(cruise_speed) + 1.0
        else if event_cruise and event_decrement then pre(cruise_speed) - 1.0
        else pre(cruise_speed);
tel
```

## Build Instructions

```bash
make -f Makefile3 cruisectr2-exe
```

## Simulation

```bash
make -f Makefile3 cruisectr2-sim
```

## PID Tuning

The PID parameters are initialized with example values:

```haskell
pid_params.Kgain = 1.0;  -- Proportional gain
pid_params.Ti = 1.0;     -- Integral time
pid_params.Td = 0.1;     -- Derivative time
pid_params.delT = 0.1;   -- Time step
```

These should be tuned experimentally for optimal performance:
- **Kgain**: Affects responsiveness (higher = more aggressive)
- **Ti**: Affects integral windup prevention (higher = slower integral accumulation)
- **Td**: Affects overshoot damping (higher = more damping)

## Comparison with Previous Controller

Compare this PID-enhanced controller with the previous version (cruisectr.lus) by:
1. Running simulations with identical input scenarios
2. Observing speed stability and settling time
3. Analyzing pressure force outputs for smoothness
4. Checking response to disturbances (speed changes, toggles)

The PID controller should provide better steady-state accuracy and disturbance rejection compared to simpler control methods.