#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>

void setEnvironment(void);
void checkTerminal(void);
void saveAttributes(void);
void setMenuMode(void);
void setRaceMode(void);
void setNoncanonicalMode(int vmin, int vtime);
void registerSigIntCatcher(void);
void sigIntCatcher(int signum);
void disableRepeatAndCursor(void);
void resetEnvironment(void);
void resetInputMode(void);
void enableRepeatAndCursor(void);
void resetConsole(void);

//////////////////////////////

struct termios saved_attributes;

int const DISABLE_REPEAT = 0;

//////// AT START ////////////

void setEnvironment() {
	checkTerminal();
	saveAttributes();
	setMenuMode();
	registerSigIntCatcher();
	disableRepeatAndCursor();
}

void checkTerminal() {
	if (!~isatty(STDIN_FILENO)) {
		printf("Not a terminal: %d.\n", STDIN_FILENO);
		exit(EXIT_FAILURE);
	}
}

void saveAttributes() {
	tcgetattr(STDIN_FILENO, &saved_attributes);
}

// blocking mode (getc waits for input)
void setMenuMode() {
	setNoncanonicalMode(1, 0);
}

// nonblocking mode (getc does not wait for input, it returns every 0.1 s)
void setRaceMode() {
	setNoncanonicalMode(0, 1);
}

void setNoncanonicalMode(int vmin, int vtime) {
	struct termios tattr;
	// set noncanonical mode, disable echo
	atexit(resetEnvironment);
	tcgetattr(STDIN_FILENO, &tattr);
	tattr.c_lflag &= ~(ICANON|ECHO);
	tattr.c_cc[VMIN] = vmin;
	tattr.c_cc[VTIME] = vtime;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

void registerSigIntCatcher() {
	struct sigaction action;
	action.sa_handler = sigIntCatcher;
	sigaction(SIGINT, &action, NULL);
}

// method that gets executed when ctrl-c is pressed.
// necesary so that atexit method gets executed,
// that sets terminal back to the original state.
void sigIntCatcher(int signum) {
	exit(0);
}

void disableRepeatAndCursor() {
	if (DISABLE_REPEAT) {
		// disable repeat in xwindow console
		system("xset -r");
		// disable repeat in Linux console
		system("setterm --repeat off");
	}
	// set cursor off. could also probably use system("setterm -cursor off);
	printf("\e[?25l");
	fflush(stdout);
}

///////// AT END ////////////

void resetEnvironment() {
	resetInputMode();
	enableRepeatAndCursor();
	resetConsole();
}

void resetConsole() {
	system("reset");
}

void resetInputMode() {
	// bring back canonical mode
	tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void enableRepeatAndCursor() {
	if (DISABLE_REPEAT) {
		// enable repeat in Xwindow console
		system("xset r");
		// disable repeat in Linux console
		system("setterm --repeat on");
	}
	system("clear");
	// bring back cursor
	printf("\e[?25h");
	fflush(stdout) ;
}

