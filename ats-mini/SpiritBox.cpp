#include "Common.h"
#include "SpiritBox.h"
#include "Storage.h"
#include "Utils.h"
#include "Draw.h"

#include <string.h>

namespace
{
  enum Field { RUN, BAND, INTERVAL, DIRECTION, STEP, VOLUME, EXIT, FIELD_COUNT };
  const char *const labels[] = {"Run / Stop", "Band", "Interval", "Direction", "Step", "Volume", "Exit"};
  const uint16_t intervals[] = {100, 150, 200, 250, 300, 350};
  const uint32_t fmSteps[] = {100000, 50000};
  const uint32_t amSteps[] = {9000, 1000};

  bool active = false;
  bool running = false;
  bool editing = false;
  bool mw = false;
  bool reverse = false;
  uint8_t field = RUN;
  uint8_t intervalIdx = 0;
  uint8_t stepIdx[2] = {};
  uint32_t lastTune = 0;
  uint32_t lastDraw = 0;
  int normalBand = 0;
  int fmBand = -1;
  int mwBand = -1;
  VfoState normalVfo;
  bool normalAvcHold = false;
  ColorTheme palette;

  struct BandSnapshot { uint8_t idx; Band value; } savedBands[3];
  uint8_t savedCount = 0;

  void rememberBand(uint8_t idx)
  {
    for(uint8_t i=0 ; i<savedCount ; i++)
      if(savedBands[i].idx==idx) return;
    savedBands[savedCount++] = {idx, bands[idx]};
  }

  int wrap(int value, int delta, int count)
  {
    return ((value + delta) % count + count) % count;
  }

  uint32_t low() { return mw? 531000 : 87500000; }
  uint32_t high() { return mw? 1602000 : 108000000; }
  uint32_t step() { return mw? amSteps[stepIdx[1]] : fmSteps[stepIdx[0]]; }
  uint32_t lastFrequency() { return low() + (high() - low()) / step() * step(); }

  void tune(uint32_t freq)
  {
    updateFrequency(freq, false);
    lastTune = millis();
  }

  void configureBand()
  {
    running = false;
    int idx = mw? mwBand : fmBand;
    bands[idx].bandMode = mw? AM : FM;
    bands[idx].currentFreq = reverse? lastFrequency() : low();
    bands[idx].currentStepIdx = 0;
    selectBand(idx, false);
    muteOn(MUTE_SQUELCH, false);
    // A band power-up can reset receiver properties; retain explicit user mute.
    if(muteOn(MUTE_MAIN)) rx.setAudioMute(true);
    lastTune = millis();
  }

  void toggleRun()
  {
    running = !running;
    // Starting resumes from the displayed point; no burst of missed steps.
    lastTune = millis();
  }
}

bool spiritBoxActive() { return active; }
ColorTheme &spiritBoxTheme() { return palette; }
int spiritBoxStoredBandIndex() { return active? normalBand : bandIdx; }
VfoState spiritBoxNormalVfo() { return normalVfo; }

const Band &spiritBoxStoredBand(uint8_t idx)
{
  if(active)
    for(uint8_t i=0 ; i<savedCount ; i++)
      if(savedBands[i].idx==idx) return savedBands[i].value;
  return bands[idx];
}

void spiritBoxEnter()
{
  if(active) return;
  fmBand = mwBand = -1;
  for(int i=0 ; i<getTotalBands() ; i++)
  {
    if(fmBand<0 && bands[i].bandType==FM_BAND_TYPE &&
       bands[i].minimumFreq<=87500000 && bands[i].maximumFreq>=108000000) fmBand = i;
    if(!strcmp(bands[i].bandName, "MW2") && bands[i].minimumFreq<=531000 &&
       bands[i].maximumFreq>=1602000) mwBand = i;
  }
  if(fmBand<0 || mwBand<0)
  {
    statusShow("Spirit Box unavailable", "Required FM/MW bands missing");
    return;
  }

  // Commit the normal baseline before entering; later save paths use snapshots.
  prefsSave();
  normalBand = bandIdx;
  normalVfo = captureVfo();
  normalAvcHold = ssbAvcHold;
  savedCount = 0;
  rememberBand(normalBand);
  rememberBand(fmBand);
  rememberBand(mwBand);

  // Copy fields, never edit a user's palette or the persisted theme index.
  palette = theme[themeIdx];
  palette.name = "Spirit Box";
  palette.bg = 0x0802;
  palette.text = palette.freq_text = 0xFFFF;
  palette.text_muted = palette.funit_text = 0xCE7F;
  palette.menu_bg = 0x1806;
  palette.menu_border = palette.freq_hl = 0xA31F;
  palette.menu_hl_bg = 0x5011;
  palette.menu_hl_text = palette.menu_item = 0xFFFF;

  mw = reverse = editing = running = false;
  field = RUN;
  intervalIdx = 0;
  stepIdx[0] = stepIdx[1] = 0;
  active = true;
  currentCmd = CMD_SPIRITBOX;
  configureBand();
  lastDraw = millis();
}

void spiritBoxExit()
{
  if(!active) return;
  running = false;
  active = false;
  editing = false;
  for(uint8_t i=0 ; i<savedCount ; i++)
    bands[savedBands[i].idx] = savedBands[i].value;
  currentCmd = CMD_NONE;
  selectBand(normalBand, false);
  if(normalAvcHold && isSSB())
  {
    ssbAvcHold = true;
    rx.setSSBAutomaticVolumeControl(0);
  }
  clearStationInfo();
  // Restore normal squelch immediately, including when leaving via shutdown.
  rx.getCurrentReceivedSignalQuality();
  uint8_t value = currentSquelch[currentMode];
  uint8_t quality = value & 0x80? rx.getCurrentSNR() : rx.getCurrentRSSI();
  muteOn(MUTE_SQUELCH, (value & 0x7f) && quality < (value & 0x7f));
  if(muteOn(MUTE_MAIN)) rx.setAudioMute(true);
}

bool spiritBoxTick()
{
  if(!active) return false;
  uint32_t now = millis();
  if(running && now - lastTune >= intervals[intervalIdx])
  {
    uint32_t freq;
    if(reverse)
      freq = currentFrequency < low() + step()? lastFrequency() : currentFrequency - step();
    else
      freq = currentFrequency >= lastFrequency()? low() : currentFrequency + step();
    // Capture start before calling a library routine with a blocking tune delay.
    tune(freq);
    lastTune = now;
  }
  if(now - lastDraw < 100) return false;
  lastDraw = now;
  return true;
}

bool spiritBoxRotate(int16_t delta)
{
  if(!active || !delta) return false;
  if(!editing)
    field = wrap(field, delta * scrollDirection, FIELD_COUNT);
  else switch(field)
  {
    case BAND:
      mw = wrap(mw, delta, 2);
      configureBand();
      break;
    case INTERVAL: intervalIdx = wrap(intervalIdx, delta, ITEM_COUNT(intervals)); break;
    case DIRECTION: reverse = wrap(reverse, delta, 2); break;
    case STEP:
      stepIdx[mw] = wrap(stepIdx[mw], delta, 2);
      tune(low() + (currentFrequency - low()) / step() * step());
      break;
    case VOLUME: doVolume(delta); break;
  }
  return true;
}

bool spiritBoxClick(bool shortPress)
{
  if(!active) return false;
  if(shortPress) { editing = false; toggleRun(); }
  else if(editing) editing = false;
  else if(field==RUN) toggleRun();
  else if(field==EXIT) spiritBoxExit();
  else editing = true;
  return true;
}

void spiritBoxTimeout() { editing = false; }

void spiritBoxDraw()
{
  spr.fillSprite(TH.bg);
  spr.setTextDatum(TL_DATUM);
  spr.setTextColor(TH.text);
  spr.drawString("Spirit Box", 8, 4, FONT_LARGE);
  drawWiFiIndicator(224, 0);
  drawBleIndicator(226, 20);
  drawBattery(288, 0);
  // Small static ghost drawn with the existing graphics primitives.
  spr.fillCircle(202, 13, 9, TH.text_muted);
  spr.fillRect(193, 13, 19, 13, TH.text_muted);
  for(int x=193 ; x<212 ; x+=6) spr.fillTriangle(x, 26, x+3, 22, x+6, 26, TH.bg);
  spr.fillCircle(199, 13, 2, TH.bg);
  spr.fillCircle(206, 13, 2, TH.bg);

  char text[64];
  spr.setTextDatum(TL_DATUM);
  spr.setTextColor(TH.text);
  if(mw) snprintf(text, sizeof(text), "%lu kHz", (unsigned long)(currentFrequency / 1000));
  else snprintf(text, sizeof(text), "%lu.%02lu MHz", (unsigned long)(currentFrequency / 1000000),
                (unsigned long)(currentFrequency % 1000000 / 10000));
  spr.drawString(text, 8, 39, FONT_LARGE);
  snprintf(text, sizeof(text), "%s  %s  %u ms", running? "Running" : "Stopped", reverse? "REV" : "FWD", intervals[intervalIdx]);
  spr.setTextColor(TH.text_muted);
  spr.drawString(text, 8, 72, FONT_SMALL);

  char value[24] = "";
  switch(field)
  {
    case RUN: snprintf(value, sizeof(value), "%s", running? "Stop" : "Start"); break;
    case BAND: snprintf(value, sizeof(value), "%s", mw? "AM / MW" : "FM"); break;
    case INTERVAL: snprintf(value, sizeof(value), "%u ms", intervals[intervalIdx]); break;
    case DIRECTION: snprintf(value, sizeof(value), "%s", reverse? "Reverse" : "Forward"); break;
    case STEP: snprintf(value, sizeof(value), "%lu kHz", (unsigned long)(step() / 1000)); break;
    case VOLUME: snprintf(value, sizeof(value), "%u%s", volume, muteOn(MUTE_MAIN)? " (muted)" : ""); break;
    case EXIT: snprintf(value, sizeof(value), "Radio"); break;
  }
  spr.fillRoundRect(4, 99, 312, 38, 4, TH.menu_hl_bg);
  spr.setTextColor(TH.menu_hl_text);
  snprintf(text, sizeof(text), "%s: %s%s", labels[field], value, editing? " *" : "");
  spr.drawString(text, 10, 108, FONT_SMALL);
  spr.setTextColor(TH.text_muted);
  spr.drawString(editing? "Turn: change   Click: done" : "Turn: select   Click: open", 8, 140, FONT_SMALL);
  spr.drawString("Short press: start / stop", 8, 156, FONT_DEFAULT);
  spr.pushSprite(0, 0);
}
