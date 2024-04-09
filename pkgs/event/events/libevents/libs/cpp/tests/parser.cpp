
#include "parse/parser.h"

#include <events_generated.h>

// always keep assertions. Could use gtest as well...
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>

int main(int argc, char** argv)
{
    events::parser::Parser p;
    bool ret = p.loadDefinitionsFile("../../test.json");
    assert(ret);

    p.setProfile("dev");

    p.formatters().url = [](const std::string& content, const std::string& link) {
        printf("formatting url: link=%s, content=%s\n", content.c_str(), link.c_str());
        return "#" + content + "<link=" + link + ">#";
    };

    {
        events::EventType event = events::test::create_test1(events::Log::Info);

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 2306236 | (1 << 24));
        assert(parsed_event->name() == "test1");
        assert(parsed_event->group() == "default");
        assert(parsed_event->numArguments() == 0);
        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Test");
        assert(parsed_event->description() == "");
    }

    {
        int8_t a = -3;
        int16_t b = -9238;
        int32_t c = 832223;
        int64_t d = -9277377287318721;
        float e = 3423423.3423f;
        events::EventType event = events::test::create_test2(events::Log::Info, a, b, c, d, e);

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 2307445 | (1 << 24));
        assert(parsed_event->name() == "test2");
        assert(parsed_event->group() == "default");

        assert(parsed_event->numArguments() == 5);
        assert(parsed_event->argument(0).type == events::parser::BaseType::int8_t);
        assert(parsed_event->argumentValue(0).value.val_int8_t == a);
        assert(parsed_event->argument(1).type == events::parser::BaseType::int16_t);
        assert(parsed_event->argumentValue(1).value.val_int16_t == b);
        assert(parsed_event->argument(2).type == events::parser::BaseType::int32_t);
        assert(parsed_event->argumentValue(2).value.val_int32_t == c);
        assert(parsed_event->argument(3).type == events::parser::BaseType::int64_t);
        assert(parsed_event->argumentValue(3).value.val_int64_t == d);
        assert(parsed_event->argument(4).type == events::parser::BaseType::float_t);
        assert(parsed_event->argumentValue(4).value.val_float == e);

        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Arguments: -3 -9238 832223 -9277377287318721 3423423.25");
        assert(parsed_event->description() == "test\n<{}PARAM#www.test.com<link=www.test.com>#");

        int8_t a2;
        int16_t b2;
        int32_t c2;
        int64_t d2;
        float e2;
        events::test::decode_test2(event, a2, b2, c2, d2, e2);
        assert(a == a2 && b == b2 && c == c2 && d == d2 && e == e2);
    }

    {
        uint8_t a = 42;
        float b = 321.3134554f;
        uint16_t c = 9182;
        uint32_t d = 663281;
        uint64_t e = 2834873473267162;
        events::EventType event = events::test::create_test3(events::Log::Info, a, b, c, d, e);

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 2307042 | (1 << 24));
        assert(parsed_event->name() == "test3");
        assert(parsed_event->group() == "default");

        assert(parsed_event->numArguments() == 5);
        assert(parsed_event->argument(0).type == events::parser::BaseType::uint8_t);
        assert(parsed_event->argumentValue(0).value.val_uint8_t == a);
        assert(parsed_event->argument(1).type == events::parser::BaseType::float_t);
        assert(parsed_event->argumentValue(1).value.val_float == b);
        assert(parsed_event->argument(2).type == events::parser::BaseType::uint16_t);
        assert(parsed_event->argumentValue(2).value.val_uint16_t == c);
        assert(parsed_event->argument(3).type == events::parser::BaseType::uint32_t);
        assert(parsed_event->argumentValue(3).value.val_uint32_t == d);
        assert(parsed_event->argument(4).type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argumentValue(4).value.val_uint64_t == e);

        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Arguments: 42 321.3 9182 663281 2834873473267162");
        assert(parsed_event->description() == "#url<link=www.test.com/a/b/c?x=1>#\nkeep this. P");

        uint8_t a2;
        float b2;
        uint16_t c2;
        uint32_t d2;
        uint64_t e2;
        events::test::decode_test3(event, a2, b2, c2, d2, e2);
        assert(a == a2 && b == b2 && c == c2 && d == d2 && e == e2);
    }

    {
        events::test2::enums::enum_bitfield_t a =
            events::test2::enums::enum_bitfield_t::bit2 | events::test2::enums::enum_bitfield_t::bit3;
        events::EventType event = events::test::create_test4(events::Log::Info, a);

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 2307043 | (1 << 24));
        assert(parsed_event->name() == "test4");

        assert(parsed_event->numArguments() == 1);
        assert(parsed_event->argument(0).type == events::parser::BaseType::uint8_t);
        assert(parsed_event->argument(0).isEnum());
        assert(parsed_event->argument(0).enum_def->name == "enum_bitfield_t");
        assert(parsed_event->argument(0).enum_def->event_namespace == "test2");
        assert(parsed_event->argument(0).enum_def->is_bitfield);
        assert(parsed_event->argument(0).enum_def->type == events::parser::BaseType::uint8_t);
        assert(parsed_event->argumentValue(0).value.val_uint8_t == (uint8_t)a);

        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Bitfield value: Bit 2/ Bit 3");
        assert(parsed_event->description() == "");

        assert(a & events::test2::enums::enum_bitfield_t::bit2);
    }

    {
        float a = 16.423f;
        float b = 9472.3245894327f;
        int8_t c = -54;
        uint64_t d = 4613686018427387904;
        events::EventType event = events::test::create_test5(events::Log::Info, a, b, c, d);

        // customize unit formatters
        p.formatters().float_value_with_unit = [](float value, int num_decimal_digits, const std::string& unit) {
            std::ostringstream argument_stream;
            if (unit == "m_v") {  // vertical distance
                argument_stream << std::fixed << std::setprecision(num_decimal_digits + 3) << (value / 1000.f)
                                << " kilometers";
            } else if (num_decimal_digits >= 0) {
                argument_stream << std::fixed << std::setprecision(num_decimal_digits) << value << " " << unit;
            } else {
                argument_stream << value << " " << unit;
            }
            return argument_stream.str();
        };
        p.formatters().int_value_with_unit = [](int64_t value, const std::string& unit) {
            std::ostringstream argument_stream;
            if (unit == "m") {
                argument_stream << value << " meters";
            } else if (unit == "m/s") {
                argument_stream << value << " meters per second";
            } else {
                argument_stream << value << " " << unit;
            }
            return argument_stream.str();
        };

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 2307044 | (1 << 24));
        assert(parsed_event->name() == "test5");
        assert(parsed_event->group() == "default");

        assert(parsed_event->numArguments() == 4);
        assert(parsed_event->argument(0).type == events::parser::BaseType::float_t);
        assert(parsed_event->argumentValue(0).value.val_float == a);
        assert(parsed_event->argument(1).type == events::parser::BaseType::float_t);
        assert(parsed_event->argumentValue(1).value.val_float == b);
        assert(parsed_event->argument(2).type == events::parser::BaseType::int8_t);
        assert(parsed_event->argumentValue(2).value.val_int8_t == c);
        assert(parsed_event->argument(3).type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argumentValue(3).value.val_uint64_t == d);

        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Testing units: 16.423 m");
        assert(parsed_event->description() == R"STR(0.016423 kilometers 16.423 C
9472.324219 m^2
-54 meters
4613686018427387904 meters per second)STR");

        p.formatters().float_value_with_unit = nullptr;
        p.formatters().int_value_with_unit = nullptr;
    }

    {
        events::test2::enums::enum_t a = events::test2::enums::enum_t::one;
        events::test2::enums::enum_t b = events::test2::enums::enum_t::large;
        events::EventType event =
            events::test::create_test_enum({events::Log::Info, events::LogInternal::Warning}, a, b);

        std::unique_ptr<events::parser::ParsedEvent> parsed_event = p.parse(event);
        assert(parsed_event);
        assert(parsed_event->id() == event.id);
        assert(event.id == 1531103 | (1 << 24));
        assert(parsed_event->name() == "test_enum");
        assert(parsed_event->group() == "custom_group");

        assert(parsed_event->numArguments() == 2);
        assert(parsed_event->argument(0).type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argument(0).isEnum());
        assert(parsed_event->argument(0).enum_def->name == "enum_t");
        assert(parsed_event->argument(0).enum_def->event_namespace == "test2");
        assert(parsed_event->argument(0).enum_def->type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argumentValue(0).value.val_uint64_t == (uint64_t)a);

        assert(parsed_event->argument(1).type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argument(1).isEnum());
        assert(parsed_event->argument(1).enum_def->name == "enum_t");
        assert(parsed_event->argument(1).enum_def->event_namespace == "test2");
        assert(parsed_event->argument(1).enum_def->type == events::parser::BaseType::uint64_t);
        assert(parsed_event->argumentValue(1).value.val_uint64_t == (uint64_t)b);

        printf("message=%s\n", parsed_event->message().c_str());
        printf("description=%s\n", parsed_event->description().c_str());
        assert(parsed_event->message() == "Event using enums. Enum values: One value, Large value");
        assert(parsed_event->description() == "");

        events::test2::enums::enum_t a2;
        events::test2::enums::enum_t b2;
        events::test::decode_test_enum(event, a2, b2);
        assert(a == a2 && b == b2);
    }

    printf("Success!\n");
    return 0;
}
