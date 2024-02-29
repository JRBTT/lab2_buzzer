#include <avr/io.h>
#include <stddef.h>
#define F_CPU 16000000UL
#include "util/delay.h"
//--------------------------------------------------------------------------------------
// Buzzer parameters
//--------------------------------------------------------------------------------------
#define DUTYCYCLE 0.1           // duty cycle of the sound wave

#define BUZZERP_PIN PIND5       // passive buzzer pin
#define BUZZERP_PORT &PORTD     // passive buzzer port
#define BUZZERP_REG &DDRD       // passive buzzer register
#define BUZZERP_TIME 200        // time duration ms

#define BUZZERA_PIN PINC0       // active buzzer pin
#define BUZZERA_PORT &PORTC     // active buzzer port
#define BUZZERA_REG &DDRC       // active buzzer register
#define BUZZERA_TIME 200        // time duration ms

//--------------------------------------------------------------------------------------
// Song parameters: Ode to Joy - Beethoven
//--------------------------------------------------------------------------------------
#define OCTAVE 4                  // octave of the note 4, 5 or 6
const char* melody[] = {          // song notes 
  "E","E","F","G","G","F","E","D","C","C","D","E","E","D","D",
  "E","E","F","G","G","F","E","D","C","C","D","E","D","C","C"
};
int doubleDuration[] = {15, 30};  // notes with double duration

//--------------------------------------------------------------------------------------
struct Buzzer {
  uint8_t pin;
  volatile uint8_t *port;   
  volatile uint8_t *reg;     
  int time;               
  float dutyCycle;  
};

void playSound(float F, Buzzer *buzzer, int doubleDuration[], int noteIndex) {
  float T = 1/F*1e6;  //1/F is in second
  int Ton = (buzzer->dutyCycle)*T;
  int Toff = T - Ton;
  int buzzerTime = buzzer->time;
  for (int i = 0; i < 2; i++){
    if (noteIndex == (doubleDuration[i]-1)) {
      buzzerTime = buzzer->time*2;
    }
  }
  int j = 0;
  while (j < (buzzerTime*1e3)/T) {
    *(buzzer->port) = *(buzzer->port) | (1 << buzzer->pin);
    for(int i = 0; i < Ton; i++){
      _delay_us(1);
    }
    *(buzzer->port) = *(buzzer->port) & ~(1 << buzzer->pin);
    for(int i = 0; i < Toff; i++){
      _delay_us(1);
    }
    j++;
  }
}

float getFrequency(const char *noteNames[], float frequency[][12], 
                   const char *note, int octave) {
  for (int i = 0; i < 12; i++) {
    if (note == noteNames[i]) {
      return frequency[octave-4][i]; 
    } 
  }
  return 0;
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
  Buzzer* buzzer = nullptr; // pointer to buzzer struct
  // Activate all pins in register
  *(buzzerP.reg) = 0xFF;
  *(buzzerA.reg) = 0xFF;

  // notes starting from C4 ending at B6
  float notes[3][12] = {
    {261.63, 277.18, 293.66, 311.13, 329.63, 349.23,
     369.99, 392.00, 415.30, 440.00, 466.16, 493.88},
    {523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 
     739.99, 783.99, 830.61, 880.00, 932.33, 987.77}, 
    {1046.50, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 
     1479.98, 1567.98, 1661.22, 1760.00, 1864.66, 1975.53}
  };
  // note names
  const char *noteNames[] = {"C", "C#", "D", "D#", "E", "F", 
                             "F#", "G", "G#", "A", "A#", "B"};

  size_t melodySize = sizeof(melody)/sizeof(const char*);
  while(1){
    for (size_t i = 0; i < melodySize; i++) {
      float noteFrequency = getFrequency(noteNames, notes, melody[i], OCTAVE);
      if (i%2 == 0){
        buzzer = &buzzerP; // even notes
      } else {
        buzzer = &buzzerA; //odd notes
      }
      playSound(noteFrequency, buzzer, doubleDuration, i);
      _delay_ms(100);
    }
    _delay_ms(500);
  }

}