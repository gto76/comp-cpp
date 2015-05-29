#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>

void setOutput(void);
void printCharXY(char c, int x, int y);
void printString(char const s[], int x, int y);
int getAbsoluteX(int x);
int getAbsoluteY(int y);
int getAbsoluteCoordinate(int value, int console, int track);
int coordinatesOutOfBounds(int x, int y);
void clearScreen(void);
typedef void (*callback_function)(void); 
void redrawScreen(callback_function draw);
void registerSigWinChCatcher(void);
void sigWinChCatcher(int signum);
void updateConsoleSize(void);
void copyArray(char dest[], const char src[], int width);

/////////////////////////////

#define PRINT_IN_CENTER 1
#define TRACK_WIDTH 80
#define TRACK_HEIGHT 24

int columns = TRACK_WIDTH;
int rows = TRACK_HEIGHT;

///////// PUBLIC ////////////

void setOutput() {
	registerSigWinChCatcher();
	updateConsoleSize();
}

/////////////////////////

void printCharXY(char c, int x, int y) {
	if (coordinatesOutOfBounds(x, y))
		return;
	printf("\033[%d;%dH%c", getAbsoluteY(y), getAbsoluteX(x), c);  	
}

void printString(char const s[], int x, int y) {
	if (coordinatesOutOfBounds(x, y))
		return;
	int itDoesntFitTheScreen = strlen(s) + x > columns;
	if (itDoesntFitTheScreen) {
		int distanceToTheRightEdge = columns - x - 1;
		char subArray[distanceToTheRightEdge+2];
		copyArray(subArray, s, distanceToTheRightEdge+2);
		s = subArray;
		printf("\033[%d;%dH%s", getAbsoluteY(y), getAbsoluteX(x), subArray);
	} else {
		printf("\033[%d;%dH%s", getAbsoluteY(y), getAbsoluteX(x), s);
	}
}

int getAbsoluteX(int x) {
	return getAbsoluteCoordinate(x, columns, TRACK_WIDTH);
}

int getAbsoluteY(int y) {
	return getAbsoluteCoordinate(y, rows, TRACK_HEIGHT);
}

int getAbsoluteCoordinate(int value, int console, int track) {
	int offset = 0;
	if (PRINT_IN_CENTER) {
		offset = (console - track) / 2;
		if (offset < 0)
			offset = 0;
	}
	return value+1 + offset;
}

int coordinatesOutOfBounds(int x, int y) {
	return x >= columns || y >= rows || x < 0 || y < 0;
}


////////// DRAW ///////////

void clearScreen(void) {
	printf("\e[1;1H\e[2J");
}

void redrawScreen(callback_function draw) {
	updateConsoleSize();
	clearScreen();
	// draw stuff here
	draw();
	fflush(stdout);
}

/////////// SIGNALS /////////////

void registerSigWinChCatcher() {
	struct sigaction action;

	sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

	action.sa_handler = sigWinChCatcher;
	sigaction(SIGWINCH, &action, NULL);
}

void sigWinChCatcher(int signum) {
	//redrawScreen();
}

void updateConsoleSize() {
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	columns = w.ws_col;
	rows = w.ws_row;
}

///////// UTIL //////////
void copyArray(char dest[], const char src[], int width) {
    int i;
    for (i = 0; i < width-1; i++) {
        dest[i] = src[i];
    }   
    dest[width-1] = '\0';
}


