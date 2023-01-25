// Тест проверяет, что поисковая система исключает стоп-слова при добавлении документов
void TestExcludeStopWordsFromAddedDocumentContent() {
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };
    // Сначала убеждаемся, что поиск слова, не входящего в список стоп-слов,
    // находит нужный документ
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("in"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == doc_id);
    }

    // Затем убеждаемся, что поиск этого же слова, входящего в список стоп-слов,
    // возвращает пустой результат
    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
    }
}

/*Adding documents*/
void TestAddDocument()
{
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };

    {   // we should be able to find document with any of these words
        // document needs to be ACTUAL
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat in the city"s);
        ASSERT(found_docs.size() == 1);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.id == doc_id);
    }
}

/* Stop words are removed from the search */
void TestStopWords()
{
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };

    {
        SearchServer server;
        server.SetStopWords("in the"s);
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("in"s).empty());
    }

}

/* Documents that have minus-words are removed from the search */
void TestMinusWords()
{
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 };

    SearchServer server;
    server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
    ASSERT(server.FindTopDocuments("-in"s).empty());

}

/* Should return all plus-words in query, minus-words queries are removed from the search */
void TestMatchedDocuments()
{
    {
        SearchServer server;
        server.AddDocument(0, "tall -man and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, -3 });
        const int document_count = server.GetDocumentCount();
        for (int document_id = 0; document_id < document_count; ++document_id)
        {
            const auto& [words, status] = server.MatchDocument("karl -man"s, document_id);
            ASSERT(words.size() == 0);
            ASSERT(document_id == 0);
            ASSERT(status == DocumentStatus::ACTUAL);
        }
    }
}

/* Sort with relevance > */
void TestSortFindedDocumentsByRelevance()
{
    {
        SearchServer server;

        server.AddDocument(0, "white man and stupid hat"s, DocumentStatus::ACTUAL, { 8, -3 });
        server.AddDocument(1, "tall man tall park"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
        server.AddDocument(2, "strange woman with hots"s, DocumentStatus::ACTUAL, { 5, -12, 2 });
        server.AddDocument(3, "strange old klark"s, DocumentStatus::BANNED, { 9 });

        const auto& documents = server.FindTopDocuments("tall strange man"s);

        int doc_size = static_cast<int>(documents.size());
        for (int i = 0; i < doc_size; ++i)
        {
            ASSERT(documents[i].relevance > documents[i + 1].relevance);
        }
    }
}

/* Run all tests */
void TestSearchServer() {
    TestExcludeStopWordsFromAddedDocumentContent();
    TestAddDocument();
    TestStopWords();
    TestMinusWords();
    TestMatchedDocuments();
    TestSortFindedDocumentsByRelevance();
}
