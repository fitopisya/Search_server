# Search server

# Описание программы
Ядро поисковая система. База данных документов, по которым будет происходить поиск наиболее релевантных, исходя из запроса пользователя.

# Использование      

### Перед тем как начать:
  1. Установка и настройкка всех требуемых компонентов в среде разработки для запуска приложения
  2. Вариант использования показан в main.cpp

## Описание возможностей:

### Ядром поисковой системы является класс:  SearchServer
 Метдоы класса SearchServer:
 
- Конструктор принимает строку- Стоп-слова например: `"in at and"s` <br>
  Стоп-слова в запросе не учитываются при поиске.
-  Добавление документов в поисковую систему.
  `void AddDocument(int document_id, string_view document,DocumentStatus status, const vector<int> &ratings);`  
  document - строка вида: `"funny pet and nasty -rat"s`<br>
  где *"funny pet nasty"* - слова по которым будет идти поиcк<br>
  *"and"* - стоп слово, указанное в конструкторе SearchServer<br>
  *"-rat"* - миус слово<br>
  Минус-слова исключают из результатов поиска документы, содержащие такие слова.<br>
  Возможный DocumentStatus: `ACTUAL, IRRELEVANT, BANNED, REMOVED` <br>
  ratings - Каждый документ на входе имеет набор оценок пользователей. <br>
  Первая цифра — это количество оценок<br>
  например:*{4 5 -12 2 1}*;<br>
- Поиск document в поисковом сервере и ранжирование по TF-IDF<br>
  Есть 6 способов вызова функции 3 однопоточные и 3 многопоточнные ExecutionPolicy  
  `FindTopDocuments (ExecutionPolicy,query)`  
  `FindTopDocuments (ExecutionPolicy,query,DocumentStatus)`  
  `FindTopDocuments (ExecutionPolicy,query,DocumentPredicate)`  
  `FindTopDocuments (query)`  
  `FindTopDocuments (query,DocumentStatus)`  
  `FindTopDocuments (query,DocumentPredicate)`<br>
  Метод возвращает vector<Document> подходящих по *query*<br>
  Полезность слов оценивают понятием inverse document frequency или IDF. <br>
  Эта величина — свойство слова, а не документа. <br>
  Чем в большем количестве документов есть слово, тем ниже IDF.<br>
  Выше располагать документы, где искомое слово встречается более одного раза. <br>
  Здесь нужно рассчитать term frequency или TF.<br>
  Для конкретного слова и конкретного документа это доля, которую данное слово занимает среди всех.<br>
 - `GetDocumentCount()` - возвращает количество документов в поисковом сервере<br>
 - `begin и end` - Они вернут итераторы. Итератор даст доступ к id всех документов, хранящихся в поисковом сервере.<br>
- `tuple<std::vector<std::string_view>, DocumentStatus> MatchDocument(raw_query, document_id)`<br>
  Возвращает:Первый объект — это вектор слов запроса, которые нашлись в документе document_id, <br>
  а второй объект — статус документа<br>
- Метод получения частот слов по id документа:<br>
 `map<string, double> GetWordFrequencies(document_id)`<br>
- Удаление документа из поискового сервера <br>
  `RemoveDocument(document_id)`  
  `RemoveDocument(ExecutionPolicy,document_id)`  
  `RemoveDocument(ExecutionPolicy, document_id)`
  
### Дополнительный фуннкционал:
- Вывод в поток станндартного ввода/вывода информацию о докукменте<br>
  `PrintDocument(document)`<br>
  `PrintMatchDocumentResult(document_id,vector<std::string_view> &words, DocumentStatus)`
- Класс `LogDuration` - позволяет проводить профилирование
- `Paginate()` - Позволяет разбивать результаты на страницы
- `RemoveDuplicates(SearchServer)` - позволяет избавиться от дублирующихся документов<br>
- А так же другие функции, обеспечивающие обработку входных данных.

# Требования:
1. -std=c++17
2. g++ (MinG w64) 13.2.0

# Стек технологий:
  1. Профилирование
  2. Оценка сложности программы
  3. MapReduce — концепция, при которой алгоритм разбивается на две стадии:
	независимая фильтрация и преобразование отдельных элементов (map или transform);
	группировка результатов предыдущей стадии (reduce).

# Описание проведенных тестов:
## Проблема большого количества запросов
Когда запросов приходит слишком много, их обработка занимает время. Запросы, ожидающие обработки, могут просто «‎посидеть»‎ в очереди и подождать, пока сервис-обработчик доберётся до них.
Для хранения только нужных запросов. Например, вы хотите знать, какие запросы пользователи отправляли на поисковый сервер. Но важны только актуальные запросы за последние сутки. То есть вам интересно время отправки. Хранить запросы старше суток не требуется.
Каждую минуту приходит один запрос от пользователя. То есть максимальное количество запросов, которые надо хранить, — это количество минут в сутках (1440). Появление 1441 запроса будет означать, что сутки прошли, первый запрос прошлых суток нам больше не интересен и может быть удалён. Для реализации такого механизма удобно использовать deque. Новый запрос легко вставится в конец, а устаревший запрос удалится из начала. 

## Функция поиска и удаления дубликатов: 
Поисковые системы сталкиваются с проблемой — зеркалами. Это копии сайта. Их количество в Интернете может достигать десятков или сотен. Чтобы первые страницы поисковой выдачи не состояли из копий одного и того же сайта, нужно разработать дедупликатор. Он удаляет копии из поискового сервера. Дубликатами считаются документы, у которых наборы встречающихся слов совпадают. Совпадение частот необязательно. Порядок слов неважен, а стоп-слова игнорируются. Функция использует только доступные к этому моменту методы поискового сервера.
При обнаружении дублирующихся документов функция должна удалить документ с большим id из поискового сервера, и при этом сообщить id удалённого документа в соответствии с форматом выходных данных, приведённым ниже.

## Многопоточнность 
Функция ProcessQueries распараллеливает обработку нескольких запросов к поисковой системе.
функция ProcessQueriesJoined, подобно функции ProcessQueries, распараллеливает обработку нескольких запросов к поисковой системе, но возвращает набор документов в плоском виде.
