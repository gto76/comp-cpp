#ifndef RENDERER_H
#define RENDERER_H

using namespace std;

class Renderer {
	public:
		static Printer printer;
		static Ram ram;
		static Cpu cpu;
		static std::string renderState(Printer pritner, Ram ram, Cpu cpu);

	private:
		static std::string insertActualValues(string lineIn);
		static char getLightbulb(char cIn);
		static bool pcIsPointingToAddress(int adr);
		static bool instructionIsPointingToAddress(int adr);
		static bool instructionHasId(int id);
		static char getFormattedOutput(i: Int);
		static char getRam(char cIn, int i);
};

#endif