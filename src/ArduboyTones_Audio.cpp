/**
 * @file ArduboyTones.cpp
 * \brief An Arduino library for playing tones and tone sequences, 
 * intended for the Arduboy game system.
 */


#if defined(__SAMD51__)

#include "ArduboyTones.h"
#include <Audio.h>
#include "Adafruit_Arcada.h"

extern AudioSynthWaveformSine   sine1, sine2;
extern Adafruit_Arcada arcada;

// pointer to a function that indicates if sound is enabled
static bool (*outputEnabled)();

static volatile long durationToggleCount = 0;
static volatile bool tonesPlaying = false;
static volatile bool toneSilent;
#ifdef TONES_VOLUME_CONTROL
static volatile bool toneHighVol;
static volatile bool forceHighVol = false;
static volatile bool forceNormVol = false;
#endif

static volatile uint16_t *tonesStart;
static volatile uint16_t *tonesIndex;
static volatile uint16_t toneSequence[MAX_TONES * 2 + 1];
static volatile bool inProgmem;


volatile int32_t duration = 0;
volatile int32_t second = 0;
void TimerCallback()
{
  /*
  if (second == 0) {
    second = 1000;
    Serial.print(".");
  } else {
    second--;
  }*/
 
  if (duration > 0) {
    duration--;
    if (duration == 0) {
      ArduboyTones::nextTone();
    }
  }
}

ArduboyTones::ArduboyTones(boolean (*outEn)())
{
  outputEnabled = outEn;

  toneSequence[MAX_TONES * 2] = TONES_END;
}

static boolean initialized = false;
void ArduboyTones::begin(void) {
  AudioMemory(2);
  arcada.timerCallback(1000, TimerCallback);
  arcada.enableSpeaker(true);
  initialized = true;
}

void ArduboyTones::tone(uint16_t freq, uint16_t dur)
{
  if (!initialized) { begin(); }

  //Serial.println("Tone");
  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq;
  toneSequence[1] = dur;
  toneSequence[2] = TONES_END; // set end marker
  nextTone(); // start playing
}

void ArduboyTones::tone(uint16_t freq1, uint16_t dur1,
                        uint16_t freq2, uint16_t dur2)
{
  if (!initialized) { begin(); }

  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = TONES_END; // set end marker
  nextTone(); // start playing
}

void ArduboyTones::tone(uint16_t freq1, uint16_t dur1,
                        uint16_t freq2, uint16_t dur2,
                        uint16_t freq3, uint16_t dur3)
{
  if (!initialized) { begin(); }

  inProgmem = false;
  tonesStart = tonesIndex = toneSequence; // set to start of sequence array
  toneSequence[0] = freq1;
  toneSequence[1] = dur1;
  toneSequence[2] = freq2;
  toneSequence[3] = dur2;
  toneSequence[4] = freq3;
  toneSequence[5] = dur3;
  // end marker was set in the constructor and will never change
  nextTone(); // start playing
}

void ArduboyTones::tones(const uint16_t *tones)
{
  if (!initialized) { begin(); }

  inProgmem = true;
  tonesStart = tonesIndex = (uint16_t *)tones; // set to start of sequence array
  nextTone(); // start playing
}

void ArduboyTones::tonesInRAM(uint16_t *tones)
{
  if (!initialized) { begin(); }

  inProgmem = false;
  tonesStart = tonesIndex = tones; // set to start of sequence array
  nextTone(); // start playing
}

void ArduboyTones::noTone()
{
  if (!initialized) { begin(); }

  sine1.amplitude(0);
  sine2.amplitude(0);
  tonesPlaying = false;
}

void ArduboyTones::volumeMode(uint8_t mode)
{
  if (!initialized) { begin(); }

#ifdef TONES_VOLUME_CONTROL
  forceNormVol = false; // assume volume is tone controlled
  forceHighVol = false;

  if (mode == VOLUME_ALWAYS_NORMAL) {
    forceNormVol = true;
  }
  else if (mode == VOLUME_ALWAYS_HIGH) {
    forceHighVol = true;
  }
#endif
}

bool ArduboyTones::playing()
{
  if (!initialized) { begin(); }

  return tonesPlaying;
}

void ArduboyTones::nextTone()
{
  if (!initialized) { begin(); }

  uint16_t freq;
  uint16_t dur;
  long toggleCount;
  uint32_t ocrValue;
#ifdef TONES_ADJUST_PRESCALER
  uint8_t tccrxbValue;
#endif

  //Serial.print("Next tone: ");
  freq = getNext(); // get tone frequency
  if (freq == TONES_END) { // if freq is actually an "end of sequence" marker
    noTone(); // stop playing
    //Serial.println("END");
    return;
  }
  tonesPlaying = true;

  if (freq == TONES_REPEAT) { // if frequency is actually a "repeat" marker
    tonesIndex = tonesStart; // reset to start of sequence
    //Serial.println("REPEAT ");
    freq = getNext();
  }

#ifdef TONES_VOLUME_CONTROL
  if (((freq & TONE_HIGH_VOLUME) || forceHighVol) && !forceNormVol) {
    toneHighVol = true;
  }
  else {
    toneHighVol = false;
  }
#endif

  freq &= ~TONE_HIGH_VOLUME; // strip volume indicator from frequency
  // Serial.print(", Freq: "); Serial.print(freq);
  sine1.frequency(freq);
  sine1.amplitude(0.5);

  if (!outputEnabled()) { // if sound has been muted
    //Serial.print(" MUTE ");
    toneSilent = true;
    sine1.amplitude(0);
  }

#ifdef TONES_VOLUME_CONTROL
  if (toneHighVol && !toneSilent) {
    //Serial.print(" LOUD ");
    sine1.amplitude(1.0);
  }
#endif

  duration = getNext(); // get tone duration
  //Serial.print(" for "); Serial.println(duration);
}

uint16_t ArduboyTones::getNext()
{
  if (inProgmem) {
    return pgm_read_word(tonesIndex++);
  }
  return *tonesIndex++;
}

#endif
