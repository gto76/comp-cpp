#include <algorithm>
#include <vector>
#include <string>
// Ex: #include <regex>
#include <fstream>

#include "const.hpp"
#include "renderer.hpp"
#include "printer.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "util.hpp"
#include "drawing.hpp"

using namespace std;

// STATIC PUBLIC:
string Renderer::renderState(Printer printerIn, Ram ramIn, Cpu cpuIn) {
    Renderer instance(printerIn, ramIn, cpuIn);

    string out;
    for (string line : Util::splitString(drawing)) { 
        string processedLine = instance.insertActualValues(line);
        out += processedLine + "\n";
    }
    out.erase(out.end() - 1);
    return out;
}

// PRIVATE CONSTRUCTOR:
Renderer::Renderer(Printer printerIn, Ram ramIn, Cpu cpuIn) {
    Renderer::printer = printerIn;
    Renderer::ram = ramIn;
    Renderer::cpu = cpuIn;
    switchIndex.clear();
}

// DYNAMIC PRIVATE:
string Renderer::insertActualValues(string lineIn) {
	// Ex: regex alpNum("[0-9a-z]");
	string lineOut;

	for (char cIn : lineIn) {
		char cOut;

		bool charIsALightbulb = (cIn >= 'a' && cIn <= 'z') || (cIn >= '0' && cIn <= '9');
        if (charIsALightbulb) {
            cOut = getLightbulb(cIn);
        } else {
            cOut = cIn;
        }
		lineOut += cOut;
	}
	return lineOut;
}

char Renderer::getLightbulb(char cIn) {
    int i = switchIndex[cIn]++;

    // Ex: regex patRam("[0-9a-e]");

	bool charRepresentsRam = (cIn >= 'a' && cIn <= 'e') || (cIn >= '0' && cIn <= '9');
    if (charRepresentsRam) {
        int j = Util::hexToInt(cIn);
        return getRamAt(j, i);
    }

    switch (cIn) {
        case 'p': 
            return Util::getChar(pcIsPointingToAddress(i));
        case 's':
            return Util::getChar(instructionIsPointingToAddress(i));
        case 'r':
            return  Util::getChar(cpu.getRegister().at(i)); 
        case 'i':
            return Util::getChar(instructionHasId(i));
        case 'o':
            return getFormattedOutput(i);
    }
	//There was an error parsing a drawing file. Problem with char cIn
    return ' ';
}

bool Renderer::pcIsPointingToAddress(int adr) {
    return Util::getInt(cpu.getPc()) == adr;
}

bool Renderer::instructionIsPointingToAddress(int adr) {
    // If execution did't yet start
    if (cpu.getCycle() == 0) {
        return false;
    }
    // If pc is pointing to the last addres (execution reached the end)
    if (Util::getInt(cpu.getPc()) == RAM_SIZE) {
        return false;
    }
    return Util::getInt(cpu.getAddress()) == adr;
}

bool Renderer::instructionHasId(int id) {
    // If execution did't yet start
    if (cpu.getCycle() == 0) {
        return false;
    }
    // If pc is pointing to the last addres (execution reached the end)
    if (Util::getInt(cpu.getPc()) == RAM_SIZE) {
        return false;
    } 
    return Util::getInt(cpu.getInstruction()) == id; 
}

char Renderer::getFormattedOutput(int i) {
    if (printer.getPrinterOutput().length() <= (unsigned) i) {
        return ' ';
    } else {
        return printer.getPrinterOutput().at(i);
    }
}

char Renderer::getRamAt(int j, int i) {
    return Util::getChar(ram.state.at(j).at(i));
}

