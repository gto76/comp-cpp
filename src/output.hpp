#ifndef OUTPUT_H
#define OUTPUT_H

//#include "const.hpp"

#include <signal.h>

using namespace std;

typedef void (*callback_function)(void);

// Initializes the output
void setOutput(callback_function drawScreen, int width, int height);

// API
void printCharXY(char c, int x, int y);
void printString(const char s[], int x, int y);
void redrawScreen();
void printCharImediately(char c, int x, int y);

void clearScreen(void);

extern volatile sig_atomic_t screenResized;
 
#endif
