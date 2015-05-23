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
		static int test;
		static Printer printer;
		static Ram ram;
		static Cpu cpu;
		static map<char, int> switchIndex;
		static string insertActualValues(string lineIn);
		static char getLightbulb(char cIn);
		static bool pcIsPointingToAddress(int adr);
		static bool instructionIsPointingToAddress(int adr);
		static bool instructionHasId(int id);
		static char getFormattedOutput(int i);
		static char getRam(char cIn, int i);
};

#endif