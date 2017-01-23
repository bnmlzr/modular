#include <MozziGuts.h>
#include <Oscil.h>
#include <mozzi_midi.h>
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>

#define CONTROL_RATE 1024

int waveform, octave;
uint8_t key;
int semi, semicv;

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> VCO(SIN2048_DATA);

void setup(){
  startMozzi(CONTROL_RATE);
  waveform = 0;
}

void updateControl(){
  octave = (mozziAnalogRead(7) >> 8) * 12;
  key = map(mozziAnalogRead(1), 0, 1023, 12, 72);
  semi = map(mozziAnalogRead(6), 0, 1023, 0, 11);
  semicv = map(mozziAnalogRead(0), 0, 1023, 0, 11);
  
  Q16n16 midi_note = Q8n0_to_Q16n16(key+octave+semi+semicv);
  VCO.setFreq_Q24n8(Q16n16_to_Q24n8(Q16n16_mtof(midi_note)));
  
  waveform = mozziAnalogRead(5) >> 8;
  updateWaveform(waveform);
}

void updateWaveform(int waveform) {
  switch (waveform) {
    case 0:
      VCO.setTable(SIN2048_DATA);
      break;
    case 1:
      VCO.setTable(SAW2048_DATA);
      break;
    case 2:
      VCO.setTable(TRIANGLE2048_DATA);
      break;
    case 3:
      VCO.setTable(SQUARE_NO_ALIAS_2048_DATA);
      break;
  }
}

int updateAudio(){
  return VCO.next() << 6;
}

void loop(){
  audioHook();
}
