#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <streambuf>

#include "const.hpp"
#include "util.hpp"

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


string output;
//var ramValues: Array[Array[Boolean]] = null

class Ram {
	public: 
		vector<bool> get(vector<bool> adr) {
			int address = Util::getInt(adr);
			vector<bool> wordOut(WORD_SIZE);
			for (int i = 0; i < WORD_SIZE; i++) {
				wordOut[i] = state[address][i];
			}
			return wordOut;
		}

		void set(vector<bool> adr, vector<bool> wordIn) {
			int address = Util::getInt(adr);
			if (address < RAM_SIZE) {
				for (int i = 0; i < WORD_SIZE; i++) {
					state[address][i] = wordIn[i];
				}
			} else {
				char formatedInt [10];
				sprintf(formatedInt, "%3d", Util::getInt(wordIn));
				string outputLine = Util::getString(wordIn) + " " + formatedInt + "\n";
		        output = output + outputLine;
		        if (!debug) {
		          cout << outputLine;
		        }			
			}
		}
		
	private:
		vector<vector<bool>> state = vector<vector<bool>>(RAM_SIZE, vector<bool>(WORD_SIZE)); 
} ram;

class Cpu {
	public:
		void exec() {      
			if (Util::getInt(pc) >= RAM_SIZE) {
				exit(0);
			}
			vector<bool> tmp = ram.get(pc);
			vector<bool> instruction = Util::getFirstNibble(tmp);
			vector<bool> adr = Util::getSecondNibble(tmp);
			if (debug) {
				string out = "testing..."; //renderState(instruction, adr, output);
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

	private:
		vector<bool> reg = vector<bool>(WORD_SIZE);
		vector<bool> pc = vector<bool>(ADDR_SIZE);
		int cycle = 0;
		//string printerOutput = "";

		void increasePc() {
			pc = Util::getBoolNibb(Util::getInt(pc) + 1);
		}

		void read(vector<bool> adr) {
			reg = ram.get(adr);
			increasePc();
		}
		void write(vector<bool> adr) {
			ram.set(adr, reg);
			increasePc();
		}
		void add(vector<bool> adr) {
			reg = Util::getBoolByte(Util::getInt(reg) + Util::getInt(adr));
			increasePc();			
		}
		void sub(vector<bool> adr) {
			reg = Util::getBoolByte(Util::getInt(reg) - Util::getInt(adr));
			increasePc();
		}
		void jump(vector<bool> adr) {
			pc = adr;
		}
		void readPointer(vector<bool> adr) {
			reg = ram.get(Util::getSecondNibble(reg));
			increasePc();
		}
		void jumpIf(vector<bool> adr) {
			if (Util::getInt(reg) >= 127) {
				pc = adr;
			} else {
				increasePc();
			}
		}
		void jumpIfSmaller(vector<bool> adr) {
			if (Util::getInt(reg) >= 127) {
				pc = adr;
			} else {
				increasePc();
			}
		}
} cpu;


int main(int argc, const char* argv[]) {
	ram.set({false, false, false, false}, {true, true, true, true, true, true, true, true});
	vector<bool> word = ram.get({false, false, false, false});
	cout << "Value of first bit of first word is " << word[0] << "\n";
}


