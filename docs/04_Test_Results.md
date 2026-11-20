# Mini Skidster — Test Results

**Status:** No measurements recorded yet.

Record measured current in amperes (A), supply voltage, test conditions and sensor setup. Do not enter estimated values as measured results.

## N20 motors

| Test | N20 left | N20 right | Supply voltage | Notes |
|---|---:|---:|---:|---|
| No-load current | TBD | TBD | 6.0 V target | Wheels/gearbox free-running |
| Representative working current | TBD | TBD | 6.0 V target | Record mechanical load |
| Startup peak observed | TBD | TBD | 6.0 V target | ACS712/UNO may miss short peaks |
| Brief stall current (only if safely tested) | TBD | TBD | 6.0 V target | Current-limited supply; very brief test |

## MG90S servos

| Test | Arm servo | Bucket servo | Supply voltage | Notes |
|---|---:|---:|---:|---|
| Idle current | TBD | TBD | 5.0 V target | Powered, stationary |
| Moving, no load | TBD | TBD | 5.0 V target | Record movement |
| Representative working current | TBD | TBD | 5.0 V target | Realistic mechanism/load |
| Brief stall current (only if safely tested) | TBD | TBD | 5.0 V target | Avoid prolonged mechanical blockage |

## Combined load

| Test | Measured current | Supply voltage | Notes |
|---|---:|---:|---|
| Both servos moving | TBD | 5.0 V target | |
| ESP32 + Wi-Fi + both servos | TBD | 5.0 V target | |
| Both drive motors under representative load | TBD | Battery voltage | Record battery state of charge |

## Design decisions pending measurements

- Confirm XL4015 suitability for ESP32-C3 plus both MG90S servos.
- Confirm DRV8833 module suitability for the two N20 motors.
- Confirm 2S BMS, battery cells, wiring and fuse ratings.
- Decide whether direct 2S motor supply with PWM is acceptable or a regulated 6 V motor supply is required. PWM duty alone does not guarantee that a 6 V motor is protected from 8.4 V pulses.
- Set firmware PWM limits only after testing under representative load and checking motor temperature.
