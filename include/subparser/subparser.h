#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace subparser {
	using Options = std::unordered_set<std::string>;
	// using Options = std::unordered_map<std::string, std::string>;
	using Arguments = std::vector<std::string>;

	struct Command {
		std::string name;
		Options boolean_options;
		Arguments arguments;
	};

	struct SubparseResult {
		Options boolean_options;
		std::unordered_map<std::string, std::string> value_options;
		std::optional<Command> command;
	};

	class Subparser {
	private:
		Options _options_with_required_values;
	public:
		inline SubparseResult parse(int argc, char **argv) {
			std::vector<std::string> args_view = {argv + 1, argv + argc};
			return parse(args_view);
		}

		inline SubparseResult parse(const std::vector<std::string>& args) {
			SubparseResult result;

			std::size_t arg_index = 0;
			while (arg_index < args.size() && _is_option(args[arg_index])) {
				_parse_option(arg_index, args, result);
				arg_index++;
			}

			std::optional<std::string> cmd_name;

			if (arg_index < args.size()) {
				result.command = Command {};
				result.command->name = args[arg_index];

				bool onlyArgs = false;
				for (std::size_t i = arg_index + 1; i < args.size(); i++) {
					std::string arg {args[i]};
					if (!onlyArgs && _is_option(arg)) {
						result.command->boolean_options.insert(arg);
					} else {
						if (!onlyArgs && arg == "--") {
							onlyArgs = true;
						} else {
							result.command->arguments.push_back(arg);
						}
					}
				}
			}

			return result;
		}
	private:
		inline bool _parse_option(std::size_t index,
			const std::vector<std::string>& args,
			SubparseResult& result) {
			std::string option = args[index];
			if (_options_with_required_values.count(option) == 1) {
				if (index == args.size() - 1) {
					// index refers to the last argument, meaning this value option was not supplied
					// an argument
					return false;
				}

				result.value_options[option] = args[index + 1];
			} else {
				result.boolean_options.insert(option);
			}

			return true;
		}

		inline Options _parse_short_option(std::string_view option_str) {
			using namespace std::string_literals;

			if (!_is_short_option(option_str)) {
				return {};
			}

			Options optionsSet;
			for (std::size_t i = 1; i < option_str.size(); i++) {
				optionsSet.insert("-"s + option_str[i]);
			}

			return optionsSet;
		}

		inline bool _is_option(std::string_view arg) {
			if (arg.size() >= 1) {
				if (arg[0] == '-') {
					if (arg.size() == 1) {
						return false;
					}

					if (arg[1] == '-') {
						if (arg.size() == 2) {
							return false;
						}
					}

					return true;
				}
			}

			return false;
		}

		inline bool _is_short_option(std::string_view arg) {
			if (arg.size() >= 1) {
				if (arg[0] == '-') {
					if (arg.size() == 1 || arg[1] == '-') {
						return false;
					}

					return true;
				}
			}

			return false;
		}
	};
} // namespace subparser