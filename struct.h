#ifndef STRUCT_H
#define STRUCT_H
#include <functional>
//window struct:
struct Button {

	const char* lable;
	bool isRunning;
	std::function<void(Button&)>func;
	Button(const char* lable, std::function<void(Button&)>func) :
		lable(lable), isRunning(false), func(func) {
	}
};
#endif // !STRUCT_H
