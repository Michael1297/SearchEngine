#Документация 

<details><summary>Config.h</summary>

| Название   | Описание                                 |
|------------|------------------------------------------|
| driverSQL  | драйвер SQL сервера                      |
| serverSQL  | название сервера                         |
| portSQL  | порт сервера                             |
| databaseSQL  | название базы данных                     |
| loginSQL  | логин базы данных                        |
| passwordSQL  | пароль  базы данных                      |
| start_page  | страница с которой начинается индексация |
| host  | хост сервера                             |
| port  | порт сервера                             |
</details><br/>

<details><summary>Exception.h</summary>

| Название    | Описание                                       |
|-------------|------------------------------------------------|
| Exception(std::string text) | Обертка std::exception<br/> принимающая строку |
</details><br/>

<details><summary>GumboAPI.h</summary>

| Название    | Описание                                                        |
|-------------|-----------------------------------------------------------------|
| GumboAPI(std::string code) | Принимает код страницы                                          |
| get_links(std::function<void(std::string)> | Принимет лямбда выражение которое возвращает ссылки со страницы |
| get_fragments(std::function<void(std::string)> function | Принимет лямбда выражение которое возвращает фрагменты текста   |
| get_words(std::function<void(std::string)> function) | Принимет лямбда выражение которое возвращает слова              |
| std::string find_title() | Возвращает заголовок страницы                                   |
</details><br/>

<details><summary>HttpTool.h</summary>

| Название    | Описание                                                    |
|-------------|-------------------------------------------------------------|
| bool is_ownLink(std::string& link) | Принимат ссылку и возвращает принадлежность ссылки к домену |
| std::string getDomain() | Возвращает домен индексированного сайта                     |
| std::string getDomain(std::string link) | Возвращает домен из полученной ссылки                       |
| std::string getPath(std::string link) | Возвращает патч из полученной ссылки                        |
| escape(std::string& text, char symbol);| Экранирует символ в тексте                                  |
</details><br/>

<details><summary>SearchEngine.h</summary>

| Название    | Описание                                                                                  |
|-------------|-------------------------------------------------------------------------------------------|
| nlohmann::json startIndexing() | Запускает индексацию всех страниц домена                                                  |
| nlohmann::json startIndexing(std::string queurls)| Запускает индексацию списка страниц разделенных символом +                                |
| nlohmann::json stopIndexing() | Останавливает индексацию                                                                  |
| nlohmann::json status() | Возвращает количество строк в каждой таблицы в базе данных                                |
| nlohmann::json search(std::string query, int offset, int limit) | Принимает список слов разделенных символом + , значение оффсет и лимит результатов поиска |
</details><br/>

<details><summary>Server.hpp</summary>

| Название    | Описание                 |
|-------------|--------------------------|
| run_server(std::unique_ptr<SearchEngine>& searchEngine) | Функция запускает сервер |
</details><br/>

<details><summary>SQL_database.h</summary>

| Название                                                          | Описание                         |
|-------------------------------------------------------------------|----------------------------------|
| connection()                                                      | Подключает бд                    |
| create()                                                          | Создает таблицы в бд             |
| drop(std::string table)                                           | Удаляет таблицу  из бд           |
| insert_page(std::string path, int code, std::string content);     | Добавляет страницу в бд          |
| insert_word(std::string value)                                    | Добавляет слово в бд             |
| insert_search_index(int page_id, int word_id, float rank          | Добавляет поисковай индекс в бд  |
| erase_page(std::string path)                                      | Удаляет страницу из бд           |
| int page_id(std::string path)                                     | Возврашает id страницы           |
| int word_id(std::string value)                                    | Возврашает id слова              |
| update_word(std::string value)                                    | Увеличить значение frequency на 1 |
| int size(std::string table)                                       | Возвращает табмер таблицы        |
| float get_relevance(int page_id, std::string world);              | Возвращает релевантность слова   |
| nlohmann::json search(std::unordered_set\<std::string\>& worlds); | Осуществляет поиск слов в бд     |
</details><br/>

<details><summary>Stemming.h</summary>

| Название    | Описание                          |
|-------------|-----------------------------------|
| std::string WStringToString(const std::wstring& Wstring) | wstring преобразовывает в string  |
| std::wstring StringToWString(const std::string& string) | string преобразовывает в wstring  |
| eng_to_lower(std::wstring& word) | Английские буквы в нижний регистр |
| ru_to_lower(std::wstring& word) | Русские буквы в нижний регистр    |
| std::string word_stemming(const std::string& input_word) | Стимминг слова                    |
</details><br/>