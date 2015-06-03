#ifndef OUTPUT_H
#define OUTPUT_H

typedef void (*callback_function)(void); 
void setOutput(callback_function drawScreen, int width, int height); 
void printCharXY(char c, int x, int y);
void printString(const char s[], int x, int y);
void redrawScreen();

#endif
