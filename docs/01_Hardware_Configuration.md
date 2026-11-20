# Mini Skidster — Hardware Configuration

**Status:** Initial configuration; verify physical modules and ratings before final wiring.

## Confirmed project hardware

| Item | Selected component | Notes |
|---|---|---|
| Controller | ESP32-C3 Super Mini | Exact board variant confirmed; verify the pin labels on the physical board before wiring |
| Drive motor driver | DRV8833 dual H-bridge module | Screw terminals will be soldered to the module |
| Drive motors | 2 × 6 V N20 geared DC motors | One motor per side; exact motor current and gearbox details to be measured/confirmed |
| Arm actuator | 1 × MG90S servo | 5 V operation has been tested successfully |
| Bucket actuator | 1 × MG90S servo | 5 V operation has been tested successfully |
| Battery | 2 × 18650 cells in series (2S) | 7.4 V nominal; 8.4 V fully charged |
| Battery protection | 2S BMS | Confirm suitable continuous and peak current ratings |
| Main adjustable regulator | XL4015 module with USB-C connector and screw terminals | Intended to supply ESP32-C3 Super Mini and both servos at 5 V; verify the exact module's output topology and ratings |
| Current measurement | Arduino UNO + ACS712-5A | Test-bench instrument for motor and servo current measurements |

## Functional arrangement

- Differential/skid steering: left and right N20 motors are controlled independently.
- Arm movement: MG90S servo.
- Bucket movement: MG90S servo.
- Phone interface: Android application made with MIT App Inventor.
- Wireless control: Wi-Fi is the planned vehicle-control link.

## Controller notes — ESP32-C3 Super Mini

- Use the ESP32-C3 Super Mini pinout when assigning GPIOs; do not reuse pin assignments from an ESP32-S3 or a different ESP32-C3 development board without checking compatibility.
- Confirm the exact board's printed pin labels and USB/boot-related pin constraints before connecting the DRV8833 inputs or servo signal wires.
- The board is powered from the regulated 5 V supply as planned; confirm the specific board's 5 V input pin labeling before wiring.

## Still to confirm

- Exact ESP32-C3 Super Mini board revision/pin labels and safe GPIO assignments.
- Exact DRV8833 breakout-board pin labels and continuous/peak current capability.
- N20 motor stall current and normal loaded current.
- MG90S idle, moving, loaded and brief stall current.
- XL4015 module output arrangement, USB-C behavior, and realistic thermal/current capability.
- Battery cell specifications, BMS rating, wiring gauge, fuse rating and charger.

Do not treat provisional GPIO assignments or unmeasured current values as final design decisions.
