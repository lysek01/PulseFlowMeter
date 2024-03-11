#include <PulseFlowMeter.h>

PulseFlowMeter meter;

#define PIN 35 // Input pin
#define K 25.613 // Pulses per liter parameter of flowmeter
#define MIN_FLOW 5 // Minimal flowrate treshold  
#define MAX_FLOW 50 // Maximal flowrate treshold


void setup() {
  Serial.begin(9600);
  meter.begin(PIN, K);  
  meter.setTresholds(MIN_FLOW, MAX_FLOW);
}

void loop() {
  Serial.print("Flowrate l/m");
  Serial.println(meter.getFlowRateLiterMinute());
  Serial.print("Flowrate m3/h: ");
  Serial.println(meter.getFlowRateCubicHour());
  Serial.print("Total volume: ");
  Serial.println(meter.getTotalVolume());
  delay(500);
}