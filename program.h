//Frontend logic
#ifndef PROGRAM_H
#define PROGRAM_H

#include "backend.h"
#include <functional>

 class Program {
private: 
	Backend program_backend;
	char passwordBuffer[13];
	char userNameBuffer[13];
	char name[13];

	bool logIn = false;
	bool registerIn = false;
	bool showMessage = false;
public:
	 bool isRunning = false;
	enum class LoginState {
		EnterUsername,
		EnterPassword,
		LoggedIn
	};
	LoginState loginState = LoginState::EnterUsername;
#ifdef ENABLE_LOGIN_UI
	void registerBox(const bool& darkTheme,int number);
	void drawUI(const bool& darkTheme, const float& value);
#endif
};
#endif