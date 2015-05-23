#include <algorithm>
#include <vector>
#include <string>
#include <regex>

#include "renderer.hpp"
#include "printer.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "util.hpp"

using namespace std;

//string DRAWING = "testing!!!!!"; //TODO
// int Renderer::test = 0;
// Printer Renderer::printer;
// Ram Renderer::ram;
// Cpu Renderer::cpu;
// map<char, int> Renderer::switchIndex;


int test = 0;
Printer printer;
Ram ram;
Cpu cpu;
map<char, int> switchIndex;

string Renderer::renderState(Printer printerIn, Ram ramIn, Cpu cpuIn) {
    test = 0;
    printer = printerIn;
    ram = ramIn;
    cpu = cpuIn;
    switchIndex.clear();

    string out;
    for (string line : Util::splitString("fsdfsdf")) { // DRAWING)) {
        string processedLine = insertActualValues(line);
        out += processedLine + "\n";
    }
    out.erase(out.end() - 1);
    return out;
}

string Renderer::insertActualValues(string lineIn) {
	regex alpNum("[0-9a-z]");
	string lineOut;

	for (char cIn : lineIn) {
		char cOut;
		if (regex_match(Util::getString(cIn), alpNum)) {
			cOut = cIn;
		} else {
			cOut = getLightbulb(cIn);
		}
		lineOut += cOut;
	}
	return lineOut;
}

char Renderer::getLightbulb(char cIn) {
    //bool keyExists = switchIndex.count(cIn);
    int i = switchIndex[cIn]++;

    regex patRam("[0-9a-e]");

    if (regex_match(Util::getString(cIn), patRam)) {
        return getRam(cIn, i);
    }

    switch (cIn) {
        case 'p': 
            return Util::getChar(pcIsPointingToAddress(i));
        case 's':
            return Util::getChar(instructionIsPointingToAddress(i));
        case 'r':
            return  Util::getChar(cpu.getReg().at(i)); // todo reg
        case 'i':
            return Util::getChar(instructionHasId(i));
        case 'o':
            return getFormattedOutput(i);
    }
}

bool Renderer::pcIsPointingToAddress(int adr) {
    return Util::getInt(cpu.getPc()) == adr; // todo pc
}

bool Renderer::instructionIsPointingToAddress(int adr) {
    return Util::getInt(Util::getSecondNibble(ram.get(cpu.getPc()))) == adr; //todo ram pc
}

bool Renderer::instructionHasId(int id) {
    return Util::getInt(Util::getFirstNibble(ram.get(cpu.getPc()))) == id; // todo ram
}

char Renderer::getFormattedOutput(int i) {
    if (printer.getPrinterOutput().length() <= i) {
        return ' ';
    } else {
        return printer.getPrinterOutput().at(i);
    }
}

char Renderer::getRam(char cIn, int i) {
    int j = Util::hexToInt(cIn);
    return Util::getChar(ram.state.at(j).at(i));
}

