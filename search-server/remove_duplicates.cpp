#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) noexcept {
    std::set<std::vector<std::string>> words;
    std::vector<int> ids_to_delete;
    for (const int doc : search_server) {
        std::vector<std::string> vec_with_words;
        for (const auto& [key, val] : search_server.GetWordFrequencies(doc)) {
            vec_with_words.push_back(key);
        }
        if (words.find(vec_with_words) == words.end()) {
            words.insert(vec_with_words);
        }
        else {
            ids_to_delete.push_back(doc);
        }
    }
    for (const auto id : ids_to_delete) {
        std::cout << "Found duplicate document id " << id << "\n";
        search_server.RemoveDocument(id);
    }
}
