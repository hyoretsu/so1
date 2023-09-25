#include <cmath>
#include <string>

double precisionRound(double number, int precision, std::string direction) {
    int offset = pow(10, precision);

    if (direction == "up") {
        return ceil(number * offset) / offset;
    } else if (direction == "down") {
        return floor(number * offset) / offset;
    }

    return round(number * offset) / offset;
}
