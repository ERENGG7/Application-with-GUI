
#include <fstream>
#include "json.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include "backend.h"

std::string Backend::hash_password(const char* password) {
		char hashed_password[crypto_pwhash_STRBYTES];
		if (crypto_pwhash_str(hashed_password,
			password,
			strlen(password),
			crypto_pwhash_OPSLIMIT_INTERACTIVE,
			crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			throw std::runtime_error("hash failed!");
		}
		return std::string(hashed_password);
	}
	bool Backend::verifyPassword(std::string hashed_password, const char* entered_password) {
		return crypto_pwhash_str_verify(hashed_password.c_str(),
			entered_password,
			strlen(entered_password) == 0);
	}
	//TODO: Create account
	bool Backend::Register(const char* name, const char* password) {

		std::ifstream inFile("users_data.json");
		if (inFile.is_open()) {
			inFile >> database;
			inFile.close();
		}
		else {
			database = json::object();
		}
		if (!database.contains("users") || !database["users"].is_array()) {
			database["users"] = json::array();
		}

		for (auto& user : database["users"]) {
			if (user["username"] == name) {
				return false;
			}
		}
		database["users"].push_back({
			{"username",name},
			{"password",hash_password(password)}
			});

		std::ofstream outFile("users_data.json");
		if (!outFile) {
			throw std::runtime_error("Cannot open users.json for writing!\n");
			return false;
		}
		outFile << database.dump(4);
		outFile.close();
		return true;
	}
	//TODO: Login
	bool Backend::log_in(const char* name, const char* password) {
		std::string hashed;
		std::ifstream inFile("users_data.json");
		if (inFile.is_open()) {
			inFile >> database;
			inFile.close();
		}
		else {
			database = json::object();
		}
		if (!database.contains("users") || !database["users"].is_array()) {
			database["users"] = json::array();
		}

		for (const auto& user : database["users"]) {
			if (user["username"] == name) {
				hashed = user["password"];
			}
		}
		if (hashed.empty()) {
			return false;   //user not found
		}
		return verifyPassword(hashed, password); //password 
	}