#include <catch.hpp>
#include <memory>
#include <queue>
#include <vector>

#include "src/long_code.h"
#include "src/utils/bit_reader.h"
#include "src/utils/bit_writer.h"
#include "src/utils/counter.h"
#include "src/utils/file.h"
#include "src/utils/parser.h"
#include "src/utils/priority_queue.h"
#include "src/utils/round.h"
#include "src/utils/trie.h"
#include "src/utils/weight.h"

template <typename K, typename V>
void RequireEquality(const std::unordered_map<K, V>& actual, const std::unordered_map<K, V>& expected) {
    REQUIRE(actual.size() == expected.size());
    for (const auto& [key, value] : actual) {
        auto exp_entry = expected.find(key);
        REQUIRE(exp_entry != expected.end());
        REQUIRE(exp_entry->second == value);
    }
}

template <typename T>
void RequireEquality(const std::vector<T>& actual, const std::vector<T>& expected) {
    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        REQUIRE(actual[i] == expected[i]);
    }
}

template <typename T>
void RequireEquality(const LongCode& actual, const std::vector<T>& expected) {
    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        REQUIRE(actual[i] == expected[i]);
    }
}

void RequireEquality(const LongCode& actual, const LongCode& expected) {
    REQUIRE(actual.size() == expected.size());
    for (size_t i = 0; i < actual.size(); ++i) {
        REQUIRE(actual[i] == expected[i]);
    }
}

TEST_CASE("BitStream") {
    {
        std::ostringstream oss;
        std::ostream& ostream(oss);
        BitWriter bit_writer(ostream);

        std::vector<char> expected = {'0', '1', '2', '3'};
        for (char c : expected) {
            bit_writer.Write(c, 8);
        }

        REQUIRE(bit_writer.ByteCount() == 4);
        bit_writer.Complete();
        REQUIRE(bit_writer.ByteCount() == 4);
        bit_writer.Close();

        REQUIRE(oss.str() == "0123");

        std::istringstream iss("0123");
        std::istream& istream(iss);
        BitReader bit_reader(istream);

        std::vector<char> actual;
        char value = 0;
        while (bit_reader.Get(value, 8)) {
            REQUIRE(bit_reader.IsEOF() == (value == '3'));
            actual.push_back(value);
        }

        REQUIRE(bit_reader.ByteCount() == 4);

        RequireEquality(actual, expected);
    }

    {
        std::ostringstream oss;
        std::ostream& ostream(oss);
        BitWriter bit_writer(ostream);

        std::vector<bool> expected = {false, false, true, false, true, false, false, false};
        for (int i = 0; i < 6; ++i) {
            bit_writer.Write(expected[i]);
        }

        REQUIRE(bit_writer.ByteCount() == 0);
        bit_writer.Complete();
        REQUIRE(bit_writer.ByteCount() == 1);
        bit_writer.Close();

        std::istringstream iss("(");
        std::istream& istream(iss);
        BitReader bit_reader(istream);

        std::vector<bool> actual;
        bool value = false;
        REQUIRE(bit_reader.ByteCount() == 0);
        while (bit_reader.Get(value)) {
            REQUIRE(bit_reader.IsEOF() == (actual.size() == expected.size() - 1));
            actual.push_back(value);
        }

        REQUIRE(bit_reader.ByteCount() == 1);

        RequireEquality(actual, expected);
    }
}

TEST_CASE("LongCode") {
    {
        LongCode long_code({false, false, false});
        RequireEquality(long_code, LongCode(3));

        long_code.Push(true);
        std::vector<bool> actual({false, false, false, true});
        RequireEquality(long_code, actual);
        REQUIRE(long_code.Pop());
        actual.pop_back();
        RequireEquality(long_code, actual);

        long_code = LongCode({true, true, true});
        actual = {true, false, false, false};
        long_code++;
        RequireEquality(long_code, actual);
        long_code = long_code << 1;
        actual = {true, false, false, false, false};
        RequireEquality(long_code, actual);
    }
}

TEST_CASE("File") {
    {
        std::string file_path = "file.txt";
        File file(file_path);
        REQUIRE(file.GetPath() == file_path);
        REQUIRE(file.GetName() == file_path);
    }
    {
        std::string file_path = "../dir1/dir2/file.txt";
        File file(file_path);
        REQUIRE(file.GetPath() == file_path);
        REQUIRE(file.GetName() == "file.txt");
    }
}

TEST_CASE("Weight") {
    {
        REQUIRE(static_cast<std::string>(Weight(0)) == "0B");
        REQUIRE(static_cast<std::string>(Weight(1)) == "1B");
        REQUIRE(static_cast<std::string>(Weight(8)) == "8B");
        REQUIRE(static_cast<std::string>(Weight(1023)) == "1023B");

        REQUIRE(static_cast<std::string>(Weight(1024)) == "1Kb");
        REQUIRE(static_cast<std::string>(Weight(1025)) == "1.001Kb");
        REQUIRE(static_cast<std::string>(Weight(1026)) == "1.002Kb");
        REQUIRE(static_cast<std::string>(Weight(2048)) == "2Kb");

        REQUIRE(static_cast<std::string>(Weight(10250)) == "10.01Kb");
        REQUIRE(static_cast<std::string>(Weight(102500)) == "100.1Kb");

        REQUIRE(static_cast<std::string>(Weight(1024 * 1024)) == "1Mb");
        REQUIRE(static_cast<std::string>(Weight(1024 * 1024 * 1024)) == "1Gb");
    }

    {
        Weight a(0);
        Weight b(1024);

        REQUIRE(b == a + b);

        REQUIRE(b < a + b + b);
        REQUIRE(b + b > b);

        REQUIRE(Weight(1024 * 8) / Weight(8) == 1024);
    }
}

TEST_CASE("PriorityQueue") {
    {
        std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> origin;
        PriorityQueue<int64_t, std::greater<int64_t>> custom({4, 3, 3, 2, 1});

        origin.push(4);
        origin.push(3);
        origin.push(3);
        origin.push(2);
        origin.push(1);

        REQUIRE(origin.size() == custom.Size());
        REQUIRE(origin.top() == custom.Top());
        origin.pop();
        custom.Pop();
        REQUIRE(origin.top() == custom.Top());
        origin.push(0);
        custom.Push(0);
        REQUIRE(origin.top() == custom.Top());
        REQUIRE(origin.size() == custom.Size());

        REQUIRE(PriorityQueue<int64_t>().Empty());
        REQUIRE(!PriorityQueue<int64_t>({1}).Empty());
    }
}

TEST_CASE("Round") {
    {
        REQUIRE(Round(123.2, 0) == 123);
        REQUIRE(Round(123.5, 0) == 124);

        REQUIRE(Round(123.12, 1) == 123.1);
        REQUIRE(Round(123.15, 1) == 123.2);
    }
}

TEST_CASE("Counter") {
    {
        FileBitWriter bit_writer("counter.txt");

        size_t cnt0 = 228;
        size_t cnt1 = 47;
        for (size_t i = 0; i < cnt0; ++i) {
            bit_writer.Write(false);
        }
        for (size_t i = 0; i < cnt1; ++i) {
            bit_writer.Write(true);
        }
        bit_writer.Close();

        FileBitReader bit_reader("counter.txt");
        Counter<bool> counter;

        counter.Process(bit_reader, 1);
        REQUIRE(counter[false] == (cnt0 + cnt1 + 7) / 8 * 8 - cnt1);
        REQUIRE(counter[true] == cnt1);
        counter.Add(true, 2);
        REQUIRE(counter[true] == cnt1 + 2);

        std::remove("counter.txt");
    }
}

TEST_CASE("ArgumentValue") {
    {
        REQUIRE(ArgumentValue().Size() == 0);
        char c123[] = "123";
        char c3123[] = "3123";
        std::vector<char*> arguments = {c123, c3123};

        ArgumentValue argument_value(arguments);
        REQUIRE(argument_value.Size() == 2);
        REQUIRE(argument_value.SubArray(1).Size() == 1);
        REQUIRE(argument_value.SubArray(0, 1).Size() == 1);
        REQUIRE(argument_value.SubArray(1, 1).Size() == 0);
        REQUIRE(argument_value.SubArray(1).SubArray(1).Size() == 0);

        REQUIRE(argument_value[0] == c123);
        REQUIRE(argument_value.First() == c123);
        REQUIRE(argument_value[1] == c3123);
        REQUIRE(argument_value.Last() == c3123);

        argument_value.Add(c123);
        REQUIRE(argument_value.Last() == c123);
        REQUIRE(argument_value.Size() == 3);
    }
}

TEST_CASE("Parser") {
    {
        std::vector<std::string> argv_vector(
            {"/dir/path/to/something", "--fruit", "apple", "apple", "-o", "car", "tree", "--one", "something"});
        int argc = static_cast<int>(argv_vector.size());
        char* argv[argv_vector.size()];

        for (size_t i = 0; i < argv_vector.size(); ++i) {
            const auto& string = argv_vector[i];
            argv[i] = new char[string.size()];
            strcpy(argv[i], string.c_str());
        }

        Parser parser(argc, argv, {{'o', "object"}}, {"fruit", "one", "object"});
        REQUIRE(parser.HasArgument("fruit"));
        REQUIRE(parser.HasArgument("one"));
        REQUIRE(parser.HasArgument("object"));
        REQUIRE(!parser.HasArgument("blablabla"));

        REQUIRE(parser["fruit"].First() == argv[2]);
        REQUIRE(parser["fruit"].Last() == argv[3]);

        REQUIRE(parser["object"].First() == argv[5]);
        REQUIRE(parser["object"].Last() == argv[6]);

        REQUIRE(parser["one"].First() == argv[8]);
    }
}

TEST_CASE("Trie") {
    {
        std::shared_ptr<Node<int64_t>> root = std::make_shared<Node<int64_t>>(Node<int64_t>({.value = -1}));
        Trie<int64_t> trie(root);

        std::unordered_map<int64_t, LongCode> expected({{-1, LongCode()}});
        std::unordered_map<int64_t, LongCode> actual = trie.GetMap();
        RequireEquality(actual, expected);

        expected = {{1, LongCode({false, false, true})}};
        trie.Add(LongCode({false, false, true}), 1);
        actual = trie.GetMap();
        RequireEquality(actual, expected);

        expected[2] = LongCode({false, false, false});
        trie.Add(LongCode({false, false, false}), 2);
        actual = trie.GetMap();
        RequireEquality(actual, expected);

        REQUIRE(!trie.Trace(true));
        REQUIRE(!trie.IsTraceLeaf());

        REQUIRE(trie.Trace(false));
        REQUIRE(!trie.IsTraceLeaf());
        REQUIRE(!trie.Trace(true));

        REQUIRE(trie.Trace(false));
        REQUIRE(trie.Trace(true));

        REQUIRE(trie.IsTraceLeaf());
        REQUIRE(trie.TraceValue() == 1);

        trie.ResetTrace();
        REQUIRE(!trie.IsTraceLeaf());
    }
}
