# Mini Skidster — Current Measurement Test Bench

**Status:** Test setup planned; measurements not yet recorded.

## Equipment

- Arduino UNO.
- ACS712-5A current sensor module.
- XL4015 adjustable DC-DC converter.
- Multimeter for setting and verifying supply voltage.
- One motor or one servo under test at a time.

## ACS712-5A characteristics

- Nominal measurement range: ±5 A.
- Typical sensitivity: 185 mV/A (0.185 V/A).
- Sensor supply: 5 V.
- Arduino UNO analog output measurement: A0.
- The sensor's current path (IP+ to IP−) must be connected in series with the load, never across the supply.

The 5 A range is a hard measurement limit, not a current limiter. Do not intentionally stall a motor or servo unless the supply is current-limited and the expected current is known to remain within the sensor, wiring, connector and driver ratings. The ACS712 may saturate above its range.

## Wiring

### Sensor signal connections

| ACS712 | Arduino UNO |
|---|---|
| VCC | 5V |
| GND | GND |
| OUT | A0 |

### Load current path — N20 motor

```text
XL4015 +  ->  ACS712 IP+
ACS712 IP- ->  N20 motor +
N20 motor - -> XL4015 -
```

The Arduino and load supply must share a ground reference for this setup. Power the ACS712 from the Arduino 5 V rail. Set the XL4015 to 6.0 V and verify it with a multimeter before connecting a 6 V N20 motor.

For a servo, place the ACS712 in series with the servo's +5 V supply lead. Connect servo ground to the 5 V supply negative. Connect the servo signal to a suitable Arduino servo-control pin, with a common signal ground.

## Test sequence

1. **Zero-current check:** motor/load disconnected; power Arduino and ACS712. Record zero-current output voltage and current stability.
2. **N20 no-load:** set XL4015 to 6.0 V; run one motor freely and record average current.
3. **N20 working load:** apply a controlled representative mechanical load; record current.
4. **N20 startup:** observe startup current. The Arduino/ACS712 sketch below is not a guaranteed capture of very short transients.
5. **N20 stall:** only if necessary, use a current-limited bench supply and a controlled, very brief stall. Do not hold a motor stalled; stop immediately if current approaches 5 A or any component's rating.
6. Repeat for the second N20 motor.
7. Test each MG90S at 5.0 V: idle, moving without load, representative working load. Avoid forcing the servo against a hard stop. Only perform a brief stall test if the supply is current-limited and the expected current is safely within all ratings.
8. After individual tests, measure the combined operating load of both servos and the ESP32 on the XL4015 branch, using safe representative movements.

## Arduino UNO sketch

This is an initial monitor, not a calibrated laboratory instrument or fast peak-capture system. The zero offset is calibrated at startup with no load current. The 5 V ADC reference and sensor sensitivity introduce measurement error; verify against a known current if accuracy is important.

```cpp
const int ACS_PIN = A0;
const float SENSITIVITY = 0.185; // V/A, ACS712-5A typical

float zeroVoltage = 2.5;
float maximumAbsCurrent = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1500);

  Serial.println("ACS712-5A current monitor");
  Serial.println("Ensure load current is zero for calibration.");

  long total = 0;
  const int calibrationSamples = 1000;

  for (int i = 0; i < calibrationSamples; i++) {
    total += analogRead(ACS_PIN);
    delay(2);
  }

  zeroVoltage = (total / (float)calibrationSamples) * (5.0 / 1023.0);

  Serial.print("Zero voltage: ");
  Serial.print(zeroVoltage, 4);
  Serial.println(" V");
  Serial.println("Average_A,PeakSample_A,MaximumObserved_A");
}

void loop() {
  const int samples = 50;
  float sumAbsCurrent = 0.0;
  float peakSample = 0.0;

  for (int i = 0; i < samples; i++) {
    int raw = analogRead(ACS_PIN);
    float voltage = raw * (5.0 / 1023.0);
    float current = (voltage - zeroVoltage) / SENSITIVITY;
    float absCurrent = fabs(current);

    sumAbsCurrent += absCurrent;

    if (absCurrent > peakSample) {
      peakSample = absCurrent;
    }
  }

  float averageCurrent = sumAbsCurrent / samples;

  if (peakSample > maximumAbsCurrent) {
    maximumAbsCurrent = peakSample;
  }

  Serial.print(averageCurrent, 3);
  Serial.print(",");
  Serial.print(peakSample, 3);
  Serial.print(",");
  Serial.println(maximumAbsCurrent, 3);

  delay(200);
}
```

If the compiler reports that `fabs` is not declared, add `#include <math.h>` at the top.

### Interpretation limits

- `Average_A` is the average of the absolute sample values in one batch.
- `PeakSample_A` is the largest individual sample in that batch, not a guaranteed true startup/stall peak.
- `MaximumObserved_A` is the largest batch peak since startup.
- Sampling and the 200 ms reporting interval can miss short transients.
- ACS712 zero offset and sensitivity vary with module, temperature and supply/reference accuracy. Recalibrate with zero current and, where possible, compare with a known current.
- Do not interpret the software's readings as protection. Use physical current limiting/fusing and component ratings.

## Measurement record

See [04_Test_Results.md](04_Test_Results.md).
