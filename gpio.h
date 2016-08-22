/*****************************************************
 * simple GPIO lib for bananapi M2+
 * author: bluebanboom
 * email:  bluebanboom@gmail.com
 * 2016.08
 *****************************************************/
 
#ifndef	__GPIO_H__
#define __GPIO_H__

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// Pin modes
#define	INPUT               0
#define	OUTPUT              1
//#define	PWM_OUTPUT          2
//#define	GPIO_CLOCK          3
//#define	SOFT_PWM_OUTPUT     4
//#define	SOFT_TONE_OUTPUT    5
//#define	PWM_TONE_OUTPUT     6

#define	LOW                 0
#define	HIGH                1

// Pull up/down/none
#define	PUD_OFF             0
#define	PUD_UP              1
#define	PUD_DOWN            2

struct Port {
    uint32_t Pn_CFG[4];
    uint32_t Pn_DAT;
    uint32_t Pn_DRV[2];
    uint32_t Pn_PUL[2];
};

int setup(void);

void cleanup(void);

int pinToPort (int pin);
int pinToShift(int pin);

extern void pinMode             (int pin, int mode) ;
extern void pullUpDnControl     (int pin, int pud) ;

extern int  digitalRead         (int pin) ;
extern void digitalWrite        (int pin, int value) ;

//extern void pwmWrite            (int pin, int value) ;
//extern int  analogRead          (int pin) ;
//extern void analogWrite         (int pin, int value) ;

extern void delay (unsigned int howLong);
extern void delayMicroseconds (unsigned int howLong);

#endif
