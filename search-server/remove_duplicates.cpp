#include "remove_duplicates.h"

void RemoveDuplicates(SearchServer& search_server) {
    std::map<std::vector<std::string>, int> doc_words_with_id;
    std::vector<int> duplicate_ids;

    for (const int doc_id : search_server) {
        const auto& word_freq = search_server.GetWordFrequencies(doc_id);
        std::vector<std::string> words(word_freq.size());
        std::transform(word_freq.begin(), word_freq.end(),std::inserter(words, words.begin()),
                       [](const std::pair<std::string, double>& elements) {
                    return elements.first;
        });

        auto [word, no_match] = doc_words_with_id.emplace(words, doc_id);
        if (!no_match) {
            duplicate_ids.push_back(doc_id);
        }
    }
    for (const int doc_id : duplicate_ids) {
        std::cout << "Found duplicate document id "s << doc_id << "\n";
        search_server.RemoveDocument(doc_id);
    }
}
