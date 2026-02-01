#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "program_two.h"
#include "enable_flags.h"

typedef bool Flag;
//flags:
class Calculator :public Program_Two {
private:
	char calculatorInput[14] = "";
public:
	Flag showCalculator = false;
#if ENABLE_CALCULATOR
	bool doublePoint();
	bool isValid();
	void calculate();
	void DrawButtons(const bool& darkTheme)override;
	//draw calculator:
	void drawUI(const bool& darkTheme, const float& value)override;
#endif
};
#endif
