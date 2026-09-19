# Практическое занятие №1. Введение, основы работы в командной строке

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
