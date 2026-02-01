//first project Gui - calculator
//calculator basic.Operations and 
//ImGui UI ligic

#include <iostream>
#include <sstream>
#include <cstring>

#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include "enable_flags.h"
#include "calculator.h"

//#include "calculator.h"

#if ENABLE_CALCULATOR
//imgui:
bool Calculator::doublePoint() {
	int operatorIndex = 0.0;
	int pointersInFirstNumber = 0;
	int pointersInSecondNumber = 0; 
	int len = strlen(calculatorInput);
	//check for pointers:
	for (int i = 0; i < len; i++) {
		if (!isdigit(static_cast<unsigned char>(calculatorInput[i]))
			&& calculatorInput[i] != '.') {
			operatorIndex = i;
			break;
		}
	}
	//first number:
	for (int i = 0; i < operatorIndex; i++) {
		if (calculatorInput[i] == '.') {
			pointersInFirstNumber++;
		}
		if (pointersInFirstNumber >= 2) {
			return true;
		}
	}
	//secondNumber:
	for (int i = operatorIndex + 1; i < len; i++) {
		if (calculatorInput[i] == '.') {
			pointersInSecondNumber++;
		}
		if (pointersInSecondNumber >= 2) {
			return true;
		}
	}
	return false;
}
#endif
#if ENABLE_CALCULATOR
//validation:
bool Calculator::isValid() {
	const char* operations[] = { "+","-","*","/" };
	int counter = 0;
	int numberCounter = 0;
	int len = strlen(calculatorInput);

	if (len == 0) { return false; }
	if (!isdigit((unsigned char)calculatorInput[len - 1])) { return false; }
	if (!isdigit((unsigned char)calculatorInput[0])
		&& calculatorInput[0] != '-') {
		return false;
	}
	//check for ++ -- ..:
	for (int i = 0; i < len - 1; i++) {
		if (!isdigit((unsigned char)calculatorInput[i])
			&& !isdigit((unsigned char)calculatorInput[i + 1])) {
			return false;
		}
	}
	//check for only numbers:
	for (int i = 0; i < len - 1; i++) {
		if (isdigit((unsigned char)calculatorInput[i])) {
			numberCounter++;
		}
	}
	if (numberCounter == len) {
		return false;
	}
	//check for pointers:
	if (doublePoint()) {
		return false;
	}
	int startPoint;
	if (calculatorInput[0] == '-') { startPoint = 1; }
	else { startPoint = 0; }
	//check for more than one operation:
	for (int i = 0; i < 4; i++) {
		for (int j = startPoint; j < len; j++) {
			if (operations[i][0] == calculatorInput[j]) {
				counter++;
			}
			if (counter >= 2) { return false; }
		}
	}
	return true;
}
#endif
//calculate:
//1.separate numbers and operator:
//2. Reseting inputBuffer:
//3.sprintf into input buffer:
#if ENABLE_CALCULATOR
void Calculator::calculate() {
	int index = 0;
	char operation = 0;
	std::stringstream numberOne, numberTwo;
	long double a = 0, b = 0;

	for (int i = 0; i < strlen(calculatorInput); i++) {
		if (isdigit((unsigned char)calculatorInput[i]) || calculatorInput[i] == '.'
			|| (i == 0 && calculatorInput[i] == '-')) {
			numberOne << calculatorInput[i];
		}
		else {
			index = i;
			operation = calculatorInput[i];
			break;
		}
	}
	for (int i = index + 1; i < strlen(calculatorInput); i++) {
		if (isdigit((unsigned char)calculatorInput[i]) || calculatorInput[i] == '.') {
			numberTwo << calculatorInput[i];
		}
	}
	numberOne >> a;
	numberTwo >> b;
	long double result = 0.0;
	switch (operation) {

	case '+': result = a + b; break;
	case '-': result = a - b; break;
	case '*': result = a * b; break;
	case '/':
		if (b == 0) return;
		result = a / b;
		break;
	default: return;
	}
	//import result:
	char outputResult[50];
	sprintf_s(outputResult, sizeof(outputResult), "%.2Lf", result);
	if (strlen(outputResult) < 13) {
		sprintf_s(calculatorInput, sizeof(calculatorInput), "%s", outputResult);
	}
}
#endif
//Draw calculator:
#if ENABLE_CALCULATOR
void Calculator::DrawButtons(const bool& darkTheme) {
	const char* buttons[] = {
		"1","2","3",
		"4","5","6",
		"7","8","9",
		"0","+","-",
		"*","/","=",
		".","AC","Close"
	};
	ImGui::SetWindowFontScale(1.3f);
	if (!darkTheme) {
		//dark theme colors for buttons and text:
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	}
	for (int i = 0; i < 14; i++) {
		if (ImGui::Button(buttons[i], ImVec2(75, 30))
			&& strlen(calculatorInput) < 13) {
			strcat_s(calculatorInput, sizeof(calculatorInput), buttons[i]);
		}
		//x3 buttons per line:
		if ((i + 1) % 3 != 0) { ImGui::SameLine(); }
	}
	for (int i = 14; i < 18; i++) {
		switch (i) {
		case 14:
			if (ImGui::Button(buttons[i], ImVec2(75, 30))) {
				if (isValid()) { calculate(); }
			}
			break;
		case 15:
			if (ImGui::Button(buttons[i], ImVec2(75, 30))
				&& strlen(calculatorInput) < 13
				&& strlen(calculatorInput) > 0) {
				strcat_s(calculatorInput, sizeof(calculatorInput), ".");
			}
			break;
		case 16:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
			if (ImGui::Button(buttons[i], ImVec2(75, 30))) {
				memset(calculatorInput, 0, sizeof(calculatorInput));
			}
			ImGui::PopStyleColor();
			break;
		case 17:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			if (ImGui::Button(buttons[i], ImVec2(75, 30))) {
				memset(calculatorInput, 0, sizeof(calculatorInput));
				showCalculator = false;
			}
			ImGui::PopStyleColor();
			break;
		}
		//x3 buttons per line:
		if ((i + 1) % 3 != 0) { ImGui::SameLine(); }
	}
	if (!darkTheme) {
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}
	ImGui::SetWindowFontScale(1.0f);
}
#endif
//draw calculator:UI Logic:
//182X227
#if ENABLE_CALCULATOR
void Calculator::drawUI(const bool& darkTheme, const float& value) {
	ImGui::SetNextWindowSize(ImVec2(264, 334), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(value);
	ImGui::Begin("calculator elka", nullptr,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::SetCursorPos(ImVec2(10, 75));

	ImVec4 color = darkTheme
		? ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
		: ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

	ImGui::PushStyleColor(ImGuiCol_Text, color);
	//creating textbox:
	ImGui::PushItemWidth(246);
	ImGui::SetWindowFontScale(1.6f);
	ImGui::PushID("Calculator");
	ImGui::InputText("",
		calculatorInput,
		sizeof(calculatorInput),
		ImGuiInputTextFlags_ReadOnly);
	ImGui::PopID();
	ImGui::SetWindowFontScale(1.0f);
	ImGui::PopStyleColor();
	ImGui::PopItemWidth();
	//buttons:
	DrawButtons(darkTheme);
	ImGui::End();
}
#endif