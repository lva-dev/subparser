#include <iostream>
#include <subparser/subparser.h>
#include <unordered_map>

#include "add.h"
#include "branch.h"
#include "clone.h"
#include "commit.h"
#include "pull.h"
#include "push.h"
#include "reset.h"
#include "rm.h"
#include "status.h"

const std::string& usage() {
	static const std::string USAGE = R"(usage: git [-v | --version] [-h | --help] <command> [<args>]

commands:
  add
  branch
  clone
  commit
  pull
  push
  reset
  rm
  status)";
	return USAGE;
}

bool has_opt(const subparser::Options& options, std::string opt, std::string alt_opt = "") {
	if (options.count(opt) == 1) {
		return true;
	} else {
		return !alt_opt.empty() && options.count(alt_opt) == 1;
	}
}

int main(int argc, char **argv) {
	subparser::Subparser parser;
	subparser::SubparseResult result = parser.parse(argc, argv);

	if (has_opt(result.boolean_options, "--help", "-h")) {
		std::cout << usage() << '\n';
		return 0;
	}

	if (has_opt(result.boolean_options, "--version", "-v")) {
		std::cout << "0.1.0\n";
		return 0;
	}

	if (!result.command) {
		std::cout << usage() << '\n';
		return 0;
	}

	auto& cmd = result.command->name;
	if (cmd == "add") {
        return add(result.command.value()) ? 0 : 1;
	} else if (cmd == "branch") {
        return branch(result.command.value()) ? 0 : 1;
	} else if (cmd == "clone") {
        return clone(result.command.value()) ? 0 : 1;
	} else if (cmd == "commit") {
        return commit(result.command.value()) ? 0 : 1;
	} else if (cmd == "pull") {
        return pull(result.command.value()) ? 0 : 1;
	} else if (cmd == "push") {
        return push(result.command.value()) ? 0 : 1;
	} else if (cmd == "reset") {
        return reset(result.command.value()) ? 0 : 1;
	} else if (cmd == "rm") {
        return rm(result.command.value()) ? 0 : 1;
	} else if (cmd == "status") {
        return status(result.command.value()) ? 0 : 1;
	} else {
		std::cerr << "error: unknown command '" + cmd + "'\n";
		return 1;
	}

	return 0;
}