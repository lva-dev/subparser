#pragma once
#include <iostream>
#include <subparser/subparser.h>

#include "util.h"

inline bool commit(const subparser::SubCommand& command) {
	auto message = get_value_opt(command.value_opts, "-m");

    if (message.has_value()) {
        if (message->empty()) {
            std::cerr << "Aborting commit due to empty commit message.\n";
            return false;
        }
    } else {
        std::cerr << "error: no commit message\n";
        std::cerr << "Please supply the message using -m option.\n";
        return false;
    }

	std::cout << "[branch 0xd0b4762c]";

	if (message.has_value()) {
		std::cout << ' ' << message.value();
	}
    
	std::cout << '\n';

	return true;
}
