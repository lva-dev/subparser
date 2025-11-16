#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace subparser {
	using BooleanOptions = std::unordered_set<std::string>;
	using ValueOptions = std::unordered_map<std::string, std::string>;
	using Arguments = std::vector<std::string>;

	struct Command {
		std::string name;
		BooleanOptions boolean_options;
		ValueOptions value_options;
		Arguments arguments;
	};

	struct SubparseResult {
		BooleanOptions boolean_options;
		ValueOptions value_options;
		std::optional<Command> command;
	};

	class Subparser {
	private:
		BooleanOptions _options_with_required_values;
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
						_parse_option(arg_index, args, result.command.value());
						arg_index++;
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
			return _parse_option(index, args, result.boolean_options, result.value_options);
		}

		inline bool
		_parse_option(std::size_t index, const std::vector<std::string>& args, Command& command) {
			return _parse_option(index, args, command.boolean_options, command.value_options);
		}

		inline bool _parse_option(std::size_t index,
			const std::vector<std::string>& args,
			BooleanOptions& boolean_options,
			ValueOptions& value_options) {

			std::string option = args[index];
			if (_options_with_required_values.count(option) == 1) {
				if (index == args.size() - 1) {
					// index refers to the last argument, meaning this value option was not supplied
					// an argument
					return false;
				}

				value_options[option] = args[index + 1];
			} else {
				boolean_options.insert(option);
			}

			return true;
		}

		inline BooleanOptions _parse_short_option(std::string_view option_str) {
			using namespace std::string_literals;

			if (!_is_short_option(option_str)) {
				return {};
			}

			BooleanOptions optionsSet;
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