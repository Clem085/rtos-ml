node cruisectr
  (event_rotate: real;
  event_cruise: bool;
  event_decrement: real;
  event_increment: real)
returns
  (nat_speed: real;
  cruise_speed: real;
  pressure_force: real;
  cruise_output: bool);

var
  V34_prev_rotation: real;
  V35_prev_force: real;
  V36_updated_rotation: real;
  V101_prev_state: bool;
  V127_error: real;
  V128_abs_error: real;
  V129_abs_pressure_force: real;
  V130_prev_pressure: real;

let
  nat_speed = (event_rotate * ((10.000000 * 60.000000) / 672.000000));
  cruise_speed = (0.000000 -> (if (cruise_output and (not (pre V101_prev_state)
  )) then nat_speed else (if (cruise_output and (event_increment <> 0.000000)) 
  then ((pre cruise_speed) + 1.000000) else (if (cruise_output and (
  event_decrement <> 0.000000)) then ((pre cruise_speed) - 1.000000) else (pre 
  cruise_speed)))));
  pressure_force = (0.000000 -> (if ((V129_abs_pressure_force > V128_abs_error) 
  and (V128_abs_error > 0.500000)) then (V130_prev_pressure + 0.250000) else 
  (if (V130_prev_pressure < 0.100000) then 0.000000 else (V130_prev_pressure - 
  0.200000))));
  cruise_output = (false -> (if event_cruise then (not (pre V101_prev_state)) 
  else (pre V101_prev_state)));
  V34_prev_rotation = (0.000000 -> (pre V36_updated_rotation));
  V35_prev_force = (0.000000 -> (pre pressure_force));
  V36_updated_rotation = (if event_cruise then (V34_prev_rotation + 
  V35_prev_force) else V34_prev_rotation);
  V101_prev_state = (false -> (pre cruise_output));
  V127_error = (cruise_speed - nat_speed);
  V128_abs_error = (if (V127_error < 0.000000) then (-V127_error) else 
  V127_error);
  V129_abs_pressure_force = (if (V130_prev_pressure < 0.000000) then (-
  V130_prev_pressure) else V130_prev_pressure);
  V130_prev_pressure = (0.000000 -> (pre pressure_force));
tel

