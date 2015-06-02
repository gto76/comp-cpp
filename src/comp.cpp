#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>
#include <tuple>

#include "const.hpp"
#include "util.hpp"

#include "printer.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "renderer.hpp"

using namespace std;

extern "C" {
	void setEnvironment();
	void setOutput();
	void printCharXY(char c, int x, int y);
	void printString(const char s[], int x, int y);
	typedef void (*callback_function)(void); 
	void redrawScreen(callback_function draw);
	void resetEnvironment();
}

// const int WORD_SIZE = 8;
// const int ADDR_SIZE = 4;
// const int RAM_SIZE = ADDR_SIZE*ADDR_SIZE-1;

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
	} else {
		printf("\e[%dm\e[%dm", 37, 40);
	}
	printCharXY(c, cursorX+ramX, cursorY+ramY);
}

void switchBitUnderCursor() {
	bool newBitValue = !ram.state.at(cursorY).at(cursorX);
	ram.state.at(cursorY).at(cursorX) = newBitValue;
	buffer.at(cursorY+ramY).at(cursorX+ramX) = Util::getChar(newBitValue);
}

void run() {
	savedRam = ram.state;
	cpu.exec();
	// if 'esc' was pressed then it doesn't wait for keypress at the end
	if (executionCanceled) {
		executionCanceled = false;
	} else {
		getc(stdin);
	}
	ram = Ram();
	ram.state = savedRam;
	cpu = Cpu();
	redrawScreen(&drawScreen);
}

void userInput() {
	while(1) {
		char c = getc(stdin);
		highlightCursor(false);
		switch (c) {
			case 65: // up
				if (cursorY > 0) {
					cursorY--;
				}
				break;
			case 66: // down
				if (cursorY < RAM_SIZE-1) {
					cursorY++;
				}
				break;
			case 67: // right
				if (cursorX < WORD_SIZE-1) {
					cursorX++;
				}
				break;
			case 68: // left
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
	// Pauses execution if a key was hit, and waits for another key hit
	int keyCode = Util::getKey();
	if (keyCode) {
		// If escape was pressed
		if (keyCode == 27) {
			executionCanceled = true;
			return;
		}
		// Press key to continue
		keyCode = getc(stdin);
		// If the key was esc
		if (keyCode == 27) {
			executionCanceled = true;
		}
	}
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
		char formatedInt [3];
		sprintf(formatedInt, "%3d", Util::getInt(wordIn));
		string outputLine = Util::getString(wordIn) + " " + formatedInt + "\n";
        printer.print(outputLine);			
	}
}


/*
 * CPU
 */

void Cpu::exec() {
	sleepAndCheckForKey();
	if (executionCanceled) {
		return;
	}

	cycle++;     
	redrawScreen(&drawScreen);

	// Stop if reached last address
	if (Util::getInt(pc) >= RAM_SIZE) {
		return;
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
			readPointer(adr);
			break;
		case 6:
			jumpIf(adr);
			break;
		case 7:
			jumpIfSmaller(adr);
			break;
		default:
			read(adr);
	}
	exec();
}

// TODO check if sends a copy!!!

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
	//reg = Util::getBoolByte(Util::getInt(reg) + Util::getInt(adr)); // WRONG!!
	reg = Util::getBoolByte(Util::getInt(reg) + Util::getInt(ram.get(adr)));
	increasePc();			
}

void Cpu::sub(vector<bool> adr) {
	//reg = Util::getBoolByte(Util::getInt(reg) - Util::getInt(adr)); // WRONG!!
	reg = Util::getBoolByte(Util::getInt(reg) - Util::getInt(ram.get(adr)));
	increasePc();
}

void Cpu::jump(vector<bool> adr) {
	pc = adr;
}

void Cpu::readPointer(vector<bool> adr) {
	reg = ram.get(Util::getSecondNibble(reg));
	increasePc();
}

void Cpu::jumpIf(vector<bool> adr) {
	if (Util::getInt(reg) >= 127) {
		pc = adr;
	} else {
		increasePc();
	}
}

void Cpu::jumpIfSmaller(vector<bool> adr) {
	if (Util::getInt(reg) < 127) {
		pc = adr;
	} else {
		increasePc();
	}
}


/*
 * MAIN
 */

int main(int argc, const char* argv[]) {
	srand(time(NULL));
	setRamOffset();
	setEnvironment();
	setOutput();
	redrawScreen(&drawScreen);
	highlightCursor(true);
	userInput();
}


