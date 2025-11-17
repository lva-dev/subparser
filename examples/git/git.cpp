#include <iostream>
#include <subparser/subparser.h>
#include <unordered_map>
// commands
#include "add.h"
#include "branch.h"
#include "clone.h"
#include "commit.h"
#include "pull.h"
#include "push.h"
#include "reset.h"
#include "rm.h"
#include "status.h"
// utils
#include "util.h"

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

int main(int argc, char **argv) {
	subparser::parser parser;
	subparser::parse_result result = parser.value_opt("-m").parse(argc, argv);

	if (!result) {
		std::cerr << "error parsing command-line: " << result.error().description() << '\n';
		return 1;
	}

	auto command = result.command();

	if (has_opt(command.bool_opts, "--help", "-h")) {
		std::cout << usage() << '\n';
		return 0;
	}

	if (has_opt(command.bool_opts, "--version", "-v")) {
		std::cout << "0.1.0\n";
		return 0;
	}

	auto subcommand = command.subcommand;
	if (!subcommand) {
		std::cout << usage() << '\n';
		return 0;
	}

	for (auto& arg : subcommand->arguments) {
		std::cerr << "arg: '" + arg + "'\n";
	}

	for (auto& opt : subcommand->bool_opts) {
		std::cerr << "opt: '" + opt + "'\n";
	}

	for (auto& pair : subcommand->value_opts) {
		std::cerr << "opt: '" + pair.first + "'='" + pair.second + "'\n";
	}

	auto& cmd = subcommand->name;
	if (cmd == "add") {
		return add(subcommand.value()) ? 0 : 1;
	} else if (cmd == "branch") {
		return branch(subcommand.value()) ? 0 : 1;
	} else if (cmd == "clone") {
		return clone(subcommand.value()) ? 0 : 1;
	} else if (cmd == "commit") {
		return commit(subcommand.value()) ? 0 : 1;
	} else if (cmd == "pull") {
		return pull(subcommand.value()) ? 0 : 1;
	} else if (cmd == "push") {
		return push(subcommand.value()) ? 0 : 1;
	} else if (cmd == "reset") {
		return reset(subcommand.value()) ? 0 : 1;
	} else if (cmd == "rm") {
		return rm(subcommand.value()) ? 0 : 1;
	} else if (cmd == "status") {
		return status(subcommand.value()) ? 0 : 1;
	} else {
		std::cerr << "error: unknown command '" + cmd + "'\n";
		return 1;
	}

	return 0;
}