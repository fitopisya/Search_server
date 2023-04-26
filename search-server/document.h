#pragma once
#include <iostream>

/*
 * Possible errors: unused variables in enum class DocumentStatus
 */
struct Document {
    Document() = default;
    Document(int, double, int);

    int id = 0;
    double relevance = 0.0;
    int rating = 0;
};

std::ostream& operator<<(std::ostream& os, const Document& document);

enum class DocumentStatus {
    ACTUAL,
    IRRELEVANT,
    BANNED,
    REMOVED,
};
