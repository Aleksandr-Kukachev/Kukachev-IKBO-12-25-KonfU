# Практическое занятие №1. Введение, основы работы в командной строке

А.П. Кукачев, РТУ МИРЭА

Научиться выполнять простые действия с файлами и каталогами в Linux из командной строки. Сравнить работу в командной строке Windows и Linux.

## Задача 1

Вывести отсортированный в алфавитном порядке список имен пользователей в файле passwd (вам понадобится grep).

```
labex:/etc/ $ cat passwd | sort | grep -o "^[^:]*"
_apt
avahi
backup
bin
colord
daemon
games
gnats
irc
labex
list
lp
mail
man
messagebus
mongodb
mysql
news
nobody
proxy
pulse
redis
root
rtkit
saned
sshd
sync
sys
systemd-network
systemd-resolve
systemd-timesync
tcpdump
usbmux
uucp
www-data
```

## Задача 2

Вывести данные /etc/protocols в отформатированном и отсортированном порядке для 5 наибольших портов, как показано в примере ниже:


```
labex:/etc/ $ cat /etc/protocols | sort -k2 -nr | head -5 | awk '{print $2, $1}'
142 rohc
141 wesp
140 shim6
139 hip
138 manet
```

## Задача 3

Написать программу banner средствами bash для вывода текстов, как в следующем примере (размер баннера должен меняться!):

```
[root@localhost ~]# ./banner "Hello from RTU MIREA!"
+-----------------------+
| Hello from RTU MIREA! |
+-----------------------+
```

Перед отправкой решения проверьте его в ShellCheck на предупреждения.

```
#!/bin/bash

var="$1"
var_len=${#var}

printf -v var2 '%*s' "$var_len" ''

var2=${var2// /-}

echo "+-${var2}-+" 
echo "| ${var} |"
echo "+-${var2}-+"
```

## Задача 4

Написать программу для вывода всех идентификаторов (по правилам C/C++ или Java) в файле (без повторений).

Пример для hello.c:

```
h hello include int main n printf return stdio void world
```

```
grep -oE '[a-zA-Z_][a-zA-Z0-9_]*' hello.c | sort -u | tr '\n' ' ' && echo ""

include int main printf return stdio 
```

## Задача 5

Написать программу для регистрации пользовательской команды (правильные права доступа и копирование в /usr/local/bin).

Например, пусть программа называется reg:

```
./reg banner
```

В результате для banner задаются правильные права доступа и сам banner копируется в /usr/local/bin.

```
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Использование: $0 <имя_команды>"
    exit 1
fi

file="$1"

if [ ! -f "$file" ]; then
    echo "Ошибка: файл '$file' не найден"
    exit 1
fi

chmod 755 "$file"

if sudo cp "$file" "/usr/local/bin/$(basename "$file")"; then
    echo "Команда '$(basename "$file")' успешно зарегистрирована"
else
    echo "Ошибка при копировании файла"
    exit 1
fi
```
## Задача 6

Написать программу для проверки наличия комментария в первой строке файлов с расширением c, js и py.

```
#!/bin/bash

for file in *.c *.js *.py; do

    if [ ! -f "$file" ]; then
        continue
    fi

    first_line=$(head -n 1 "$file")

    case "$file" in
        *.c|*.js)
            if [[ "$first_line" =~ ^[[:space:]]*(//|/\*) ]]; then
                echo "$file: комментарий есть"
            else
                echo "$file: комментария нет"
            fi
            ;;

        *.py)
            if [[ "$first_line" =~ ^[[:space:]]*# ]]; then
                echo "$file: комментарий есть"
            else
                echo "$file: комментария нет"
            fi
            ;;
    esac

done
```

## Задача 7

Написать программу для нахождения файлов-дубликатов (имеющих 1 или более копий содержимого) по заданному пути (и подкаталогам).

```
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Использование: $0 <путь>"
    exit 1
fi

dir="$1"

if [ ! -d "$dir" ]; then
    echo "Ошибка: каталог не существует"
    exit 1
fi

mapfile -d '' files < <(find "$dir" -type f -print0)

for ((i=0; i<${#files[@]}; i++)); do

    duplicates=()

    for ((j=i+1; j<${#files[@]}; j++)); do

        if cmp -s "${files[i]}" "${files[j]}"; then
            duplicates+=("${files[j]}")
        fi

    done

    if [ ${#duplicates[@]} -gt 0 ]; then
        echo "Дубликаты файла: ${files[i]}"

        for duplicate in "${duplicates[@]}"; do
            echo "  $duplicate"
        done

        echo
    fi

done
```

## Задача 8

Написать программу, которая находит все файлы в данном каталоге с расширением, указанным в качестве аргумента и архивирует все эти файлы в архив tar.

```
#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Использование: $0 <каталог> <расширение>"
    exit 1
fi

dir="$1"
ext="${2#.}"

if [ ! -d "$dir" ]; then
    echo "Ошибка: каталог не существует"
    exit 1
fi

if [ -z "$ext" ]; then
    echo "Ошибка: расширение не указано"
    exit 1
fi

archive="$PWD/archive.tar"

cd "$dir" || exit 1

files=()

while IFS= read -r -d '' file; do
    files+=("$file")
done < <(find . -maxdepth 1 -type f -name "*.$ext" -print0)

if [ ${#files[@]} -eq 0 ]; then
    echo "Файлы с расширением .$ext не найдены"
    exit 1
fi

if tar -cf "$archive" -- "${files[@]}"; then
    echo "Архив успешно создан: $archive"
else
    echo "Ошибка при создании архива"
    exit 1
fi
```

## Задача 9

Написать программу, которая заменяет в файле последовательности из 4 пробелов на символ табуляции. Входной и выходной файлы задаются аргументами.

```
#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Использование: $0 <входной_файл> <выходной_файл>"
    exit 1
fi

input="$1"
output="$2"

if [ ! -f "$input" ]; then
    echo "Ошибка: входной файл не существует"
    exit 1
fi

if [ "$input" -ef "$output" ]; then
    echo "Ошибка: входной и выходной файлы совпадают"
    exit 1
fi

sed 's/    /\t/g' "$input" > "$output"

if [ $? -eq 0 ]; then
    echo "Замена выполнена успешно"
else
    echo "Ошибка при обработке файла"
    exit 1
fi
```

## Задача 10

Написать программу, которая выводит названия всех пустых текстовых файлов в указанной директории. Директория передается в программу параметром. 

```
#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Использование: $0 <директория>"
    exit 1
fi

dir="$1"

if [ ! -d "$dir" ]; then
    echo "Ошибка: директория не существует"
    exit 1
fi

echo "Пустые текстовые файлы:"

find "$dir" -maxdepth 1 -type f -name "*.txt" -empty -printf "%f\n"
```
