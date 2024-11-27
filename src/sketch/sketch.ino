#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define THERMISTOR_PIN 4
#define PHOTORESISTOR_PIN 36
#define POTENTIOMETER_PIN 34  // ADC1_6

// Initialize LCD (address 0x27, 16 chars, 2 lines)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function to convert light level to text description
const char* getLightDescription(int analogValue) {
  if (analogValue < 40) {
    return "Dark";
  } else if (analogValue < 800) {
    return "Dim";
  } else if (analogValue < 2000) {
    return "Light";
  } else if (analogValue < 3200) {
    return "Bright";
  } else {
    return "Very bright";
  }
}

void setup() {
  Serial.begin(9600);
  
  // Configure ADC
  analogSetAttenuation(ADC_11db);
  analogSetWidth(12); // Set ADC width to 12 bits
  
  // Initialize LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  
  // Display initial message
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather Station");
  delay(2000);
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void updateDisplay(double tempC, const char* lightDesc, int rawPot, float potVoltage) {
  // First line: Temperature
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(tempC, 1);
  lcd.print("C");
  
  // Second line: Light level and potentiometer
  lcd.setCursor(0, 1);
  lcd.print(lightDesc);
  lcd.print(" ");
  lcd.print(rawPot); // Show raw value for debugging
}

void loop() {
  // Temperature reading
  int adcValue = analogRead(THERMISTOR_PIN);
  double voltage = (float)adcValue / 4095.0 * 3.3;
  double Rt = 10 * voltage / (3.3 - voltage);
  double tempK = 1 / (1 / (273.15 + 25) + log(Rt / 10) / 3950.0);
  double tempC = tempK - 273.15;
  Serial.printf("ADC value : %d,\tVoltage : %.2fV, \tTemperature : %.2fC\n", adcValue, voltage, tempC);

  // Light sensor reading
  int analogValue = analogRead(PHOTORESISTOR_PIN);
  Serial.print("Analog Value = ");
  Serial.print(analogValue);
  const char* lightDesc = getLightDescription(analogValue);
  Serial.print(" => ");
  Serial.println(lightDesc);

  // Potentiometer reading - with more debug info
  int potentiometerValue = analogRead(POTENTIOMETER_PIN);
  float voltage2 = floatMap(potentiometerValue, 0, 4095, 0, 3.3);
  Serial.println("\nPotentiometer Debug Info:");
  Serial.printf("Raw ADC: %d\n", potentiometerValue);
  Serial.printf("Mapped Voltage: %.2fV\n", voltage2);
  Serial.printf("Pin Reading: %d\n", POTENTIOMETER_PIN);

  // Update LCD with all values
  updateDisplay(tempC, lightDesc, potentiometerValue, voltage2);
  
  delay(1000);
}