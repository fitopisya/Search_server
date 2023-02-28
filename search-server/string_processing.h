#pragma once
#include <vector>
#include <string>
#include <set>

#include "document.h"

std::vector<std::string> SplitIntoWords(const std::string&);

template <typename String>
std::set<std::string> MakeUniqueNonEmptyStrings(const String& string) {
    std::set<std::string> non_empty_strings;
    for (const std::string& str : string) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    } return non_empty_strings;
}
