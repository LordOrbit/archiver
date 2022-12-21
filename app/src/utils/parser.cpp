#include "parser.h"

#include <cstring>

// ArgumentValue
size_t ArgumentValue::Size() const {
    return end_ - begin_;
}

bool ArgumentValue::Empty() const {
    return Size() == 0;
}

void ArgumentValue::Add(char* value) {
    values_.push_back(value);
    ++end_;
}

char* ArgumentValue::operator[](size_t index) {
    return values_[begin_ + index];
}

const char* ArgumentValue::operator[](size_t index) const {
    return values_[begin_ + index];
}

char* ArgumentValue::First() const {
    return values_[0];
}

char* ArgumentValue::Last() const {
    return values_.back();
}

ArgumentValue ArgumentValue::SubArray(size_t begin) const {
    return ArgumentValue(values_, begin_ + begin, end_);
}

ArgumentValue ArgumentValue::SubArray(size_t begin, size_t end) const {
    return ArgumentValue(values_, begin_ + begin, begin_ + end);
}

// Iterators for values_
auto ArgumentValue::begin() {
    return values_.begin() + static_cast<std::vector<std::string*>::difference_type>(begin_);
}

auto ArgumentValue::end() {
    return values_.begin() + static_cast<std::vector<std::string*>::difference_type>(end_);
}

auto ArgumentValue::begin() const {
    return values_.begin() + static_cast<std::vector<std::string*>::difference_type>(begin_);
}

auto ArgumentValue::end() const {
    return values_.begin() + static_cast<std::vector<std::string*>::difference_type>(end_);
}

ArgumentValue::operator std::vector<std::string>() {
    std::vector<std::string> values;
    for (auto value_it = begin(); value_it != end(); ++value_it) {
        values.push_back(*value_it);
    }
    return values;
}

// Parsing given data in argv
Parser::Parser(int argc, char** argv, const std::unordered_map<char, std::string>& short_defines,
               const std::unordered_set<std::string> possible_arguments) {
    size_t i = 1;
    std::string argument = "-";
    while (i < argc) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            argument = std::string(argv[i]).substr(2);
            if (HasArgument(argument)) {
                throw DuplicateArgument(argument);
            }
            if (!possible_arguments.contains(argument)) {
                throw UndefinedArgument(argument);
            }
            arguments_[argument] = ArgumentValue();
        } else if (argv[i][0] == '-') {
            if (argv[i][1] == '\0' || argv[i][2] != '\0') {
                throw UndefinedShortArgument(std::string(argv[i]).substr(1));
            }
            auto def = short_defines.find(argv[i][1]);
            if (def == short_defines.end()) {
                throw UndefinedShortArgument(argv[i][1]);
            }
            argument = def->second;
            if (HasArgument(argument)) {
                throw DuplicateArgument(argument);
            }
            if (!possible_arguments.contains(argument)) {
                throw UndefinedArgument(argument);
            }
            arguments_[argument] = ArgumentValue();
        } else {
            if (argument == "-") {
                arguments_[argument] = ArgumentValue();
            }
            if (!possible_arguments.contains(argument)) {
                throw UndefinedArgument(argument);
            }
            arguments_[argument].Add(argv[i]);
        }
        ++i;
    }
}

bool Parser::HasArgument(const std::string& argument) const {
    return arguments_.contains(argument);
}

ArgumentValue& Parser::operator[](const std::string& argument) {
    return arguments_[argument];
}

const ArgumentValue& Parser::operator[](const std::string& argument) const {
    return arguments_.at(argument);
}

// Exceptions

// Constructors of UndefinedShortArgument
Parser::UndefinedShortArgument::UndefinedShortArgument(const std::string& argument) {
    std::string full_description = "Undefined short argument (-" + argument + ").";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}
Parser::UndefinedShortArgument::UndefinedShortArgument(char argument) {
    std::string full_description = "Undefined short argument (-" + std::string(argument, 1) + ").";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}

// Return more information about UndefinedShortArgument exception
const char* Parser::UndefinedShortArgument::what() const noexcept {
    return description_;
}

// Constructor of UndefinedArgument
Parser::UndefinedArgument::UndefinedArgument(const std::string& argument) {
    std::string full_description = "Undefined argument (--" + argument + ").";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}

// Return more information about UndefinedArgument exception
const char* Parser::UndefinedArgument::what() const noexcept {
    return description_;
}

// Constructor of DuplicateArgument
Parser::DuplicateArgument::DuplicateArgument(const std::string& argument) {
    std::string full_description = "Duplicate argument (--" + argument + ").";
    description_ = new char[full_description.size()];
    std::strcpy(description_, full_description.c_str());
}

// Return more information about DuplicateArgument exception
const char* Parser::DuplicateArgument::what() const noexcept {
    return description_;
}
