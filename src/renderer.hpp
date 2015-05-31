#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <map>

#include "printer.hpp"
#include "ram.hpp"
#include "cpu.hpp"

using namespace std;

class Renderer {
	public:
		static string renderState(Printer pritnerIn, Ram ramIn, Cpu cpuIn);

	private:
		Renderer (Printer pritnerIn, Ram ramIn, Cpu cpuIn);
		Printer printer;
		Ram ram;
		Cpu cpu;
		map<char, int> switchIndex;
		string insertActualValues(string lineIn);
		char getLightbulb(char cIn);
		bool pcIsPointingToAddress(int adr);
		bool instructionIsPointingToAddress(int adr);
		bool instructionHasId(int id);
		char getFormattedOutput(int i);
		char getRamAt(int j, int i);
};

#endif