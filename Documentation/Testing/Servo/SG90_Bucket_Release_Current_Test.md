# SG90 — Bucket-Release Servo Current Test

**Project:** Mini-Skidster  
**Test date:** 21 November 2026  
**Servo:** SG90, intended for the bucket-release mechanism  
**Configuration:** No mechanical load or linkage attached  
**Test program:** Servo Test & Current Monitor v1.2.1  
**Test controller:** Arduino UNO (bench test only)  
**Current sensor:** ACS712-5A, configured for 185 mV/A  
**Servo signal:** D9  
**Current sensor output:** A5  
**Servo supply voltage:** Not recorded in the test log

Three complete automatic test runs were performed using the same 30-position sequence and timing.

## Test method

The sequence was:

1. Full right (180°)
2. Full left (0°)
3. Centre (90°)
4. 10° steps from 100° to 180°
5. 10° steps back from 170° to 0°

At each commanded position the program waited for movement to finish, settled for 1 second, then measured holding current for 1 second. Current was also sampled during movement and settling.

**Important distinction:** The overall average and global peaks include movement, settling and holding. Per-position `HOLD RESULT` averages cover only the dedicated 1-second holding measurement.

## Results summary

| Measurement | Run 1 | Run 2 | Run 3 |
|---|---:|---:|---:|
| Overall average current | 0.047 A | 0.044 A | 0.038 A |
| Minimum current | 0.001 A | 0.001 A | 0.001 A |
| Highest global peak | 0.729 A @ 153° | 0.787 A @ 100° | 0.739 A @ 140° |
| Measurement windows | 6,502 | 6,590 | 6,590 |
| Mean of 30 holding averages | 0.0231 A | 0.0233 A | 0.0214 A |

## Holding-current observations

- The mean holding current across all three runs is approximately **0.0226 A (22.6 mA)**.
- Individual one-second holding averages were generally around **0.017–0.034 A**.
- Holding current was broadly consistent across the commanded travel, with no repeatable position showing sustained unusually high current.
- Run 2 included an isolated 0.570 A holding-period peak at 140°; its one-second holding average at that position was 0.034 A. This high transient was not repeated in Run 3.
- Run 3 recorded a 0.100 A holding-period peak at 0°, with a 0.021 A holding average.

## Preliminary interpretation

- The overall averages range from 38 to 47 mA, showing reasonable repeatability for the complete movement/settling/holding sequence.
- The three global maximum peaks range from 0.729 to 0.787 A. Their commanded positions vary between runs, so these readings do not establish a repeatable mechanical obstruction.
- The SG90 no-load holding-current baseline is approximately **23 mA**.
- For preliminary power-supply planning, allow at least 1 A of short-duration current capacity for this servo, with additional design margin. This is a planning allowance based on observed transient peaks, not a measured stall-current rating or protection threshold.

## Next test

Connect the bucket-release linkage, initially without material in the bucket. Repeat the same v1.2.1 automatic test and compare the results with this no-load baseline. This will help identify the additional current caused by linkage friction, spring force and geometry.

## Measurement limitations

- The ACS712 zero calibration was performed with the servo supply OFF.
- The code calculates absolute deviation from the calibrated zero voltage; noise and offset drift can appear as positive current.
- A reported position is the commanded servo angle, not measured shaft feedback.
- The top-three global peaks may be multiple windows from one transient event.
- The program does not automatically disconnect servo power on stall or overcurrent.
- Do not force the SG90 against its mechanical end stops.

## Source program

The reusable Arduino UNO sketch is maintained in the Electronics-Lab repository:

`Programs/Arduino_UNO/Servo_Test_Current_Monitor/v1.2.1/Servo_Test_Current_Monitor_v1.2.1.ino`

The generic test procedure is also maintained in Electronics-Lab at `Tests/Servo_Current_Test/README.md`.
