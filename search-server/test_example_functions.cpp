#include "test_example_functions.h"

void AddDocument(SearchServer& search_server, int document_id, const std::string& document, DocumentStatus status, const std::vector<int>& ratings) {
    try {
        search_server.AddDocument(document_id, document, status, ratings);
    }
    catch (const std::exception& err) {
        std::cout << "Error adding document "s << document_id << ": "s << err.what() << std::endl;
    }
}

void FindTopDocuments(const SearchServer& search_server, const std::string& raw_query) {
    std::cout << "--Top documents by query--"s << raw_query << std::endl;
    try {
        for (const Document& document : search_server.FindTopDocuments(raw_query)) {
            PrintDocument(document);
        }
    }
    catch (const std::exception& err) {
        std::cout << "Search error, there are no found documents"s << err.what() << std::endl;
    }
}

void MatchDocuments(const SearchServer& search_server, const std::string& query) {
    try {
        std::cout << "--Matched documents by query--"s << query << std::endl;
        for (const int document_id : search_server) {
            const auto [words, status] = search_server.MatchDocument(query, document_id);
            PrintMatchDocumentResult(document_id, words, status);
        }
    }
    catch (const std::exception& err) {
        std::cout << "Document match error when querying "s << query << ": "s << err.what() << std::endl;
    }
}

void PrintDocument(const Document& document) {
    std::cout << "{ "s
              << "document_id = "s << document.id << ", "s
              << "relevance = "s << document.relevance << ", "s
              << "rating = "s << document.rating << " }"s << std::endl;
}

void PrintMatchDocumentResult(int document_id, const std::vector<std::string_view>& words, DocumentStatus status) {
    std::cout << "{ "s
              << "document_id = "s << document_id << ", "s
              << "status = "s << static_cast<int>(status) << ", "s
              << "words ="s;
    for (const std::string_view& word : words) {
        std::cout << ' ' << word;
    }
    std::cout << "}"s << std::endl;
}
