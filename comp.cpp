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

string DRAWING_FILE = "/drawing";
std::ifstream drawingStream(DRAWING_FILE);
std::string drawingString((std::istreambuf_iterator<char>(drawingStream)),
                 std::istreambuf_iterator<char>());

Printer printer;
Ram ram;
Cpu cpu;


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



		void Cpu::exec() {      
			if (Util::getInt(pc) >= RAM_SIZE) {
				exit(0);
			}
			vector<bool> tmp = ram.get(pc);
			vector<bool> instruction = Util::getFirstNibble(tmp);
			vector<bool> adr = Util::getSecondNibble(tmp);
			if (debug) {
				// TODO: check for efficiency
				string out = Renderer::renderState(printer, ram, cpu); // *this only makes a shallow copy!!!!
				//renderState(instruction, adr, output);
				for (int i = 0; i < ROWS; i++) {
					cout << "\n";
				}
				cout << out;
				if (automatic && cycle != 0) {
					usleep(fq*1000);
				} else {
					if (cycle == 0) {
						cout << "Press Enter to begin execution";
					}
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



int main(int argc, const char* argv[]) {
	ram.set({false, false, false, false}, {true, true, true, true, true, true, true, true});
	vector<bool> word = ram.get({false, false, false, false});
	cout << "Value of first bit of first word is " << word[0] << "\n";
}


