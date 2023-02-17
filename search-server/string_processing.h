#pragma once
#include <vector>
#include <string>

#include "document.h"

std::vector<std::string> SplitIntoWords(const std::string&);
std::ostream& operator<<(std::ostream& out, const Document& document);
