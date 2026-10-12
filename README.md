# Эмулятор командной оболочки UNIX

Учебный проект: C++17, Qt 6 Widgets, CMake, Git. GUI — `shell-emulator`, ядро не зависит от Qt. Все операции над VFS происходят в RAM. Программа не запускает команды ОС и не обращается к реальной файловой системе при `ls`, `cd`, `cat`.

## Сборка в CLion (Windows)

1. Установите Qt 6 с комплектом Desktop MinGW 64-bit либо MSVC 64-bit. Комплект должен соответствовать компилятору в профиле CLion.
2. Откройте каталог с `CMakeLists.txt` как CMake-проект.
3. Settings → Build, Execution, Deployment → Toolchains: выберите соответствующий компилятор. В разделе CMake → CMake options укажите, например, `-DCMAKE_PREFIX_PATH=C:/Qt/6.8.3/mingw_64`. Замените пример на фактический путь установленного Qt.
4. Reload CMake Project. Выберите цель `shell-emulator` и нажмите Run. Если Windows не видит Qt DLL, добавьте каталог Qt `bin` в PATH конфигурации запуска (или используйте windeployqt для развёртывания).
5. В Run → Edit Configurations для `shell-emulator` задайте Working directory — корень проекта, Program arguments — например `--vfs data/deep.csv --script scripts/stage3-start.txt`.

Из терминала (если компилятор и Qt настроены):

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.3/mingw_64"
cmake --build build
.\build\shell-emulator.exe --vfs data/deep.csv --script scripts/stage3-start.txt
```

При multi-config генераторе исполняемый файл может находиться в `build/Debug` или `build/Release`. Для Linux/macOS используйте путь к своему Qt и `./build/shell-emulator`.

## Архитектура

- `main.cpp`: QApplication, параметры запуска, обработка ошибок загрузки.
- `MainWindow`: QWidget, QPlainTextEdit (вывод), QLineEdit (ввод), QLabel (приглашение), сигнал returnPressed. QTimer запускает стартовый скрипт после открытия окна.
- `Parser`: токенизация с одинарными/двойными кавычками, экранированием обратной косой чертой и пустыми аргументами. Не реализует подстановки, пайплайны, перенаправления и другие возможности полноценной POSIX shell.
- `Shell`: диспетчеризация команд, Result {output, ok, exit}, текущий каталог.
- `Config`: --vfs, --script, --help; параметры могут записываться через пробел или `=`. conf-dump печатает ключи vfs/script, включая значения по умолчанию.
- `ScriptRunner`: построчное чтение; пропускает пустые строки и строки-комментарии с #; передаёт команды тому же обработчику, что и интерактивный ввод. Прерывается на первой ошибке или exit. При ошибке окно остаётся открытым.
- `VFS`: `map<string, Node>`, где ключ — абсолютный виртуальный путь, значение — признак каталога и байты файла. При загрузке CSV проверяются корень, родители, уникальность, тип и Base64. CSV читается, но не изменяется. Родительские каталоги могут располагаться в CSV после дочерних элементов.

## CSV и Base64

```csv
path,type,content
/,dir,
/home,dir,
/home/user,dir,
/home/user/hello.txt,file,SGVsbG8=
```

`dir` — каталог (content пустой); `file` — файл (content в стандартной Base64, включая текстовые файлы). `SGVsbG8=` — строка Hello. Для фактических байтов используется `std::string`, сохраняющий и нулевые байты. CSV содержит пути с `/`, корень обязателен, пути абсолютные и не содержат `.` или `..` как отдельные компоненты. Поля с запятыми заключаются в двойные кавычки. Вложенность определяется путём, а не распаковкой в физический каталог. `data/minimal.csv` — только корень; `data/files.csv` — текстовые и бинарный файлы; `data/deep.csv` — более трёх уровней вложенности. Без `--vfs` создаётся демонстрационное дерево в RAM.

## Команды (этап 3)

| Команда | Назначение |
| --- | --- |
| `ls` | Список непосредственных детей текущего каталога |
| `ls /home` | Список по заданному пути |
| `ls -a` | Включить скрытые файлы и . / .. |
| `ls -- path` | Отключить разбор опций |
| `ls file.txt` | Вывести имя файла |
| `cd /home/user` | Перейти по абсолютному пути |
| `cd ..`, `cd .`, `cd name` | Относительные пути |
| `cd` | Вернуться в корень (упрощённый аналог home) |
| `pwd` | Текущий виртуальный путь |
| `cat path` | Содержимое файла (GUI отображает как UTF-8, произвольные бинарные байты могут не отображаться корректно) |
| `conf-dump` | Параметры запуска |
| `help` | Список команд |
| `exit` | Закрыть окно |

## Демонстрация и Git

Проект содержит три реальных коммита. Проверка истории: `git log --oneline`. Для просмотра этапов используйте `git switch --detach <хеш>` и для возврата `git switch main` (если ветка называется main; текущую ветку можно узнать из `git branch`). Пересобирайте программу при переключении этапа.

### Этап 1

На первом коммите в GUI по очереди введите:

```text
ls
ls /home "two words" 'three words' ""
cd "folder name"
ls "unfinished
unknown
exit extra
exit
```

`ls` и `cd` печатают имя и каждый аргумент. Незакрытая кавычка и неизвестная команда дают сообщение об ошибке. `exit` закрывает приложение. Сделайте собственные скриншоты GUI для защиты.

### Этап 2

Второй коммит добавляет конфигурацию и сценарии. Запуск из PowerShell:

```powershell
.\scripts\stage2.ps1 -Exe ".\build\shell-emulator.exe"
.\scripts\stage2-config.ps1 -Exe ".\build\shell-emulator.exe"
```

Для Linux/macOS: `sh scripts/stage2.sh ./build/shell-emulator`. Закрывайте каждое окно, чтобы продолжить. Проверяются отсутствие параметров, каждый параметр по отдельности, совместное использование, ошибочные параметры. Стартовый скрипт демонстрирует echo команд, conf-dump, заглушки и остановку на неизвестной команде.

### Этап 3

```powershell
.\scripts\stage3.ps1 -Exe ".\build\shell-emulator.exe"
.\scripts\errors.ps1 -Exe ".\build\shell-emulator.exe"
```

Для Linux/macOS: `sh scripts/stage3.sh ./build/shell-emulator` и `sh scripts/errors.sh ./build/shell-emulator`.

`stage3-start.txt` содержит все основные успешные режимы и завершает работу сценария на ошибке, следующая команда не выполняется. `stage3-exit.txt` проверяет выход отдельно: из-за остановки при первой ошибке нельзя показать exit после ошибки в одном запуске. `scripts/errors/` содержит 14 отдельных сценариев ошибок. При необходимости проверьте отсутствующий стартовый скрипт: `shell-emulator --script missing.txt`.

## Автоматическая проверка без Qt

```sh
cmake -S . -B build-core -DBUILD_GUI=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

Для multi-config добавьте `--config Debug` к сборке и `-C Debug` к ctest. Цель `shell-demo` — только тестовый консольный стенд для запуска того же ядра без GUI, не замена приложению `shell-emulator`. Пример:

```sh
./build-core/shell-demo --vfs data/deep.csv --script scripts/stage3-start.txt
```

Файл `demo-transcript.txt` — фактический вывод тестового стенда. Все три этапа были проверены автоматическими тестами ядра в Linux. Qt в среде подготовки архива не установлен, поэтому компиляцию и запуск самого GUI, а также выполнение PowerShell-сценариев следует проверить на своей ОС. Реальные скриншоты GUI не приложены.
