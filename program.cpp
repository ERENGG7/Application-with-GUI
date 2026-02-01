//frontend logic:
//UI ligic of log in program
//ImGui.

#include "program.h"
#include "User.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "enable_flags.h"
#include <cstring>

#if ENABLE_LOGIN_UI
//drawing UI for login box:
void Program::registerBox(const bool& darkTheme, int number) {
	
	ImGui::PushID(number);
	ImGui::BeginChild("log in", ImVec2(450, 100), false);
	if (darkTheme) {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	ImGui::SetWindowFontScale(1.2f);
	ImGui::PushItemWidth(220);
	ImGui::PushID(1);
	if (ImGui::InputText("name", userNameBuffer, sizeof(userNameBuffer),
		ImGuiInputTextFlags_EnterReturnsTrue)) {
		loginState = LoginState::EnterPassword;
	}
	ImGui::PopItemWidth();
	ImGui::PopID();
	if (loginState == LoginState::EnterPassword) {
		ImGui::PushID(2);
		ImGui::PushItemWidth(220);
		if (ImGui::InputText("password", passwordBuffer, sizeof(passwordBuffer),
			ImGuiInputTextFlags_EnterReturnsTrue |
			ImGuiInputTextFlags_Password)) {
			bool success = false;
			//log in
			if (number == 0) {
				success = program_backend.log_in(userNameBuffer, passwordBuffer);
				if (!success) { showMessage = true; }
			}
			//registration
			else if (number == 1) {
				success = program_backend.Register(userNameBuffer, passwordBuffer);
				if (!success) { showMessage = true; }
			}
			if (success) {
				sprintf_s(name, sizeof(name), "%s", userNameBuffer);
				loginState = LoginState::LoggedIn;
				logIn = false;
				registerIn = false;
				showMessage = false;
			}
			//cleaning buffers:
			memset(userNameBuffer, 0, sizeof(userNameBuffer));
			sodium_memzero(passwordBuffer, sizeof(passwordBuffer));
		}
		if (showMessage) {
			ImGui::Text("%s", number == 0 ? "Wrong name or password" : "User with this name already exists");

		}
		ImGui::PopID();
		ImGui::PopItemWidth();
	}
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::PopID();
}
#endif 
#ifdef ENABLE_LOGIN_UI
//drawing UI after log-in:
void Program::drawUI(const bool& darkTheme, const float& value) {

	ImGui::SetNextWindowSize(ImVec2(550, 350), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(value);
	ImGui::Begin("login", &isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse);
	if (loginState == LoginState::EnterUsername
		|| loginState == LoginState::EnterPassword) {
		if (ImGui::Button("Log in")) {
			logIn = true;
			registerIn = !logIn;
		}
		if (ImGui::Button("Make registration")) {
			registerIn = true;
			logIn = !registerIn;
		}
		if (logIn) {
			ImGui::Text("Log in");
			registerBox(darkTheme, 0);
		}
		if (registerIn) {
			ImGui::Text("Register in");
			registerBox(darkTheme, 1);
		}
	}
	else if (loginState == LoginState::LoggedIn) {
		ImGui::Text("Hello %s", name);
	}
	ImGui::End();
	//reset flags:
	if (!isRunning) {
		loginState = LoginState::EnterUsername;
		logIn = false;
		registerIn = false;
	}
}
#endif