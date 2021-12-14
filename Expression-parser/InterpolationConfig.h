#pragma once
#include <string>;
#include <optional>;
#include <array>;

using namespace std::literals::string_literals;

InterpolationConfig DEFAULT_INTERPOLATION_CONFIG{ "{{"s, "}}"s };

class InterpolationConfig {
public:
    std::string start;
    std::string end;
    static InterpolationConfig fromArray(std::optional<std::array<std::string, 2>> markers) {
        if (!markers.has_value()) {
            return DEFAULT_INTERPOLATION_CONFIG;
        }
        // TODO: implement
        // assertInterpolationSymbols('interpolation', markers);

        return InterpolationConfig(markers.value()[0], markers.value()[1]);
    }
    InterpolationConfig(std::string start, std::string end) : start{ start }, end{ end } {}
};