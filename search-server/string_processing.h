#pragma once
#include <vector>
#include <string>
#include <set>

#include "document.h"

std::vector<std::string> SplitIntoWords(const std::string&);

template <typename StringContainer>
std::set<std::string, std::less<>> MakeUniqueNonEmptyStrings(const StringContainer& strs) {
    std::set<std::string, std::less<>> non_empty_strings;
    for (auto str : strs) {
        if (!str.empty()) {
            non_empty_strings.emplace(str);
        }
    }
    return non_empty_strings;
}

std::vector<std::string_view> SplitIntoWordsView(std::string_view str);


