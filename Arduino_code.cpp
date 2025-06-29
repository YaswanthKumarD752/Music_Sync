/*****************************************************************
  ESP32  +  MAX9814  +  75-LED 12 V WS2811 strip
  Music-reactive rainbow VU bar (more sensitive)   –  v2
*****************************************************************/
#include <Adafruit_NeoPixel.h>

/* ─── USER TUNABLES ─────────────────────────────────────────── */
#define LED_PIN      14
#define NUM_LEDS     75
#define MIC_PIN      36
#define BRIGHT_MAX   220     // bump brightness a little
#define THRESHOLD     60     // lower → more sensitive to quiet beats
#define MAP_MAX      600     // lower → bar fills with softer music
#define FALL_RATE      1     // 1–5 (higher = faster drop)
/* ───────────────────────────────────────────────────────────── */

/* ---------- NeoPixel strip object ---------- */
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

/* ---------- runtime vars ---------- */
uint16_t baseline;           // DC bias from MAX9814
uint16_t smoothAmp = 0;      // smoothed amplitude
uint16_t displayed  = 0;     // bar height after fall-off

/* Helper: rainbow colour wheel 0-255 → RGB */
uint32_t colorWheel(uint8_t pos) {
  pos = 255 - pos;
  if (pos < 85)  return strip.Color(255 - pos * 3, 0, pos * 3);          // Red→Blue
  if (pos < 170) { pos -= 85; return strip.Color(0, pos * 3, 255 - pos * 3); }
  pos -= 170;    return strip.Color(pos * 3, 255 - pos * 3, 0);           // Green→Red
}

/* ─── setup ─────────────────────────────────────────────────── */
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setBrightness(BRIGHT_MAX);
  strip.show();
  analogReadResolution(12);                                      // 0-4095

  /* Baseline auto-calibration (≈1 s) */
  uint32_t sum = 0;
  const int samples = 500;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(MIC_PIN);
    delay(2);
  }
  baseline = sum / samples;
  Serial.print("Baseline = "); Serial.println(baseline);
}

/* ─── loop ──────────────────────────────────────────────────── */
void loop() {

  /* 1. Read mic & smooth */
  int raw          = analogRead(MIC_PIN);
  int32_t centered = raw - baseline;          // remove DC bias
  uint16_t amp     = abs(centered);           // 0-2047
  smoothAmp        = (smoothAmp * 3 + amp) >> 2;  // 1/4 smoothing

  /* 2. Threshold gate */
  uint16_t audio = (smoothAmp > THRESHOLD) ? smoothAmp - THRESHOLD : 0;

  /* 3. Map loudness → LED count */
  uint16_t lit = map(audio, 0, MAP_MAX, 0, NUM_LEDS);
  lit = constrain(lit, 0, NUM_LEDS);

  /* 4. Fall-off for smoother decay */
  if (lit > displayed)             displayed = lit;
  else if (displayed > FALL_RATE)  displayed -= FALL_RATE;
  else                             displayed = 0;

  /* 5. Render rainbow gradient bar */
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    if (i < displayed) {
      uint8_t wheelPos = map(i, 0, NUM_LEDS - 1, 0, 255);  // full rainbow span
      strip.setPixelColor(i, colorWheel(wheelPos));
    } else {
      strip.setPixelColor(i, 0);
    }
  }
  strip.show();
  delay(10);   // ~100 fps
}