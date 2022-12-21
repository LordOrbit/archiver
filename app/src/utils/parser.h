#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class ArgumentValue {
public:
    ArgumentValue() : values_(), begin_(0), end_(0){};
    ArgumentValue(std::vector<char*> values, size_t begin, size_t end) : values_(values), begin_(begin), end_(end){};
    ArgumentValue(std::vector<char*> values) : values_(values), begin_(0), end_(values.size()){};  // NOLINT
    ArgumentValue(char* value) : values_({value}), begin_(0), end_(1){};                           // NOLINT

    operator std::vector<std::string>();  // NOLINT

    size_t Size() const;
    bool Empty() const;

    void Add(char* value);

    char* operator[](size_t index);
    const char* operator[](size_t index) const;
    char* First() const;
    char* Last() const;
    ArgumentValue SubArray(size_t begin) const;
    ArgumentValue SubArray(size_t begin, size_t end) const;

    auto begin();        // NOLINT
    auto end();          // NOLINT
    auto begin() const;  // NOLINT
    auto end() const;    // NOLINT

private:
    std::vector<char*> values_;
    size_t begin_;
    size_t end_;
};

class Parser {
public:
    class UndefinedShortArgument : public std::exception {
    public:
        explicit UndefinedShortArgument(const std::string& argument);
        explicit UndefinedShortArgument(char argument);

        const char* what() const noexcept override;

    private:
        char* description_;
    };
    class UndefinedArgument : public std::exception {
    public:
        explicit UndefinedArgument(const std::string& argument);

        const char* what() const noexcept override;

    private:
        char* description_;
    };
    class DuplicateArgument : public std::exception {
    public:
        explicit DuplicateArgument(const std::string& argument);

        const char* what() const noexcept override;

    private:
        char* description_;
    };

    Parser() : arguments_(){};
    Parser(int argc, char** argv, const std::unordered_map<char, std::string>& short_defines,
           const std::unordered_set<std::string> possible_arguments);

    bool HasArgument(const std::string& argument) const;
    ArgumentValue& operator[](const std::string& argument);
    const ArgumentValue& operator[](const std::string& argument) const;

private:
    std::unordered_map<std::string, ArgumentValue> arguments_;
};
