#pragma once
#include <subparser/subparser.h>

#include "util.h"

inline bool commit(const subparser::Command& command) {
	auto message = get_value_opt(command.value_options, "-m");
    if (message.has_value()) {
        
    }
    
	return true;
}
