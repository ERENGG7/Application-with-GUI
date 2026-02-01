//Backend for login app
//it's not real backend
#ifndef BACKEND_H
#define BACKEND_H

#include <sodium.h>
#include "sodium_init.h"

using json = nlohmann::json;

class Backend {
private:
	json database;
public:
	Backend() {
		SodiumInit sodium;
	}
	//TODO: hash password:
	std::string hash_password(const char* password);
	bool verifyPassword(std::string hashed_password, const char* entered_password);
	//TODO: Create account
	bool Register(const char* name, const char* password);
	//TODO: Login
	bool log_in(const char* name, const char* password);
};
#endif