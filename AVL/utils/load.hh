#pragma once
#include <cmath>
#include <iostream>

double string_to_double(std::string __str) {
    double value;

    size_t pos = __str.find('e');
    if (pos != std::string::npos) {

        std::string mantissa = __str.substr(0, pos);
        std::string exponent = __str.substr(pos + 1);

        double mantissa_value = std::stod(mantissa);
        int exponent_value = std::stoi(exponent);

        value = mantissa_value * std::pow(10, exponent_value);
    }
    else value = std::stod(__str);

    return value;
}
