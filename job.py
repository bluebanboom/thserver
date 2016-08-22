# encoding:utf-8
#!/usr/bin/env python

import subprocess
import os
import db
import json
import random
import time


def dht11():
    process=subprocess.Popen('sudo ./dht11',stdout=subprocess.PIPE,shell=True)
    out = process.stdout.read()

    # {"status": 1, "data": {"humidity" : 44, "temperature" : 29}}
    data = json.loads(out)
    if data['status'] == 1:
        temperature = data['data']['temperature']
        humidity = data['data']['humidity']
        return True, temperature, humidity
    else:
        return False, 0, 0


def insert_data(t, h):
    db = db.DB()
    db.insertData(t, h)
    db.close()

def main():
    dht11()
    status = False
    temperature = 0
    humidity = 0
    for i in range(3):
        status, temperature, humidity = dht11()
        if status:
            insert_data(temperature, humidity)
            break
        else:
            time.sleep(10)

if __name__ == '__main__':
    main()
