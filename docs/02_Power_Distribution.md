# Mini Skidster — Power Distribution

**Status:** Proposed architecture. Verify voltages with a multimeter before connecting the electronics.

## Battery

- Two 18650 lithium-ion cells in series (2S).
- Nominal pack voltage: 7.4 V.
- Fully charged pack voltage: 8.4 V.
- Use a suitable 2S BMS with cell balancing and adequate continuous/peak current ratings.
- Use a charger specifically intended for a 2S lithium-ion pack (8.4 V CC/CV).
- Fit a suitably rated fuse close to the battery positive output and a main power switch.

## Proposed branches

### Branch A — drive motors

Battery/BMS output supplies the DRV8833 VM input directly.

- Left N20 motor connects to one H-bridge output pair.
- Right N20 motor connects to the other H-bridge output pair.
- The N20 motors are rated 6 V, while the 2S battery reaches 8.4 V when fully charged.
- The proposed software PWM ceiling is only an initial experiment, not a guaranteed voltage/current protection method.

**Important PWM limitation:** PWM does not reduce the instantaneous voltage of each ON pulse. At full charge, the motor still receives battery-voltage pulses (up to 8.4 V). A duty-cycle calculation such as 6/8.4 ≈ 71% estimates average applied voltage only; it does not guarantee safe winding current, temperature, commutation behavior or gearbox life. Measure current and temperature under representative load. If the motor manufacturer's 6 V maximum must not be exceeded, use a regulated motor supply instead of relying solely on PWM.

The DRV8833 IC has a motor supply operating range of 2.7–10.8 V, but the actual breakout board's current and thermal limits must be checked. Confirm motor stall current before deciding whether the driver is suitable.

### Branch B — controller and servos

Use the XL4015 module, adjusted and verified at 5.0 V, for:
- ESP32-C3 via its USB-C power input.
- Both MG90S servos via the module's screw terminals.

The user has tested an SG90 servo successfully at 5 V; the selected MG90S servos are also planned for 5 V operation. Confirm their actual current demand by measurement.

**XL4015 module check required:** Do not assume the USB-C connector and screw terminals are independent outputs. They may share one regulated output. With the module disconnected from the ESP32 and servos, measure the screw-terminal output and verify the USB-C output behavior. If both are the same 5 V rail, this is acceptable for the intended 5 V loads provided the module can supply their combined current and remains adequately cool.

Do not connect the XL4015 output to the ESP32 USB-C port while another USB power source is also connected unless the board's power-path design explicitly supports it.

## Grounding

Use a common ground reference:
- Battery/BMS negative.
- DRV8833 GND.
- XL4015 output negative.
- ESP32-C3 GND.
- Servo supply negative.

Keep high-current motor and servo return wiring appropriately sized and routed so that motor/servo current does not flow through thin ESP32 ground wiring.

## Before first power-up

1. Check battery polarity and BMS connections.
2. Check the fuse and switch.
3. Set XL4015 output to 5.0 V and verify with a multimeter.
4. Verify the USB-C output and screw-terminal output arrangement.
5. Verify common ground and polarity.
6. Check DRV8833 board pinout and current rating.
7. Do not connect motors or servos until the relevant supply voltage has been measured.
