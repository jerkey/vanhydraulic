#include "PWMFrequency.h" // https://github.com/kiwisincebirth/Arduino/blob/master/libraries/PWMFrequency/examples/LeonardoFrequencyTest/LeonardoFrequencyTest.pde

#define HYDMOTOR_PWM_PIN        9
#define HYDMOTOR_PWM_MAX        255 // 255 is fully ON with analogWrite()
#define HYDMOTOR_PWM_MIN        40 // start ramp from here
#define PWM_INC_RATE            5 // milliseconds between PWM increments

int mode = 1;  // 0 = user entry, 1 = automatic ramp up to max,
unsigned long lastIncrementPwm = 0; // what time we last incremented PWM value by 1
unsigned long displayTime = 0;
int oldPwmValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(HYDMOTOR_PWM_PIN, OUTPUT);
  setPWMPrescaler(HYDMOTOR_PWM_PIN,8); // 8=3906.25Hz   31250Hz / 1,8,64,256,1024
  Serial.println("vanhydraulic");
  setMotorPwm(HYDMOTOR_PWM_MIN); // start motor at minimum power
  lastIncrementPwm = millis();
}

void loop() {
  if (mode == 1) incrementPwm();
  while (Serial.available() > 0) {
    int inputInt = Serial.parseInt(); // look for the next valid integer in the incoming serial stream:
    if ((inputInt < 256) && (inputInt >= 0)) {
      mode = 0; // switch to user entry mode
      int pwmValue = constrain(inputInt, 0, 255);
      Serial.println(pwmValue);
      setMotorPwm(pwmValue);
    } else if ((inputInt >= -5) && (inputInt <= -1)) {
      mode = inputInt * -1;
      Serial.print("mode to ");
      Serial.println(inputInt * -1);
    } else if ((inputInt >= -10240) && (inputInt <= -10)) {
      setPWMPrescaler(HYDMOTOR_PWM_PIN,inputInt * -10);
      Serial.print("setPWMPrescaler to ");
      Serial.println(inputInt * -10);
    }
  }
  printDisplay();
}

void incrementPwm() {
  if (millis() - lastIncrementPwm > PWM_INC_RATE) {
    lastIncrementPwm = millis();
    setMotorPwm(constrain(oldPwmValue+1,HYDMOTOR_PWM_MIN,HYDMOTOR_PWM_MAX));
  }
}

void setMotorPwm(int newValue) {
  if (newValue != oldPwmValue) { // don't analogWrite unnecessarily
    analogWrite(HYDMOTOR_PWM_PIN,newValue);
    oldPwmValue = newValue;
  }
}

void printDisplay() {
  if ( millis() - displayTime > 1000 ) {
    displayTime = millis();
    // if (!digitalRead(OVERHEAT_SENSE)) Serial.println("OVERHEATING!  ");
    Serial.print("PWM: ");
    Serial.print(oldPwmValue);

    Serial.print("  mode: ");
    Serial.println(mode);
  }
}
