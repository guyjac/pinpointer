////////// CODE debuted on 11/5/2020 by El-Yeti /////////
// Last update 12/12/2020 Reduced consumption
// Works with arduino and with attiny85 at 16 / 20MHZ with crystal and with the ATTinyCore device manager from https://github.com/SpenceKonde/ATTinyCore
// YFM Pinpointer 2-tone metal detector
// Hardware and software implemented by Hector Santos ("The yeti" on youtube)
// https://yeti-lab.blogspot.com/
// YFM v2.1 interrupt 8/24/2020
// Input D2, Buzzer D12

bool start = true;
volatile int buff = 0;
unsigned long frq = 0;
int resta = 0;
int sens = 3; // sensitivity *** the higher the number the lower the sensitivity
int autoBalance = 0;
bool AutoBal = true;
volatile long frequency;
int counter = 0;
long unsigned long T;
const int L = 2000; // number of cycles measured **** the higher inductance the number mentor
volatile int intensity = 0;
volatile bool Fe = false;
bool Max = false;
volatile bool balanceInit = true;
const int toneFe = 500;
const int toneNoFe = 900;

void interruption() {

  counter++;

  if (counter == 1) T = micros(); //Micros Returns the number of microseconds since the Arduino board began running the current program. 

  else if (counter >= L) { // number of cycles measured **** the higher inductance the number mentor

    counter = 0;

    frq = T - micros(); 

    if (start) {
      frequency = frq;

      start = false;

    } else if (frq < (frequency - 1)) {
      frequency = frequency - 1; // This makes the reading much more stable but has a lot of inertia

      if (Max && !balanceInit) frequency = frequency - (intensity / 3); // remove inertia

    } else if (frq > (frequency + 1)) {
      frequency = frequency + 1;

      if (Max && !balanceInit) frequency = frequency + (intensity / 3);

    }

    resta = buff - frequency;

    if (resta >= 0) { // Ferrous metal

      intensity = resta; // the variable "intensity" is for the loop

      Fe = true; // the variable "Fe" is for the loop

      if (intensity > 3) Max = true;

      else Max = false;

    } else if (resta < 0) { // Nonferrous metals

      intensity = -resta;

      Fe = false;

      if (intensity > 3) Max = true;

      else Max = false;

    }

  }

}

void setup() {

  bitWrite(ACSR, ACD, 1); // Disable the comparator

  bitWrite(ADCSRA, ADEN, 0); // Disable the ADC to save power

  // Enable interrupts
  // FALLING: To trigger an interrupt when the pin transits from HIGH to LOW.
  // I'm assusming this means when a target moves closer and thus provides a signal to ping 2
  attachInterrupt(digitalPinToInterrupt(2), interruption, FALLING); // falling edge FALLING, rising edge RISING

  delay(100);

  pinMode(0, OUTPUT); // Balance

  for (int i = 0; i < 2; i++) {

    tone(0, 500, 40);

    delay(60);

    if (frequency != buff) {
      i = 0;
      buff = frequency;
    }

  }

  tone(0, 1000, 40);

  delay(40);

  balanceInit = false;

}

void loop() {

  AutoBal = true;

  if (intensity > sens && Fe) { //Ferrous metals

    tone(0, toneFe, 40);

    delay(40);

    delay(40 - (constrain(intensity, 10, 40)));

    AutoBal = false; //to reset the autobalance

  } else if (intensity > sens && !Fe) { // Nonferrous metals

    tone(0, toneNoFe, 40);

    delay(40);

    delay(40 - (constrain(intensity, 10, 40)));

    AutoBal = false; //to reset the autobalance

  }

  if (true) { // auto Balance

    if (AutoBal && intensity != 0) {

      if (autoBalance > 1000) {
        autoBalance = 0;
        buff = frequency;
      }

      autoBalance++;

    } else autoBalance = 0;

    delay(1);

  }

}
