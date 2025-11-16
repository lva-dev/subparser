#pragma once
#include <optional>
#include <subparser/subparser.h>

inline bool has_opt(const subparser::BooleanOptions& options, std::string opt, std::string alt_opt = "") {
	if (options.count(opt) == 1) {
		return true;
	} else {
		return !alt_opt.empty() && options.count(alt_opt) == 1;
	}
}

inline std::optional<std::string> get_value_opt(const subparser::ValueOptions& options, std::string opt, std::string alt_opt = "") {
    auto it = options.find(opt);
    if (it != options.end()) {
        return it->second;
    } else if (!alt_opt.empty()) {
        
        auto alt_it = options.find(opt);
        if (alt_it == options.end()) {
            return alt_it->second;
        }
    }
    
    return std::nullopt;
}