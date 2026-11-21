# Mini-Skidster — Testing Documentation

This section contains project-specific bench-test results, measurements and conclusions for the Mini-Skidster.

## Test index

### Servo tests

- [Servo 1 — Gearbox/current tests](Servo/Servo_1_Gearbox_Current_Test.md) — bare servo and gearbox fitted, without the arm.
- [SG90 — Bucket-release servo](Servo/SG90_Bucket_Release_Current_Test.md) — three no-load baseline runs.

See the [Servo test overview](Servo/README.md) for a quick comparison and test status.

### Drive motor tests

Drive motor test result files will be added here as the left and right N20 motor measurements are completed.

## Reusable test resources

The reusable Arduino UNO test sketch and generic test procedure are maintained separately in the Electronics-Lab repository:

- Program: `Programs/Arduino_UNO/Servo_Test_Current_Monitor/v1.2.1/Servo_Test_Current_Monitor_v1.2.1.ino`
- Procedure: `Tests/Servo_Current_Test/README.md`

## Documentation convention

Keep each actuator's measurements and conclusions in its own file. Use this page as the navigation index. The older combined file, [Servo_Current_Test_Results.md](Servo_Current_Test_Results.md), is retained as a historical record of the original combined log.
