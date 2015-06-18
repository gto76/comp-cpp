#ifndef PRINTER_H
#define PRINTER_H

#include <string>

using namespace std;

class Printer {
	public:
		void print(string sIn);
		string getOutput();
		string getPrinterOutput();
		void clear();
	private:
		string output = "";
		string printerOutput;
		bool printerOutputUpdated = false;
		string renderPrinterOutput();
};

#endif