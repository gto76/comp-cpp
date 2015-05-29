#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

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

bool debug = true;
int ROWS = 25;
bool automatic = true;
int fq = 300;

string TEST_FILE = "/fibbBin";
std::ifstream testStream(TEST_FILE);
std::string testString((std::istreambuf_iterator<char>(testStream)),
                 std::istreambuf_iterator<char>());


Printer printer;
Ram ram;
Cpu cpu;

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
	vector<string> outputLines = Util::splitString(output);
	reverse(outputLines.begin(), outputLines.end());
	for (string line : outputLines) {
		line =  "|0| " + line + " |0|";
	}
	outputLines.push_back("|0|______________|0|");
	return Util::makeString(outputLines);
}

/*
 * RAM
 */
vector<bool> Ram::get(vector<bool> adr) {
	int address = Util::getInt(adr);
	vector<bool> wordOut(WORD_SIZE);
	for (int i = 0; i < WORD_SIZE; i++) {
		wordOut[i] = state[address][i];
	}
	return wordOut;
}

void Ram::set(vector<bool> adr, vector<bool> wordIn) {
	int address = Util::getInt(adr);
	if (address < RAM_SIZE) {
		for (int i = 0; i < WORD_SIZE; i++) {
			state[address][i] = wordIn[i];
		}
	} else {
		char formatedInt [10];
		sprintf(formatedInt, "%3d", Util::getInt(wordIn));
		string outputLine = Util::getString(wordIn) + " " + formatedInt + "\n";
        printer.print(outputLine);
        if (!debug) {
          cout << outputLine;
        }			
	}
}

const bool CANONICAL = false;

const int RAM_X = 7;
const int RAM_Y = 5;

int cursorX = 0;
int cursorY = 0;

vector<string> buffer;

void highlightCursor(bool highlight) {
	char c;
	try {
		c = buffer.at(cursorY+RAM_Y).at(cursorX+RAM_X);
	} catch (int e) {
		cout << "Cursor out of bounds. Exception Nr. " << e << '\n';
		return;
	}
	if (highlight) {
		printf("\e[%dm\e[%dm", 30, 47);
	} else {
		printf("\e[%dm\e[%dm", 37, 40);
	}
	printCharXY(c, cursorX+RAM_X, cursorY+RAM_Y);
}

void switchBit() {
	ram.state.at(cursorY).at(cursorX) = !ram.state.at(cursorY).at(cursorX);
	string out = Renderer::renderState(printer, ram, cpu);
	buffer = Util::splitString(out);	
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
				switchBit();
				break;
			case 10: // enter
				cpu.exec();
				getchar();
				break;
		}
		highlightCursor(true);
	}
}


void drawScreen() {
	string out = Renderer::renderState(printer, ram, cpu);
	buffer = Util::splitString(out);
	int i = 0;
	for (string line : buffer) {
		printString(line.c_str(), 0, i++);
	}
}

/*
 * CPU
 */
void Cpu::exec() {      
	if (Util::getInt(pc) >= RAM_SIZE) {
		exit(0);
	}
	vector<bool> tmp = ram.get(pc);
	vector<bool> instruction = Util::getFirstNibble(tmp);
	vector<bool> adr = Util::getSecondNibble(tmp);
	if (debug) {
		redrawScreen(&drawScreen);
		if (automatic) {
			usleep(fq*1000);
		} else {
			getchar();
		}
	}
	int instCode = Util::getInt(instruction);

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
	}
	cycle++;
	exec();
}

// TODO check if safe!!!

vector<bool> Cpu::getReg() {
	return reg;
}

vector<bool> Cpu::getPc() {
	return pc;
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
	reg = Util::getBoolByte(Util::getInt(reg) + Util::getInt(adr));
	increasePc();			
}

void Cpu::sub(vector<bool> adr) {
	reg = Util::getBoolByte(Util::getInt(reg) - Util::getInt(adr));
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
	if (Util::getInt(reg) >= 127) {
		pc = adr;
	} else {
		increasePc();
	}
}


/*
 * MAIN
 */
int main(int argc, const char* argv[]) {
	setEnvironment();
	setOutput();
	ram.state = Util::getRamFromString(testString);
	drawScreen();
	userInput();
}


