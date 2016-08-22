# encoding:utf-8
import sqlite3
import sys
import os

class DB:
    def __init__(self):
        self.conn = sqlite3.connect('data.db', check_same_thread = False)
        self.cursor = self.conn.cursor()

    def close(self):
        self.conn.close()

    def commit(self):
        self.conn.commit()

    def execute(self, sql):
        self.cursor.execute(sql)

    def fetchone(self):
        return self.cursor.fetchone()

    def makeData(self):
        data = []
        while True:
            onedata = self.fetchone()
            if onedata == None or len(onedata) == 0:
                break
            data.append(onedata)
        return data

    def fetchWithSQL(self, sql):
        self.execute(sql)
        return self.makeData()
        
    def insertData(self, temperature, humidity):
        sql = "insert into temperature (temperature, humidity) values (%d, %d)" % (temperature, humidity)
        self.execute(sql)
        self.commit()
    
    def getDay(self, day):
        sql = "select * from temperature where time between '%s 00:00:00' and '%s 23:59:59'" % (day, day)
        return self.fetchWithSQL(sql)


def main():
    db = DB()
    print db.getDay('2016-08-10')
    # db.insertData(28, 49)
    db.close()

if __name__ == '__main__':
    main()