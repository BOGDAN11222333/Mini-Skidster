# Servo 1 — Gearbox and No-Arm Current Tests

**Project:** Mini-Skidster  
**Test date:** 21 November 2026  
**Test program:** Servo Test & Current Monitor v1.2.1  
**Test controller:** Arduino UNO (bench test only)  
**Current sensor:** ACS712-5A, configured for 185 mV/A  
**Servo signal:** D9  
**Current sensor output:** A5  
**Servo supply voltage / exact servo model:** Not recorded in this test log

## Test method

The same automatic sequence was run for each configuration:

1. Full right (180°)
2. Full left (0°)
3. Centre (90°)
4. 10° steps from 100° to 180°
5. 10° steps back from 170° to 0°

At each commanded position the program waited for movement to finish, settled for 1 second, then measured holding current for 1 second. Current was also sampled during movement and settling.

**Important distinction:** The overall average and global peaks include movement, settling and holding. Per-position `HOLD RESULT` averages cover only the dedicated 1-second holding measurement.

## Results summary

| Configuration | Run | Overall average | Minimum | Highest global peak | Measurement windows |
|---|---:|---:|---:|---:|---:|
| Servo without external gearbox / arm | 1 | 0.042 A | 0.001 A | 0.744 A @ 110° | 6,589 |
| Servo without external gearbox / arm | 2 | 0.041 A | 0.001 A | 0.771 A @ 170° | 6,590 |
| Gearbox fitted, no arm | 1 | 0.055 A | 0.001 A | 0.760 A @ 137° | 6,502 |
| Gearbox fitted, no arm | 2 (repeat) | 0.058 A | 0.001 A | 0.744 A @ 145° | 6,590 |

## Global peak details

- Bare servo, run 1: 0.744 A @ 110°, 0.702 A @ 138°, 0.702 A @ 140°.
- Bare servo, run 2: 0.771 A @ 170°, 0.755 A @ 102°, 0.723 A @ 170°.
- Gearbox/no arm, run 1: 0.760 A @ 137°, 0.760 A @ 94°, 0.755 A @ 165°.
- Gearbox/no arm, run 2: 0.744 A @ 145°, 0.734 A @ 130°, 0.734 A @ 114°.

## Holding-current observations

| Configuration | Typical holding-current range |
|---|---:|
| Servo without external gearbox / arm | approximately 0.015–0.020 A |
| Gearbox fitted, no arm | approximately 0.026–0.038 A |

For the second gearbox/no-arm run, most holding averages were approximately 0.026–0.034 A. Some positions were higher:

- 90°: 0.060 A average, 0.644 A peak
- 120°: 0.035 A average, 0.649 A peak
- 150°: 0.041 A average, 0.427 A peak

The 90° result is an outlier compared with the other positions and the first gearbox/no-arm run.

## Preliminary interpretation

- The two bare-servo runs are close: overall averages of 41–42 mA.
- The two gearbox/no-arm runs are also reasonably close: overall averages of 55–58 mA.
- The gearbox/no-arm holding-current range is higher than the bare-servo range by roughly 10–20 mA in these tests.
- The large global peaks vary in both magnitude and commanded angle between runs. They are transient 10 ms windows and do not establish a repeatable mechanical binding point.
- The second gearbox/no-arm run contains an isolated high result at 90°. Repeat this position-specific observation in a future test rather than treating it as a confirmed fault.

These are bench observations, not a final servo selection or protection threshold.

## Next planned test

Attach the servo arm directly to the gearbox/output shaft, but leave it disconnected from the Mini-Skidster mechanism and without an external load. Repeat the same automatic test. This will isolate the effect of the arm's own mass and inertia before testing the complete linkage.

## Measurement limitations

- The ACS712 zero calibration was performed with the servo supply OFF.
- The code calculates absolute deviation from the calibrated zero voltage; noise and offset drift can appear as positive current.
- A reported position is the commanded servo angle, not measured shaft feedback.
- The top-three global peaks may be multiple windows from one transient event.
- The program does not cut servo power automatically on stall or overcurrent.
- The 0° and 180° commands are nominal endpoints. Confirm the servo can safely reach them before repeating the test.

## Source program

The reusable Arduino UNO sketch is maintained in the Electronics-Lab repository:

`Programs/Arduino_UNO/Servo_Test_Current_Monitor/v1.2.1/Servo_Test_Current_Monitor_v1.2.1.ino`
