#include <lib/parser.h>

#include <gtest/gtest.h>
#include <sstream>

using namespace omfl;

TEST(ParserTestSuite, EmptyTest) {
    std::string data = "";

    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());
}

TEST(ParserTestSuite, IntTest) {
    std::string data = R"(
        key1 = 100500
        key2 = -22
        key3 = +28)";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_TRUE(root.Get("key1").IsInt());
    ASSERT_TRUE(root.Get("key2").IsInt());
    ASSERT_TRUE(root.Get("key3").IsInt());

    ASSERT_EQ(root.Get("key1").AsInt(), 100500);
    ASSERT_EQ(root.Get("key2").AsInt(), -22);
    ASSERT_EQ(root.Get("key3").AsInt(), 28);
}

TEST(ParserTestSuite, InvalidIntTest) {
    std::string data = R"(
        key1 = true
        key2 = -22.1
        key3 = "ITMO")";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_FALSE(root.Get("key1").IsInt());
    ASSERT_FALSE(root.Get("key2").IsInt());
    ASSERT_FALSE(root.Get("key3").IsInt());

    ASSERT_EQ(root.Get("key1").AsIntOrDefault(1), 1);
    ASSERT_EQ(root.Get("key2").AsIntOrDefault(-2), -2);
    ASSERT_EQ(root.Get("key3").AsIntOrDefault(3), 3);
}

TEST(ParserTestSuite, FloatTest) {
    std::string data = R"(
        key1 = 2.1
        key2 = -3.14
        key3 = -0.001)";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_TRUE(root.Get("key1").IsFloat());
    ASSERT_TRUE(root.Get("key2").IsFloat());
    ASSERT_TRUE(root.Get("key3").IsFloat());

    ASSERT_FLOAT_EQ(root.Get("key1").AsFloat(), 2.1f);
    ASSERT_FLOAT_EQ(root.Get("key2").AsFloat(), -3.14f);
    ASSERT_FLOAT_EQ(root.Get("key3").AsFloat(), -0.001f);
}

TEST(ParserTestSuite, InvalidFloatTest) {
    std::string data = R"(
        key1 = true
        key2 = -2
        key3 = "ITMO")";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_FALSE(root.Get("key1").IsFloat());
    ASSERT_FALSE(root.Get("key2").IsFloat());
    ASSERT_FALSE(root.Get("key3").IsFloat());

    ASSERT_FLOAT_EQ(root.Get("key1").AsFloatOrDefault(2.1), 2.1);
    ASSERT_FLOAT_EQ(root.Get("key2").AsFloatOrDefault(-3.14), -3.14);
    ASSERT_FLOAT_EQ(root.Get("key3").AsFloatOrDefault(-0.001), -0.001);
}

TEST(ParserTestSuite, StringTest) {
    std::string data = R"(
        key = "value"
        key1 = "value1")";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_TRUE(root.Get("key").IsString());
    ASSERT_TRUE(root.Get("key1").IsString());

    ASSERT_EQ(root.Get("key").AsString(), "value");
    ASSERT_EQ(root.Get("key1").AsString(), "value1");
}

TEST(ParserTestSuite, InvalidStringTest) {
    std::string data = R"(
        key = true
        key1 = ["1", "2", "3"])";


    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());

    ASSERT_FALSE(root.Get("key").IsString());
    ASSERT_FALSE(root.Get("key1").IsString());

    ASSERT_EQ(root.Get("key").AsStringOrDefault("Hello"), "Hello");
    ASSERT_EQ(root.Get("key1").AsStringOrDefault("World"), "World");
}


TEST(ParserTestSuite, CommentsTest) {
    std::string data = R"(
        key1 = 100500  # some important value

        # It's more then university)";

    OMFLParser root = parse(data);

    ASSERT_TRUE(root.valid());
    ASSERT_EQ(root.Get("key1").AsInt(), 100500);
}

TEST(ParserTestSuite, SectionTest) {
    std::string data = R"(
        [section1]
        key1 = 1
        key2 = true

        [section1]
        key3 = "value")";

    OMFLParser root = parse(data);
    ASSERT_TRUE(root.valid());

    ASSERT_EQ(root.Get("section1.key1").AsInt(), 1);
    ASSERT_EQ(root.Get("section1.key2").AsBool(), true);
    ASSERT_EQ(root.Get("section1.key3").AsString(), "value");
}

TEST(ParserTestSuite, MultiSectionTest) {
    std::string data = R"(
        [level1]
        key1 = 1
        [level1.level2-1]
        key2 = 2

        [level1.level2-2]
        key3 = 3)";

    OMFLParser root = parse(data);
    ASSERT_TRUE(root.valid());

    ASSERT_EQ(root.Get("level1.key1").AsInt(), 1);
    ASSERT_EQ(root.Get("level1.level2-1.key2").AsInt(), 2);
    ASSERT_EQ(root.Get("level1.level2-2.key3").AsInt(), 3);
}


TEST(ParserTestSuite, GetSectionTest) {
    std::string data = R"(
        [level1.level2.level3]
        key1 = 1)";

    OMFLParser root = parse(data);
    ASSERT_TRUE(root.valid());

    ASSERT_EQ(root.Get("level1").Get("level2").Get("level3").Get("key1").AsInt(), 1);
}