#pragma once

using namespace std;

#include "Fixed.h"
#include "Double.h"
#include "Float.h"

#define FIXED(N, K) Fixed<N, K>
#define FAST_FIXED(N, K) FastFixed<N, K>
#define DOUBLE Double
#define FLOAT Float

std::string prettify(const std::string& s) {
    if (s == "FLOAT") {
        return "(Float)";
    }
    if (s == "DOUBLE") {
        return "(Double)";
    }
    std::string f = "FIXED";
    if (s.substr(0, f.size()) == f) {
        int posOpen = s.find('(');
        int posClose = s.find(')', posOpen);
        int posComma = s.find(',', posOpen);
        return "(Fixed<int" + s.substr(posOpen + 1, posComma - posOpen - 1) + "_t, " + s.substr(posComma + 1, posClose - posComma - 1) + ">)";
    }
    f = "FAST_FIXED";
    if (s.substr(0, f.size()) == f) {
        int posOpen = s.find('(');
        int posClose = s.find(')', posOpen);
        int posComma = s.find(',', posOpen);

        int size = stoi(s.substr(posOpen + 1, posComma - posOpen - 1));
        size = size <= 8 ? 8 : size <= 16 ? 16 : size <= 32 ? 32 : 64;

        return "(FastFixed<int_fast" + to_string(size) + "_t, " + s.substr(posComma + 1, posClose - posComma - 1) + ">)";
    }

    throw std::runtime_error("Unknown type: " + s);
}