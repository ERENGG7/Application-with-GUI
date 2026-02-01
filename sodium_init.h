#ifndef SODIUM_INIT_H
#define SODIUM_INIT_H 

#include <sodium.h>
#include <stdexcept>

struct SodiumInit {
	SodiumInit() {
		if (sodium_init() < 0) {
			throw std::runtime_error("Sodium initialization failed");
		}
	}
	~SodiumInit() = default;
	SodiumInit(const SodiumInit&) = delete;
	SodiumInit& operator=(const SodiumInit&) = delete;
};
#endif
