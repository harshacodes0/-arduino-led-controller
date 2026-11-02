/*
 * LED CONTROLLER FIRMWARE - Wokwi / Arduino Uno (ATmega328P) version
 * ------------------------------------------------------------------
 * Timer1 CTC interrupt (1 kHz) drives LED patterns + software PWM on
 * pin 13 (through 330 ohm). UART commands (9600 baud) via the Wokwi
 * Serial Monitor select the pattern and brightness.
 *
 * Commands (newline terminated, case-insensitive):
 *   HELP | STATUS | DUTY
 *   MODE <OFF|SOLID|SLOW|FAST|HEART>
 *   BRIGHT <0-100>
 * DUTY prints the measured LED on-time percentage since the previous
 * DUTY call (use it as test evidence, e.g. MODE SLOW, wait 4 s, DUTY).
 *
 * Timer: 16 MHz / 64 / 1000 Hz - 1 => OCR1A = 249 (exact 1 ms tick).
 */
#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#define PWM_PERIOD_TICKS 10u
#define CMD_MAX_LEN      24u
#define WDT_TIMEOUT_MS   500u
#define LED_BIT          5          /* PORTB5 = Arduino pin 13 */

enum LedMode : uint8_t { MODE_OFF, MODE_SOLID, MODE_SLOW, MODE_FAST, MODE_HEART, MODE_COUNT };
static const char *const MODE_NAMES[MODE_COUNT] = { "OFF", "SOLID", "SLOW", "FAST", "HEART" };

/* Shared between ISR and main loop -> volatile */
static volatile uint32_t g_ms, g_phase_ms, g_wdt_ms;
static volatile uint32_t g_on_ticks, g_tot_ticks;
static volatile uint8_t  g_mode = MODE_OFF;
static volatile uint8_t  g_brightness = 100;
static volatile uint8_t  g_pwm_cnt;
static volatile bool     g_wdt_expired;

static char    g_line[CMD_MAX_LEN];
static uint8_t g_line_len;
static bool    g_line_overflow;

static bool pattern_level(uint8_t m, uint32_t ph)
{
  switch (m) {
    case MODE_SOLID: return true;
    case MODE_SLOW:  return (ph % 1000u) < 500u;      /* 1 Hz, 50 %  */
    case MODE_FAST:  return (ph % 200u)  < 100u;      /* 5 Hz, 50 %  */
    case MODE_HEART: { uint32_t t = ph % 1000u;       /* double pulse */
                       return (t < 100u) || (t >= 200u && t < 300u); }
    default:         return false;
  }
}

/* 1 kHz tick: O(1), no printing, no loops */
ISR(TIMER1_COMPA_vect)
{
  g_ms++; g_phase_ms++; g_wdt_ms++;
  g_pwm_cnt = (g_pwm_cnt + 1u) % PWM_PERIOD_TICKS;

  uint8_t on_ticks = (uint8_t)(((uint16_t)g_brightness * PWM_PERIOD_TICKS + 50u) / 100u);
  bool lvl = pattern_level(g_mode, g_phase_ms) && (g_pwm_cnt < on_ticks);

  if (lvl) { PORTB |=  (1 << LED_BIT); g_on_ticks++; }
  else     { PORTB &= ~(1 << LED_BIT); }
  g_tot_ticks++;

  if (g_wdt_ms >= WDT_TIMEOUT_MS) g_wdt_expired = true;
}

static void timer1_init(void)
{
  cli();
  TCCR1A = 0;
  TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);  /* CTC, /64 */
  OCR1A  = 249;
  TCNT1  = 0;
  TIMSK1 = (1 << OCIE1A);
  sei();
}

static void wdt_feed(void)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { g_wdt_ms = 0; g_wdt_expired = false; }
}

static void set_mode(uint8_t m)
{
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { g_mode = m; g_phase_ms = 0; }
}

static void cmd_execute(char *line)
{
  for (char *p = line; *p; p++) *p = toupper((unsigned char)*p);
  char *cmd   = strtok(line, " ");
  char *arg   = strtok(NULL, " ");
  char *extra = strtok(NULL, " ");
  if (!cmd) return;
  if (extra) { Serial.println(F("ERR TOO MANY ARGS")); return; }

  if (!strcmp(cmd, "HELP")) {
    Serial.println(F("CMDS: HELP STATUS DUTY MODE <OFF|SOLID|SLOW|FAST|HEART> BRIGHT <0-100>"));
  } else if (!strcmp(cmd, "STATUS")) {
    uint32_t ms; ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ms = g_ms; }
    Serial.print(F("MODE=")); Serial.print(MODE_NAMES[g_mode]);
    Serial.print(F(" BRIGHT=")); Serial.print(g_brightness);
    Serial.print(F(" UPTIME=")); Serial.print(ms); Serial.println(F("ms"));
  } else if (!strcmp(cmd, "DUTY")) {
    uint32_t on, tot;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { on = g_on_ticks; tot = g_tot_ticks; g_on_ticks = 0; g_tot_ticks = 0; }
    Serial.print(F("DUTY=")); Serial.print(tot ? (100.0 * on / tot) : 0.0, 1);
    Serial.print(F("% over ")); Serial.print(tot); Serial.println(F(" ms"));
  } else if (!strcmp(cmd, "MODE")) {
    if (!arg) { Serial.println(F("ERR MISSING ARG")); return; }
    for (uint8_t m = 0; m < MODE_COUNT; m++)
      if (!strcmp(arg, MODE_NAMES[m])) { set_mode(m); Serial.println(F("OK")); return; }
    Serial.println(F("ERR BAD MODE"));
  } else if (!strcmp(cmd, "BRIGHT")) {
    if (!arg) { Serial.println(F("ERR MISSING ARG")); return; }
    char *end; long v = strtol(arg, &end, 10);
    if (*end != 0 || end == arg || v < 0 || v > 100) { Serial.println(F("ERR RANGE 0-100")); return; }
    g_brightness = (uint8_t)v;
    Serial.println(F("OK"));
  } else {
    Serial.println(F("ERR UNKNOWN CMD"));
  }
}

static void uart_poll(void)
{
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      if (g_line_overflow)      Serial.println(F("ERR LINE TOO LONG"));
      else if (g_line_len > 0) { g_line[g_line_len] = 0; cmd_execute(g_line); }
      g_line_len = 0; g_line_overflow = false;
    } else if (g_line_len < CMD_MAX_LEN - 1u) {
      g_line[g_line_len++] = c;
    } else {
      g_line_overflow = true;
    }
  }
}

void setup()
{
  DDRB |= (1 << LED_BIT);          /* pin 13 as output */
  Serial.begin(9600);
  timer1_init();
  Serial.println(F("LED controller ready. Type HELP."));
}

void loop()
{
  uart_poll();
  wdt_feed();
  /* Real hardware: add set_sleep_mode(SLEEP_MODE_IDLE); sleep_mode();
     here to save power between ticks (see power budget). */
}


