# encoding:utf-8
#!/usr/bin/env python

import sqlite3

print '\033[1;32m --===[ Create Database ]===-- \033[0m'
print '\033[1;32m [1]\033[0m Making data.db'

conn = sqlite3.connect('data.db', check_same_thread = False)
cursor = conn.cursor();

print '\033[1;32m [2]\033[0m Create table'
try:
    cursor.execute("create table temperature (id integer primary key autoincrement, temperature integer, humidity integer, time timestamp not null default(datetime('now', 'localtime')));")
except Exception as e:
    print '\t\033[1;31m[ERR]\033[0m', e

conn.commit()
conn.close()

print '\033[1;32m [3]\033[0m Done'
