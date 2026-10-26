#pragma once

#define NORM

#include <string>

struct Norm {
    static std::string C(const std::string& s) {
        std::string out;
        out.reserve(s.size());
        for(char c : s) {
            if(c >= 'A' && c <= 'Z') out.push_back(char(c - 'A' + 'a'));
            else out.push_back(c);
        }
        return out;
    }
};