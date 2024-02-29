#include <avr/io.h>
#include <stddef.h>
#define F_CPU 16000000UL
#include "util/delay.h"

// Buzzer parameters
//----------------------------------------------------------
#define DUTYCYCLE 0.1   // duty cycle of the sound wave

#define BUZZERP_PIN PIND5       // passive buzzer pin
#define BUZZERP_PORT &PORTD     // passive buzzer port
#define BUZZERP_REG &DDRD       // passive buzzer register
#define BUZZERP_TIME 200        // time duration ms

#define BUZZERA_PIN PINC0       // active buzzer pin
#define BUZZERA_PORT &PORTC     // active buzzer port
#define BUZZERA_REG &DDRC       // active buzzer register
#define BUZZERA_TIME 100        // time duration ms

//----------------------------------------------------------

struct Buzzer {
  uint8_t pin;
  volatile uint8_t *port;   
  volatile uint8_t *reg;     
  int time;               
  float dutyCycle;  
};

void playSound(float F, Buzzer *buzzer) {
  float T = 1/F*1e6;  //1/F is in second
  int Ton = (buzzer->dutyCycle)*T;
  int Toff = T - Ton;

  for (int j = 0; j < (buzzer->time*1e3)/T; j++) {
    *(buzzer->port) = *(buzzer->port) | (1 << buzzer->pin);
    for(int i = 0; i < Ton; i++){
      _delay_us(1);
    }
    *(buzzer->port) = *(buzzer->port) & ~(1 << buzzer->pin);
    for(int i = 0; i < Toff; i++){
      _delay_us(1);
    }
  }
}

int main(void) {
  // initialize the buzzer data structs
  Buzzer buzzerP = {BUZZERP_PIN, 
                    BUZZERP_PORT, 
                    BUZZERP_REG,
                    BUZZERP_TIME,
                    DUTYCYCLE};
  Buzzer buzzerA = {BUZZERA_PIN, 
                    BUZZERA_PORT,
                    BUZZERA_REG, 
                    BUZZERA_TIME,
                    DUTYCYCLE};
  // Activate all pins in register
  *(buzzerP.reg) = 0xFF;
  *(buzzerA.reg) = 0xFF;

  Buzzer* buzzer = nullptr; // pointer to buzzer struct
  // Octave frequencies starting from C5
  float note5[] = {523.25, 587.33, 659.26, 698.46, 783.99, 880.00, 987.77};
  float *pNote5 = note5;
  int buzzerDuration;

  while(1){
    for (size_t i = 0; i < sizeof(note5)/sizeof(float); i++) {
      if (i%2 == 0){
        buzzer = &buzzerP; // even notes
      } else {
        buzzer = &buzzerA; //odd notes
      }
      playSound(*(pNote5+i), buzzer);
      _delay_ms(100);
    }
    _delay_ms(500);
  }

}