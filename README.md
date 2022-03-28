# SearchEngine

Search engine on c++ (graduation work)

# API
* Поиск\
`/search`?query=`<word1+word2+word3>`&limit=`<value>`&offset=`<value>`
* Запуск индексации\
`/startIndexing`?queurls=`<site1+site2+site3>`
* Остановка индексации\
`/stopIndexing`
* Статус SQL базы данных\
`/status`

# Minimal required build settings:
* cmake 3.20
* mingw 9.0 64-bit
* C++ 17
* PostgreSQL
* PostgreSQL ODBC Driver

# Checklist:
- [x] Реализован метод /api/startIndexing.

- [x] Индексация страниц происходит в фоновом режиме.

- [x] Страницы при индексации обходятся рекурсивно.

- [x] Одна и та же страница не индексируется два раза.

- [x] HTML-теги не индексируются.

- [x] Слова при индексации приводятся к нормальной форме при помощи стемминга.

- [x] Для слов на странице рассчитывается их ранг по формуле.

- [x] Реализован метод /api/search.

- [x] Для каждой страницы в результате рассчитана её релевантность по формуле.

- [x] Поиск возвращает результаты в порядке их релевантности.

- [x] Метод поиска учитывает параметры limit и offset.

- [x] Метод поиска возвращает общее количество результатов

- [x] Найденные слова выделяются в сниппетах тегами \<b\>.

- [x] Параметры для подключения к базе данных хранятся в конфигурационном файле

- [x] Если в базе данных нет таблиц, они создаются автоматически при старте приложения.

- [x] Проект выложен на GitHub со всеми необходимыми файлами для сборки.

- [x] В README описана команда, которой необходимо запускать приложение после сборки.
