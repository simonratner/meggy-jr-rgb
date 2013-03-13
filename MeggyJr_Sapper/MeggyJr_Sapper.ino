/*
 MeggyJr_Sapper
 Minesweeper clone for the Meggy Jr RGB.

 Copyright (c) 2009 Simon Ratner. All right reserved.
 <https://github.com/simonratner/meggy-jr-rgb>

 This library is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MeggyJrSimple.h>

#define MineColor1     CustomColor4
#define MineColor2     CustomColor5
#define FlagColor1     Violet
#define FlagColor2     DimViolet
#define HiddenColor    CustomColor0
#define CursorColor    White

byte MarkerColor[] = {
  Dark,
  DimBlue,   // 1
  DimGreen,  // 2
  DimYellow, // 3
  DimOrange, // 4
  Blue,      // 5
  Green,     // 6
  Yellow,    // 7
  Orange,    // 8
};

// higher 4 bits are used for special flags
#define Hidden 16
#define Flag   32
#define Mine   64

byte SpecialColor[] = {
  Dark,
  HiddenColor, // Hidden
  FlagColor1,  // Flag
  FlagColor1,  // Flag | Hidden
  MineColor1,  // Mine
  HiddenColor, // Mine | Hidden
  FlagColor1,  // Mine | Flag
  FlagColor1,  // Mine | Flag | Hidden
};

#define len(arr) (sizeof(arr)/sizeof(arr[0]))
unsigned int TuneBlip[3] = {5730, 0, 7648};
unsigned int TuneWin[5] = {7648, 0, 5730, 0, 4048};
unsigned int TuneLose[5] = {600, 800, 1000, 1200, 2400};

#define MAX_MINES 10
byte mines;
byte flags;
byte hidden;
byte field[8][8];
byte cursorx;
byte cursory;

unsigned long lastButtonTime;

// current game state (splash, play, win, lose, gameOver)
void (*gameFunc)();

#define getMines(x,y) (field[(y)][(x)] & B00001111)
#define getFlags(x,y) ((field[(y)][(x)] >> 4) & B00001111)

// something whacky going on with typedefs; are they not supported?
// typedef void (*operation)(byte, byte, byte, byte, void*);

// execute op(x, y, context) for each cell.
void forEach(void (*op)(byte, byte, void*), void* context)
{
  for (byte y = 0; y < 8; ++y) {
    for (byte x = 0; x < 8; ++x) {
      op(x, y, context);
    }
  }
}

// execute op(x, y, context) for each cell that is within radius r of
// the specified reference cell.
void forEachNeighbor(byte refx, byte refy, byte r, void (*op)(byte, byte, void*), void* context)
{
  for (byte y = max(refy - r, 0); y <= min(refy + r, 7); ++y) {
    for (byte x = max(refx - r, 0); x <= min(refx + r, 7); ++x) {
      op(x, y, context);
    }
  }
}

void setHidden(byte x, byte y, void* context)
{
  field[y][x] = Hidden;
  hidden++;
}

void clearHidden(byte x, byte y, void* context)
{
  if (field[y][x] & Hidden) {
    field[y][x] &= ~Hidden;
    hidden--;
  }
}

void incrementMines(byte x, byte y, void* context)
{
  if (!(field[y][x] & Mine)) {
    field[y][x]++;
  }
}

void accumulateFlags(byte x, byte y, void* context)
{
  if (field[y][x] & Flag) {
    (*((byte*) context))++;
  }
}

void revealFloodFill(byte x, byte y, void* context)
{
  if (field[y][x] & Hidden) {
    reveal(x, y);
  }
}

void revealUnflagged(byte x, byte y, void* context)
{
  if (field[y][x] & Flag) {
    return;
  }
  if (field[y][x] & Hidden) {
    reveal(x, y);
  }
}

void reveal(byte x, byte y)
{
  if (field[y][x] & Mine) {
    forEach(&clearHidden, NULL);
    gameFunc = lose;
  } else if (field[y][x] & Hidden) {
    field[y][x] &= ~(Hidden | Flag);
    hidden--;
    if (getMines(x, y) == 0) {
      forEachNeighbor(x, y, 1, &revealFloodFill, NULL);
    }
  } else {
    byte adjacentFlags = 0;
    forEachNeighbor(x, y, 1, &accumulateFlags, &adjacentFlags);
    if (adjacentFlags == getMines(x, y)) {
      forEachNeighbor(x, y, 1, &revealUnflagged, NULL);
    }
  }
}

void renderPx(byte x, byte y, void* context)
{
  byte flags = getFlags(x, y);
  if (flags) {
    DrawPx(x, y, SpecialColor[flags]);
  } else {
    DrawPx(x, y, MarkerColor[getMines(x, y)]);
  }
}

// splash screen func
void splash()
{
  // spinning LEDs
  int t = (millis() / 60) % 14;
  SetAuxLEDs(t < 7 ? (1 << t) : (1 << (14 - t)));

  CheckButtonsPress();
  if (Button_A | Button_B) {
    playTune(TuneBlip, len(TuneBlip), 50);

    // clear the field
    forEach(&setHidden, NULL);

    // reset colors in case they were previously animating
    SpecialColor[2] = FlagColor1;
    SpecialColor[4] = MineColor1;
    flags = 0;
    cursorx = 0;
    cursory = 7;

    gameFunc = play;
  }
}

// loser func
void lose()
{
  playNoise(TuneLose, len(TuneLose));
  gameFunc = gameOver;
}

// winner func
void win()
{
  playTune(TuneWin, len(TuneWin), 50);
  gameFunc = gameOver;
}

// endgame func
void gameOver()
{
  // animate flag and mine colors
  int t = millis() / 420;
  SpecialColor[2] = t & 1 ? FlagColor1 : FlagColor2;
  SpecialColor[4] = t & 1 ? MineColor1 : MineColor2;
  forEach(&renderPx, NULL);
  splash();
}

// player func
void play()
{
  // we do some custom button handling because the way
  // MJSL does it isn't quite right for our purpose.
  byte buttons = Meg.GetButtons();
  byte buttonsDelta = buttons & ~(lastButtonState);
  Button_Up = (buttonsDelta & 4);
  Button_Down = (buttonsDelta & 8);
  Button_Left = (buttonsDelta & 16);
  Button_Right = (buttonsDelta & 32);
  if (Button_Up | Button_Down | Button_Left | Button_Right) {
    lastButtonTime = millis() + 420;
  } else if (lastButtonTime < millis() - 80) {
    Button_Up = (buttons & 4);
    Button_Down = (buttons & 8);
    Button_Left = (buttons & 16);
    Button_Right = (buttons & 32);
    lastButtonTime = millis();
  }
  Button_B  = (buttonsDelta & 1);
  Button_A = (buttonsDelta & 2);

  if (Button_Up) {
    cursory = (cursory + 1) % 8;
  }
  if (Button_Down) {
    cursory = (cursory + 7) % 8;
  }
  if (Button_Left) {
    cursorx = (cursorx + 7) % 8;
  }
  if (Button_Right) {
    cursorx = (cursorx + 1) % 8;
  }
  if (Button_A) { // toggle flag at cursor
    if (field[cursory][cursorx] & Hidden) {
      field[cursory][cursorx] ^= Flag;
      flags += (field[cursory][cursorx] & Flag) ? 1 : -1;
    }
  }
  if (Button_B) { // reveal at cursor
    // the field is populated on first click
    if (hidden == 64) {
      generate(cursorx, cursory);
    }
    reveal(cursorx, cursory);
  }

  if (field[cursory][cursorx] & Hidden) {
    SetAuxLEDs(0);
  } else {
    SetAuxLEDs((1 << getMines(cursorx, cursory)) - 1);
  }

  if ((hidden == mines) && (flags == mines)) {
    // finished!
    forEach(&clearHidden, NULL);
    gameFunc = win;
  }

  // render the field
  forEach(&renderPx, NULL);

  // suppress cursor while the buttons are down
  Button_B  = (buttons & 1);
  Button_A = (buttons & 2);
  if (!Button_A && !Button_B) {
    DrawPx(cursorx, cursory, CursorColor);
  }
  lastButtonState = buttons;
}

// generate a new playing field, ensuring that no mines are placed
// at the specified coordinates.
void generate(byte immunex, byte immuney)
{
  randomSeed(millis());
  for (mines = 0; mines < MAX_MINES; ++mines) {
    byte x, y;
    do {
      x = random(0, 8);
      y = random(0, 8);
    } while (field[y][x] & Mine || (x == immunex && y == immuney));
    // place a new mine
    field[y][x] |= Mine;
    forEachNeighbor(x, y, 1, &incrementMines, NULL);
  }
}

// play a sequence of notes synchronously.
void playTune(unsigned int* freqs, int count, int tempo)
{
  for (int i = 0; i < count; ++i) {
    Tone_Start(freqs[i], tempo);
    while (MakingSound) {}
  }
}

// play a sequence of noise bursts synchronously, `bursts` is
// an array of alternating burst and pause durations.
void playNoise(unsigned int* bursts, int count)
{
  for (int i = 0; i < count; ++i) {
    for (unsigned int j = 0; j < bursts[i]; ++j) {
      Tone_Start(rand() * (1 - (i & 1)), 10);
    }
  }
}

// run once, when the sketch starts.
void setup()
{
  MeggyJrSimpleSetup();
  EditColor(Blue,      0,  6, 4);
  EditColor(Green,     0, 10, 0);
  EditColor(Yellow,   10, 10, 0);
  EditColor(Orange,   10,  2, 0);
  EditColor(DimBlue,   0,  1, 1);
  EditColor(DimGreen,  0,  2, 0);
  EditColor(DimYellow, 3,  3, 0);
  EditColor(DimOrange, 3,  1, 0);
  EditColor(CustomColor0,  1, 6, 1);  // HiddenColor
  EditColor(CustomColor4, 10, 0, 0);  // MineColor1
  EditColor(CustomColor5,  2, 0, 0);  // MineColor2

  gameFunc = splash;
}

void loop()
{
  gameFunc();
  DisplaySlate();  // write the updated game buffer to the screen
}
