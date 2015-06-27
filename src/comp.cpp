#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sstream>
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
	extern volatile sig_atomic_t screenResized;
	extern volatile sig_atomic_t pleaseExit;
}

bool interactivieMode;

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
	// Only change char of the buffer, as to avoid screen redraw.
	buffer.at(cursorY+ramY).at(cursorX+ramX) = Util::getChar(newBitValue);
}

void eraseByteUnderCursor() {
	ram.set(Util::getBoolNibb(cursorY), Util::getBoolByte(0));
	redrawScreen();
}

bool switchBytesInRam(int index1, int index2) {
	if (index1 < 0 || index2 < 0 || index1 >= RAM_SIZE || index2 >= RAM_SIZE) {
		return false;
	}
	vector<bool> addr1 = Util::getBoolNibb(index1);
	vector<bool> addr2 = Util::getBoolNibb(index2);
	vector<bool> temp = ram.get(addr1);
	ram.set(addr1, ram.get(addr2));
	ram.set(addr2, temp);
	return true;
}

void moveByteFor(int delta) {
	bool failed = !switchBytesInRam(cursorY, cursorY+delta);
	if (failed) {
		return;
	}
	cursorY += delta;
	redrawScreen();
}

void moveByteUnderCursorUp() {
	moveByteFor(-1);
}

void moveByteUnderCursorDown() {
	moveByteFor(1);
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

/*
 * Saves the state of the ram and starts the execution of a program.
 * When execution stops, due to it reaching last address or user pressing 'esc',
 * it loads back the saved state of the ram, and resets the cpu.
 */
void run() { 
	if (executionCounter > 0) {
		printer.printEmptyLine();
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

string getFreeFileName() {
	int i = 0;
	while (Util::fileExists(SAVE_FILE_NAME + to_string(++i)));
	return SAVE_FILE_NAME + to_string(i);
}

void saveRamToFile() {
	string fileName = getFreeFileName();
	ofstream fileStream(fileName);
    fileStream << ram.getString();
    fileStream.close();
}

void userInput() {
	while(1) {
		char c = readStdin(true);

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
			case 107: // k
				moveByteUnderCursorUp();
				break;
			case 106: // j
				moveByteUnderCursorDown();
				break;
			case 115: // s
				saveRamToFile();
				break;
			case 32: // space
			case 102: // f
				switchBitUnderCursor();
				break;
			case 100: // d
				eraseByteUnderCursor();
				break;
			case 10: // enter
				run();
				break;
		}
		highlightCursor(true);
	}
}

/*
 * Run every cycle.
 */
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

/*
 * Initializes 'output.c' by sending dimensions of a 'drawing' and a 'drawScreen'  
 * callback function, that output.c will use on every screen redraw.
 */
void prepareOutput() {
	size_t drawingWidth = 0;
	size_t drawingHeight = 0;
	for (string line : Util::splitString(drawing)) {
		drawingWidth = std::max(drawingWidth, line.length());
		drawingHeight++;
	}
	setOutput(&drawScreen, drawingWidth, drawingHeight);
}

bool getBool(char c) {
	return c == '*';
}

void loadRamFromFileStream(ifstream* fileStream) {
	int address = 0;
	while (!fileStream->eof()) {
		int bitIndex = 0;
		string line;
    	getline(*fileStream, line);
    	// Ignore line if empty or a comment.
    	if (line.empty() || line[0] == '#') {
    		continue;
    	}
		for (char c : line) {
			ram.state.at(address).at(bitIndex) = getBool(c);
			if (++bitIndex >= WORD_SIZE) {
				break;
			}
		}
		if (++address >= RAM_SIZE) {
			return;
		}
  	} 
}

void checkIfInputIsPiped() {
	interactivieMode = !Util::inputIsPiped();
}

vector<bool> readLineFromPipe() {
	string line;
	// Read until next whitespace
	cin >> line;
	// If reached end of pipe input
	if (!cin) {
		exit(0);
	}

	if (Util::startsWithDigit(line)) {
		int num = Util::extractInteger(line);
		return Util::getBoolByte(num);
	} else {
		return Util::getBoolByte(line);
	}
}

void loadRamIfFileSpecified(int argc, const char* argv[]) {
	if (argc <= 1) {
		return;
	}
   	ifstream fileStream;    
	fileStream.open(argv[1]);   
	if (fileStream.fail()) {
    	fprintf(stderr, "Invalid filename '%s'. Aborting ram load.", argv[1]);
   	} else {
   		loadRamFromFileStream(&fileStream);
      	fileStream.close();  
   	}
}

/*
 * PRINTER
 */

/*
 * If in interactive mode, then prints word together with decimal
 * representation on the printer. If not in interactive mode,
 * then prints to the stdout. If stdout is a pipe, then it doesen't
 * add the decimal representation.
 */
void Printer::print(vector<bool> wordIn) {
	if (interactivieMode) {
		output += Util::getStringWithFormatedInt(wordIn);
		printerOutputUpdated = false;
	} else {
		if (Util::outputIsPiped()) {
			cout << Util::getString(wordIn) + "\n";
		} else {
			cout << Util::getStringWithFormatedInt(wordIn);
		}
	}
}

void Printer::printEmptyLine() {
	output += "            \n";
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
	// Return random if last address (reserved for output),
	// or read from pipe if input is piped in.
	if (address == RAM_SIZE) {
		if (interactivieMode) {
			return Util::getRandomWord();
		} else {
			return readLineFromPipe();
		}
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
		printer.print(wordIn);
	}
}

string Ram::getString() {
	string out;
	for (vector<bool> word : state) {
		out += Util::getString(word) + '\n';
	}
	return out;
}

/*
 * CPU
 */

void Cpu::exec() {
	while(!executionCanceled) {
		bool shouldContinue = step();
		if (interactivieMode) {
			redrawScreen();
		}
		if(!shouldContinue) {
			return;
		}
		if (interactivieMode) {
			sleepAndCheckForKey();
		}
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
	// If instruction id is larger than the number of instructions then 
	// the instruction with id 1 (write) gets executed.
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

void startInteractiveMode() {
	setRamOffset();
	setEnvironment();
	prepareOutput();
	redrawScreen();
	highlightCursor(true);
	userInput();
}

int main(int argc, const char* argv[]) {
	srand(time(NULL));
	checkIfInputIsPiped();
	loadRamIfFileSpecified(argc, argv);
	if (interactivieMode) {
		startInteractiveMode();
	} else {
		cpu.exec();
	}
}


