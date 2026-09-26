#include <iostream>
#include <string>

bool isTokenChar(char c) {
    return (c >= '0' && c <= '9') || c == '.' || c == ':';
}

bool readNumber(const std::string& str, std::size_t& pos,
                std::size_t end, int maxDigits, int maxValue,
                int& value) {
    std::size_t start = pos;
    value = 0;

    while (pos < end && str[pos] >= '0' && str[pos] <= '9') {
        if (pos - start >= static_cast<std::size_t>(maxDigits)) {
            return false;
        }
        value = value * 10 + (str[pos] - '0');
        ++pos;
    }

    return pos > start &&
           (pos - start == 1 || str[start] != '0') &&
           value <= maxValue;
}

bool extractIPv4(const std::string& str, unsigned long& outAddress,
                 int& outPort) {
    outAddress = 0;
    outPort = -1;

    std::size_t start = 0;
    while (start < str.size()) {
        if (!isTokenChar(str[start])) {
            ++start;
            continue;
        }

        // Validate the entire uninterrupted run of digits, periods, and colons.
        std::size_t end = start;
        while (end < str.size() && isTokenChar(str[end])) {
            ++end;
        }

        std::size_t pos = start;
        unsigned long address = 0;
        int port = -1;
        bool valid = true;

        for (int i = 0; i < 4 && valid; ++i) {
            int octet;
            if (!readNumber(str, pos, end, 3, 255, octet)) {
                valid = false;
                break;
            }

            address = (address << 8) | static_cast<unsigned long>(octet);

            if (i < 3) {
                if (pos == end || str[pos] != '.') {
                    valid = false;
                } else {
                    ++pos;
                }
            }
        }

        if (valid && pos < end && str[pos] == ':') {
            ++pos;
            if (!readNumber(str, pos, end, 5, 65535, port)) {
                valid = false;
            }
        }

        if (valid && pos == end) {
            outAddress = address;
            outPort = port;
            return true;
        }

        start = end;  // Never search inside a failed candidate token.
    }

    return false;
}

int main() {
    std::string line;

    while (true) {
        std::cout << "Enter text: ";
        if (!std::getline(std::cin, line) || line == "END") {
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(line, address, port)) {
            std::cout << "Extracted IPv4 address: "
                      << ((address >> 24) & 255UL) << '.'
                      << ((address >> 16) & 255UL) << '.'
                      << ((address >> 8) & 255UL) << '.'
                      << (address & 255UL)
                      << " (decimal value: " << address << ", port: ";

            if (port == -1) {
                std::cout << "none";
            } else {
                std::cout << port;
            }
            std::cout << ")\n";
        } else {
            std::cout << "No valid IPv4 address found.\n";
        }
    }

    std::cout << "Program terminated.\n";
}