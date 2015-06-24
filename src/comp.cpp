#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <tuple>
#include <cmath>

#include "const.hpp"
#include "util.hpp"

#include "printer.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "renderer.hpp"
#include "drawing.hpp"

using namespace std;

extern "C" {
	typedef void (*callback_function)(void);
	void setEnvironment();
	void setOutput(callback_function drawScreen, int width, int height);
	void printCharXY(char c, int x, int y);
	void printString(const char s[], int x, int y);
	void redrawScreen();
	void resetEnvironment();
	extern int screenResized;
	extern int pleaseExit;
}

Printer printer;
Ram ram;
Cpu cpu;

// Selected bit with the cursor
int cursorX = 0;
int cursorY = 0;

// Graphic representation of the computer state
vector<string> buffer;

// Saved state of a ram
vector<vector<bool>> savedRam;

// Offset of first ram lightbulb in the asci drawing
int ramX;
int ramY;

int executionCounter = 0;
bool executionCanceled = false;

void setRamOffset() {
	tuple<int,int> t = Util::getLocationOfFirstRamLightbulb() ;
	ramX = get<0>(t);
	ramY = get<1>(t);
}

void drawScreen() {
	string out = Renderer::renderState(printer, ram, cpu);
	buffer = Util::splitString(out);
	int i = 0;
	for (string line : buffer) {
		printString(line.c_str(), 0, i++);
	}
}

void highlightCursor(bool highlight) {
	char c;
	try {
		c = buffer.at(cursorY+ramY).at(cursorX+ramX);
	} catch (int e) {
		cout << "Cursor out of bounds. Exception Nr. " << e << '\n';
		return;
	}
	if (highlight) {
		printf("\e[%dm\e[%dm", 30, 47);
	}
	printCharXY(c, cursorX+ramX, cursorY+ramY);
	if (highlight) {
		printf("\e[%dm\e[%dm", 37, 40);
	}
	fflush(stdout);
}

void switchBitUnderCursor() {
	bool newBitValue = !ram.state.at(cursorY).at(cursorX);
	ram.state.at(cursorY).at(cursorX) = newBitValue;
	buffer.at(cursorY+ramY).at(cursorX+ramX) = Util::getChar(newBitValue);
}

char readStdin(bool drawCursor) {
	char c = 0;
	errno = 0;
	ssize_t num = read(0, &c, 1);
	if (num == -1 && errno == EINTR) {
		// Exit if ctrl-c was pressed
		if (pleaseExit) {
			exit(0);
		}
		redrawScreen();
		if (drawCursor) {
			highlightCursor(true);
		}
		return readStdin(drawCursor);
	}
	return c;
}

void run() { 
	if (executionCounter > 0) {
		printer.print("            \n");
	}
	savedRam = ram.state;
	cpu.exec();
	// if 'esc' was pressed then it doesn't wait for keypress at the end
	if (executionCanceled) {
		executionCanceled = false;
	} else {
		readStdin(false);
	}
	ram = Ram();
	ram.state = savedRam;
	cpu = Cpu();
	redrawScreen();
	executionCounter++;
}

void userInput() {
	while(1) {
		char c = readStdin(true);

		highlightCursor(false);
		switch (c) {
			case 65: // up
			case 107: // k
				if (cursorY > 0) {
					cursorY--;
				}
				break;
			case 66: // down
			case 106: // j
				if (cursorY < RAM_SIZE-1) {
					cursorY++;
				}
				break;
			case 67: // right
			case 108: // l
				if (cursorX < WORD_SIZE-1) {
					cursorX++;
				}
				break;
			case 68: // left
			case 104: // h
				if (cursorX > 0) {
					cursorX--;
				}
				break;
			case 32: // space
				switchBitUnderCursor();
				break;
			case 10: // enter
				run();
				break;
		}
		highlightCursor(true);
	}
}

// Run every cycle
void sleepAndCheckForKey() {
	usleep(FQ*1000);

	// Exit if ctrl-c was pressed
	if (pleaseExit) {
		exit(0);
	}
	
	// Pauses execution if a key was hit, and waits for another key hit
	int keyCode = Util::getKey();
	if (keyCode) {
		// If escape was pressed
		if (keyCode == 27) {
			executionCanceled = true;
			return;
		}
		// Press key to continue
		keyCode = readStdin(false);
		// If the key was esc
		if (keyCode == 27) {
			executionCanceled = true;
		}
	}
}

void prepareOutput() {
	size_t drawingWidth = 0;
	size_t drawingHeight = 0;
	for (string line : Util::splitString(drawing)) {
		drawingWidth = std::max(drawingWidth, line.length());
		drawingHeight++;
	}
	setOutput(&drawScreen, drawingWidth, drawingHeight);
}

/*
 * PRINTER
 */

void Printer::print(string sIn) {
	output += sIn;
	printerOutputUpdated = false;
}

string Printer::getOutput() {
	return output;
}

string Printer::getPrinterOutput() {
	if (!printerOutputUpdated) {
		printerOutput = renderPrinterOutput();
		printerOutputUpdated = true;
	}
	return printerOutput;
}

void Printer::clear() {
	output = "";
	printerOutputUpdated = false;
}

string Printer::renderPrinterOutput() {
	if (output.length() <= 0) {
		return "|0|______________|0|";
	}
	vector<string> lines = Util::splitString(output);
	reverse(lines.begin(), lines.end());
	vector<string>  outputLines;
	for (string line : lines) {
		outputLines.push_back("|0| " + line + " |0|");
	}
	outputLines.push_back("|0|______________|0|");
	return Util::makeString(outputLines);
}

/*
 * RAM
 */

vector<bool> Ram::get(vector<bool> adr) {
	int address = Util::getInt(adr);
	// return random if last address (reserved for output)
	if (address == RAM_SIZE) {
		vector<bool> wordOut = Util::getRandomWord();
		return wordOut;
	}
	vector<bool> wordOut(WORD_SIZE);
	for (int i = 0; i < WORD_SIZE; i++) {
		wordOut[i] = state[address][i];
	}
	return wordOut;
}

void Ram::set(vector<bool> adr, vector<bool> wordIn) {
	int address = Util::getInt(adr);
	// Save word
	if (address < RAM_SIZE) {
		for (int i = 0; i < WORD_SIZE; i++) {
			state[address][i] = wordIn[i];
		}
	// Send word to printer
	} else {
		char formatedInt [4];
		sprintf(formatedInt, "%3d", Util::getInt(wordIn));
		string outputLine = Util::getString(wordIn) + " " + formatedInt + "\n";
		printer.print(outputLine);
	}
}

/*
 * CPU
 */

void Cpu::exec() {
	while(!executionCanceled) {
		bool shouldContinue = step();
		redrawScreen(); // always redraw
		if(!shouldContinue) {
			return;
		}
		sleepAndCheckForKey();
	}
}

bool Cpu::step() {
	cycle++;

	// Stop if reached last address
	if (Util::getInt(pc) >= RAM_SIZE) {
		return false;
	}

	vector<bool> instruction = getInstruction();
	int instCode = Util::getInt(instruction);
	vector<bool> adr = getAddress();

	switch (instCode) {
		case 0:
			read(adr);
			break;
		case 1:
			write(adr);
			break;
		case 2:
			add(adr);
			break;
		case 3:
			sub(adr);
			break;
		case 4:
			jump(adr);
			break;
		case 5:
			jumpIfMax(adr);
			break;
		case 6:
			jumpIfMin(adr);
			break;
		case 7:
			shiftRight();
			break;
		default:
			read(adr);
	}

	return true;
}

int Cpu::getCycle() {
	return cycle;
}

vector<bool> Cpu::getRegister() {
	return reg;
}

vector<bool> Cpu::getPc() {
	return pc;
}

vector<bool> Cpu::getInstruction() {
	vector<bool> instruction = Util::getFirstNibble(ram.get(pc));
	// if instruction id is larger than the number of instructions than the instruction with id 1 (write) gets executed.
	if (Util::getInt(instruction) >= NUM_OF_INSTRUCTIONS) {
		return Util::getBoolNibb(0);
	}
	return instruction;
}

vector<bool> Cpu::getAddress() {
	return Util::getSecondNibble(ram.get(pc));
}

void Cpu::increasePc() {
	pc = Util::getBoolNibb(Util::getInt(pc) + 1);
}

void Cpu::read(vector<bool> adr) {
	reg = ram.get(adr);
	increasePc();
}

void Cpu::write(vector<bool> adr) {
	ram.set(adr, reg);
	increasePc();
}

void Cpu::add(vector<bool> adr) {
	reg = Util::getBoolByte(Util::getInt(reg) + Util::getInt(ram.get(adr)));
	increasePc();
}

void Cpu::sub(vector<bool> adr) {
	reg = Util::getBoolByte(Util::getInt(reg) - Util::getInt(ram.get(adr)));
	increasePc();
}

void Cpu::jump(vector<bool> adr) {
	pc = adr;
}

void Cpu::jumpIfMax(vector<bool> adr) {
	if (Util::getInt(reg) >= pow(2, WORD_SIZE)-1) {
		pc = adr;
	} else {
		increasePc();
	}
}

void Cpu::jumpIfMin(vector<bool> adr) {
	if (Util::getInt(reg) <= 0) {
		pc = adr;
	} else {
		increasePc();
	}
}

void Cpu::shiftRight() {
	reg = Util::getBoolByte(Util::getInt(reg) / 2);
	increasePc();
}

/*
 * MAIN
 */

int main(int argc, const char* argv[]) {
	srand(time(NULL));
	setRamOffset();
	setEnvironment();
	prepareOutput();
	redrawScreen();
	highlightCursor(true);
	userInput();
}


