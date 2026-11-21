# Mini-Skidster — Servo Test Overview

This folder contains the current measurements and conclusions for each servo used in the Mini-Skidster.

## Individual test records

| Servo / function | Test record | Status |
|---|---|---|
| Servo 1 — gearbox / arm actuator | [Servo 1 gearbox current test](Servo_1_Gearbox_Current_Test.md) | Bare-servo and gearbox/no-arm tests recorded; arm test remains planned |
| SG90 — bucket release | [SG90 bucket-release current test](SG90_Bucket_Release_Current_Test.md) | Three no-load baseline runs recorded; linkage test remains planned |

## Quick comparison

| Configuration | Overall average current | Typical holding current | Highest global peak |
|---|---:|---:|---:|
| Servo 1, bare servo (two runs) | 0.041–0.042 A | 0.015–0.020 A | 0.771 A |
| Servo 1, gearbox fitted, no arm (two runs) | 0.055–0.058 A | 0.026–0.038 A | 0.760 A |
| SG90, bucket release, no load (three runs) | 0.038–0.047 A | Mean 0.0226 A | 0.787 A |

**Important:** These are separate servo models/configurations. Do not use this table alone to compare mechanical performance; the SG90 and Servo 1 have different roles and potentially different supply conditions. The servo supply voltage was not recorded in these logs.

## Test method and source

The tests use Servo Test & Current Monitor v1.2.1 on an Arduino UNO bench setup with an ACS712-5A current sensor. The reusable sketch and generic procedure are maintained in the Electronics-Lab repository.

The test records commanded positions and electrical current; it does not measure actual shaft position or automatically disconnect power during a stall.
