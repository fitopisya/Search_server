#include "remove_duplicates.h"
#include <algorithm>
void RemoveDuplicates(SearchServer& search_server) noexcept {
    std::set<std::set<std::string>> words;
    std::set<int> ids_to_delete;
    for (const int doc : search_server) {
        std::set<std::string> set_with_words;
        for (const auto& [key, val] : search_server.GetWordFrequencies(doc)) {
            set_with_words.insert(key);
        }
        if (!words.count(set_with_words)) {
            words.insert(set_with_words);
        }
        else {
            ids_to_delete.insert(doc);
        }
    }
    for (const auto id : ids_to_delete) {
        std::cout << "Found duplicate document id " << id << "\n";
        search_server.RemoveDocument(id);
    }
}