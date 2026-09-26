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

