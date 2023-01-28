// 1 - rating
void TestComputeAverageRating(){
    const int doc_id = 42;
    const string content = "cat in the city"s;
    const vector<int> ratings = { 1, 2, 3 }; // average = 2
    {
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        const Document& doc0 = found_docs[0];
        ASSERT(doc0.rating == 2);
    }
}
// 2 - relevance
void TestRelevance() {
        const vector<int> ratings = {1, 2, 3};
        const double e = 1e-6; // EPSILON
        double standard = 0.0;
        SearchServer server;
        server.AddDocument(0, "cat in the village"s, DocumentStatus::ACTUAL, ratings);
        const auto found_docs = server.FindTopDocuments("cat"s);
        const Document& doc0 = found_docs[0];
        ASSERT(abs(doc0.relevance - standard) < e); //idf = 0 tf = 0.25
        server.AddDocument(1, "cat in the city"s, DocumentStatus::ACTUAL, ratings);
        server.AddDocument(2, "dog in the car"s, DocumentStatus::ACTUAL, ratings);
        const auto found_docs1 = server.FindTopDocuments("cat village"s);
        const Document& doc1 = found_docs1[0];
        standard = 0.376019;
        ASSERT(abs(doc1.relevance - standard) < e);
}
// 3 - status filter
void TestStatusFilter() {
    SearchServer server;
    server.AddDocument(0, "текст белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "текст пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
    server.AddDocument(2, "текст ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
    server.AddDocument(3, "текст ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});

    const auto found_docs = server.FindTopDocuments("текст"s, DocumentStatus::BANNED);
    ASSERT(found_docs.size() == 1);
    const Document& doc0 = found_docs[0];
    ASSERT(doc0.id == 3);
}
// 4 - predicat filter
void TestPredicate(){
    SearchServer server;
    server.AddDocument(0, "белый кот и модный ошейник"s,        DocumentStatus::ACTUAL, {8, -3});
    server.AddDocument(1, "пушистый кот пушистый хвост"s,       DocumentStatus::ACTUAL, {7, 2, 7});
    server.AddDocument(2, "ухоженный пёс выразительные глаза"s, DocumentStatus::ACTUAL, {5, -12, 2, 1});
    server.AddDocument(3, "ухоженный скворец евгений"s,         DocumentStatus::BANNED, {9});

    for(const Document& document : server.FindTopDocuments("пушистый ухоженный кот"s, [](int document_id, DocumentStatus status[[maybe_unused]], int rating[[maybe_unused]]) { return document_id % 2 == 0; }))
    ASSERT(document.id % 2 == 0);
}
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
    {
        const int doc_id = 42;
        const string content = "cat in the city"s;
        const vector<int> ratings = { 1, 2, 3 };
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(!server.FindTopDocuments("in"s).empty());
    }
    {
        const int doc_id = 42;
        const string content = "cat -in the city"s;
        const vector<int> ratings = { 1, 2, 3 };
        SearchServer server;
        server.AddDocument(doc_id, content, DocumentStatus::ACTUAL, ratings);
        ASSERT(server.FindTopDocuments("-in"s).empty());
    }
}

/* Should return all plus-words in query, minus-words queries are removed from the search */
void TestMatchedDocuments()
{
    {
        SearchServer server;
        server.AddDocument(0, "tall -man and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, -3 });
        const vector<string> expected_words = {};
        const auto& [words, status] = server.MatchDocument("karl -man"s, 0);
        ASSERT(words.size() == 0);
        ASSERT(status == DocumentStatus::ACTUAL);
        ASSERT(words == expected_words);
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
        if(doc_size != 0){
            for (int i = 0; i < static_cast<uint16_t>(doc_size)-1; ++i){
                ASSERT(documents[i].relevance > documents[i+1].relevance);
            }
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
    TestPredicate(); // 4
    TestRelevance(); // 2
    TestStatusFilter(); // 3
    TestComputeAverageRating(); // 1
}