/******************************************************
 * NanoPi M1 DHT11 温湿度获取
 * author: bluebanboom
 * email:  bluebanboom@gmail.com
 * 2016.07.23
 * 编译命令：gcc dht11.c gpio.c -std=gnu99 -o dht11
 ******************************************************/
#include <stdio.h>
#include <unistd.h>
#include "gpio.h"


#define TIMEOUT 10000

int DHT = 11;

int wait_data(int val) {
    int count = 0;
    while (digitalRead(DHT) == val) {
        count ++;
        if (count == TIMEOUT) {
            return 0;
        }
    }
    return 1;
}

unsigned char read_byte() {
    unsigned char data = 0;
    for (int i =7 ; i >= 0; i--) {
        if (wait_data(0) == 0) {
            printf("timeout3\n");
            return 0;
        }
        delayMicroseconds(40);
        int d = digitalRead(DHT);
        if (d == 1) {
            data = data | (1 << i);
            if (wait_data(1) == 0) {
                printf("timeout4\n");
                return 0;
            }
        }
    }
    return data;
}

void dht11_read() {
    pinMode(DHT, OUTPUT);
    pullUpDnControl(DHT, PUD_UP);
    delay(1 * 1000);

    digitalWrite(DHT, LOW);
    delay(20);
    digitalWrite(DHT, HIGH);
    delayMicroseconds(50);

    pinMode(DHT, INPUT);
    delayMicroseconds(30);
    unsigned char a, b, c, d = 0;
    unsigned char hash = 0;

    if (wait_data(0) == 0) {
        printf("timeout1\n");
        return;
    }
    if (wait_data(1) == 0) {
        printf("timeout2\n");
        return;
    }
    a = read_byte();
    b = read_byte();
    c = read_byte();
    d = read_byte();
    hash = read_byte();
    if (hash == (a + b + c + d)) {
        printf("{\"status\": 1, \"data\": {\"humidity\" : %d, \"temperature\" : %d}}\n", (int)a, (int)c);
    }
    else {
        printf("{\"status\": 0, \"error\": \"hash error.\"}\n");
    }
}

int main(int argc, const char * argv[]) {
    if (argc == 2) {
        DHT = atoi(argv[1]);
    }

    if (setup() == 0) {
        pinMode(DHT, INPUT);

        dht11_read();

        cleanup();
    }
    else {
        printf("{\"status\": 0, \"error\": \"%s\"}\n", "init gpio failed.");
    }
    return 0;
}
