#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace subparser {
	using boolean_options = std::unordered_set<std::string>;
	using value_options = std::unordered_map<std::string, std::string>;
	using Arguments = std::vector<std::string>;

	struct SubCommand {
		std::string name;
		boolean_options bool_opts;
		value_options value_opts;
		Arguments arguments;
	};

	struct main_command {
		boolean_options bool_opts;
		value_options value_opts;
		std::optional<SubCommand> subcommand;
	};

	class parse_error {
	private:
		std::string _message;
	public:
		parse_error(const std::string& message) : _message {message} {}

		std::string_view description() const noexcept { return _message; }
	};

	class parse_result {
	private:
		friend class parser;

		bool _good;

		std::variant<main_command, parse_error> _value;

		parse_result(main_command&& command) : _good {true}, _value {command} {}

		parse_result(const std::string& error) : _good {false}, _value {parse_error {error}} {}
	public:
		parse_result() : _good {false}, _value {parse_error {""}} {};

		parse_result(const parse_result& other) = default;
		parse_result& operator=(const parse_result& other) = default;
		parse_result(parse_result&& other) = default;
		parse_result& operator=(parse_result&& other) = default;
		~parse_result() = default;

		bool failed() const { return !_good; }

		bool succeeded() const { return _good; }

		operator bool() const { return _good; }

		main_command& command() { return std::get<0>(_value); }

		parse_error& error() { return std::get<1>(_value); }
	};

	class parser {
	private:
		std::unordered_set<std::string> _allowed_bool_opts;
		std::unordered_set<std::string> _allowed_value_opts;
	public:
		parser& bool_opt(const std::string& opt) {
			_allowed_bool_opts.insert(opt);
			return *this;
		}

		parser& value_opt(const std::string& opt) {
			_allowed_value_opts.insert(opt);
			return *this;
		}

		inline parse_result parse(int argc, char **argv) const {
			std::vector<std::string> args = {argv + 1, argv + argc};
			return parse(args);
		}

		inline parse_result parse(const std::vector<std::string>& args) const {
			parse_result result;
			main_command command;

			std::size_t arg_index = 0;
			while (arg_index < args.size() && _is_option(args[arg_index])) {
				if (!_parse_option(arg_index, args, command, result)) {
					return {};
				}
			}

			std::optional<std::string> cmd_name;

			if (arg_index < args.size()) {
				command.subcommand = SubCommand {};
				command.subcommand->name = args[arg_index];

				bool only_args = false;
				for (std::size_t i = arg_index + 1; i < args.size(); i++) {
					std::string arg {args[i]};
					if (!only_args && _is_option(arg)) {
						if (!_parse_option(i, args, command.subcommand.value(), result)) {
							return result;
						}
					} else {
						if (!only_args && arg == "--") {
							only_args = true;
						} else {
							command.subcommand->arguments.push_back(arg);
						}
					}
				}
			}

			result = parse_result {std::move(command)};
			return result;
		}
	private:
		inline bool _parse_option(std::size_t index,
			const std::vector<std::string>& args,
			main_command& maincommand,
			parse_result& result) const {
			return _parse_option(
				index, args, maincommand.bool_opts, maincommand.value_opts, result);
		}

		inline bool _parse_option(std::size_t& index,
			const std::vector<std::string>& args,
			SubCommand& subcommand,
			parse_result& result) const {
			return _parse_option(index, args, subcommand.bool_opts, subcommand.value_opts, result);
		}

		inline bool _parse_option(std::size_t& index,
			const std::vector<std::string>& args,
			boolean_options& b_opts,
			value_options& v_opts,
			parse_result& result) const {

			std::string option = args[index];
			if (_allowed_value_opts.count(option)) {
				if (index == args.size() - 1) {
					// index refers to the last argument, meaning this value option was not supplied
					// an argument
					return false;
				}

				v_opts[option] = args[index + 1];
				index += 2;
			} else {
				b_opts.insert(option);
				index++;
			}

			return true;
		}

		std::optional<std::string> _get_value_after(std::size_t& index,
			const std::vector<std::string>& args,
			parse_result& result) const {
			if (index == args.size() - 1) {
				// index refers to the last argument, meaning this value option was not supplied
				// an argument
				return {};
			}

			index++;
			return args[index];
		}

		inline bool _parse_long_option(std::size_t& index,
			const std::vector<std::string>& args,
			boolean_options& b_opts,
			value_options& v_opts,
			parse_result& result) const {

			std::string option = args[index];
			assert(_is_long_option(option));
			if (_allowed_value_opts.count(option)) {
				auto value = _get_value_after(index, args, result);
				if (!value) {
					return false;
				}

				v_opts[option] = value.value();
			} else {
				b_opts.insert(option);
			}

			index++;
			return true;
		}

		inline bool _parse_short_option(std::size_t& index,
			const std::vector<std::string>& args,
			boolean_options& b_opts,
			value_options& v_opts,
			parse_result& result) {

			using namespace std::string_literals;

			std::string_view opt_str = args[index];
			assert(_is_short_option(opt_str));

			for (std::size_t i = 1; i < opt_str.size(); i++) {
				auto opt = "-"s + opt_str[i];
				if (_allowed_value_opts.count(opt)) {
					std::string_view value;
					if (i == opt_str.size() - 1) {
						auto value_opt = _get_value_after(index, args, result);
						if (!value_opt) {
							return false;
						}

                        value = value_opt.value();
					} else {
                        value = opt_str.substr(i + 1);
                    }
                    
                    v_opts[opt] = value;
				}

				b_opts.insert(opt);
			}

			return true;
		}

		inline static bool _is_option(std::string_view arg) {
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

		inline static bool _is_short_option(std::string_view arg) {
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

		inline static bool _is_long_option(std::string_view arg) {
			// note: when arg starts with '--':
			//  if size >= 3, it is a long option
			//  if size == 2, it is the '--' argument
			if (arg.size() >= 3) {
				return arg[0] == '-' && arg[1] == '-';
			}

			return false;
		}
	};
} // namespace subparser