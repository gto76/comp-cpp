#include <algorithm>

// add to hpp:
// map<char, int> switchIndex;
// vector<bool> pc -> maybe just pass pointer to ram and cpu!, or just pass stare, pc, and reg

string Renderer::renderState(Printer pritner, Ram ram, Cpu cpu) {
    Renderer::priter = printer;
    Renderer::ram = ram;
    Renderer::cpu = cpu;
    switchIndex.clear();

    string out;
    for (string line : Util::splitString(DRAWING)) {
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
		if (regex_match(cIn, alpNum)) {
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

    if (regex_match(cIn, patRam)) {
        return getRam(cIn, i);
    }

    switch (cIn) {
        case 'p': 
            return Util::getChar(pcIsPointingToAddress(i));
        case 's':
            return Util::getChar(instructionIsPointingToAddress(i));
        case 'r':
            return  Util::getChar(cpu.getReg()(i)); // todo reg
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

char Renderer::getFormattedOutput(i: Int) {
    if (printer.getPrinterOutput().length() <= i) {
        return ' ';
    } else {
        return printer.getPrinterOutput().at(i);
    }
}

char Renderer::getRam(char cIn, int i) {
    int j = Util::hexToInt(c);
    return Util::getChar(ram.state[j][i]);
}

