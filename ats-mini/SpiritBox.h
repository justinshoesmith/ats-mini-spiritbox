#ifndef SPIRITBOX_H
#define SPIRITBOX_H

#include "Menu.h"
#include "Themes.h"

bool spiritBoxActive();
void spiritBoxEnter();
void spiritBoxExit();
bool spiritBoxTick();
bool spiritBoxRotate(int16_t delta);
bool spiritBoxClick(bool shortPress);
void spiritBoxTimeout();
void spiritBoxDraw();
ColorTheme &spiritBoxTheme();

// Save normal-radio state even while the receiver is temporarily sweeping.
int spiritBoxStoredBandIndex();
const Band &spiritBoxStoredBand(uint8_t idx);
VfoState spiritBoxNormalVfo();

#endif
