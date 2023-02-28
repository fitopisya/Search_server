#pragma once

#include <deque>
#include "search_server.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server) :search_server_(search_server) {}

    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string&, DocumentPredicate);
    std::vector<Document> AddFindRequest(const std::string&, DocumentStatus);
    std::vector<Document> AddFindRequest(const std::string&);
    [[nodiscard]] int GetNoResultRequests() const;

private:
    struct QueryResult {
        std::vector<Document> request;
    };
    const SearchServer& search_server_;
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;

};

template <typename DocumentPredicate>
std::vector<Document>RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    if (requests_.size() >= min_in_day_) {
        requests_.pop_front();
    }
    requests_.push_back({ search_server_.FindTopDocuments(raw_query, document_predicate) });
    return requests_.back().request;
}