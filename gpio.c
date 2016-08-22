/*****************************************************
 * simple GPIO lib for bananapi M2+
 * author: bluebanboom
 * email:  bluebanboom@gmail.com
 * 2016.08
 *****************************************************/

#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "gpio.h"
#include <time.h>
#include <sys/time.h>

//#define TEST 1

#define M1_GPIO_BASE 0x01C20800

static volatile void *gGpioMap;
static uint32_t gPageSize = 0;

static int gPinToPort[64] = {
//   A       C   D   E   F   G
//   0   1   2   3   4   5   6   7   8   9
    -1, -1, -1,  0, -1,  0, -1,  0,  0, -1,     // 0
     0,  0,  0,  0, -1,  0,  0, -1,  2,  2,     // 1
    -1,  2,  0,  2,  2, -1,  2,  0,  0,  0,     // 2
    -1,  0, -1,  0, -1,  0, -1,  0,  0, -1,     // 3
     0, -1, -1, -1, -1, -1, -1, -1, -1, -1,     // 4
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,     // 5
    -1, -1, -1, -1
};

static int gPinToShift[64] = {
//   0   1   2   3   4   5   6   7   8   9
    -1, -1, -1, 12, -1, 11, -1,  6, 13, -1,     // 0
    14,  1, 16,  0, -1,  3, 15, -1,  4,  0,     // 1
    -1,  1,  2,  2,  3, -1,  7, 19, 18,  7,     // 2
    -1,  8,  2,  9, -1, 10,  4, 16, 21, -1,     // 3
    20, -1, -1, -1, -1, -1, -1, -1, -1, -1,     // 4
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,     // 5
    -1, -1, -1, -1
};

int pinToPort(int pin)
{
    if (pin < 0 || pin >= 64) return -1;
    else return gPinToPort[pin];
}

int pinToShift(int pin) {
    if (pin < 0 || pin >= 64) return -1;
    else return gPinToShift[pin];
}

int setup(void)
{
#ifdef TEST
    gGpioMap = malloc(4 * 1024 * 1024);
    memset(gGpioMap, 0, 4 * 1024 * 1024);
#else
    void *gpioMem;
    uint32_t periBase;
    uint32_t gpioOffset;
    uint32_t pageMask;
    int      memFd;

    gPageSize = (uint32_t)sysconf(_SC_PAGESIZE);
    pageMask = (~(gPageSize-1));
    periBase = M1_GPIO_BASE & pageMask;
    gpioOffset = M1_GPIO_BASE & ~pageMask;

    memFd = open("/dev/mem", O_RDWR|O_SYNC);
    if(memFd < 0) {
       perror("Unable to open /dev/mem");
       return(-1);
    }

    gpioMem = mmap(0, gPageSize*2, PROT_READ|PROT_WRITE, MAP_SHARED, memFd, periBase);

    if(gpioMem == MAP_FAILED) {
       perror("Unable to mmap file");
       printf("pc:%lx\n", (unsigned long)periBase);
       return(-1);
    }

    gGpioMap = gpioMem + gpioOffset;
#endif
    return 0;
}

void cleanup(void)
{
#ifdef TEST
    free(gGpioMap);
#else
    munmap((void *)gGpioMap, gPageSize * 2);
#endif
}

void pinMode(int pin, int mode)
{
    int portn = pinToPort(pin);
    int shift = pinToShift(pin) ;
    if (portn < 0 || shift < 0) {
        return;
    }
    struct Port *port = NULL;
    port = (struct Port*)((struct Port *)gGpioMap + portn);
    int n = shift / 8;
    shift = shift % 8 * 4;
    uint32_t mask = ~(0xF << shift);
    uint32_t value = port->Pn_CFG[n] & mask;

    value |= mode << shift;
    port->Pn_CFG[n] = value;
}

void pullUpDnControl(int pin, int pud)
{
    int portn = pinToPort(pin);
    int shift = pinToShift(pin) ;
    if (portn < 0 || shift < 0) {
        return;
    }
    struct Port *port = NULL;
    port = (struct Port*)((struct Port *)gGpioMap + portn);
    int n = shift / 16;
    shift = shift % 16 * 2;

    uint32_t mask = ~(0x3 << shift);
    uint32_t value = port->Pn_PUL[n] & mask;

    port->Pn_PUL[n] = value | (pud << shift);
}

int digitalRead(int pin)
{
    int portn = pinToPort(pin);
    int shift = pinToShift(pin) ;
    if (portn < 0 || shift < 0) {
        return -1;
    }
    struct Port *port = NULL;
    port = (struct Port*)((struct Port *)gGpioMap + portn);

    uint32_t data = port->Pn_DAT;
    return (data & (1 << shift)) >> shift;
}

void digitalWrite(int pin, int value)
{
    int portn = pinToPort(pin);
    int shift = pinToShift(pin) ;
    if (portn < 0 || shift < 0) {
        return;
    }

    struct Port *port = NULL;
    port = (struct Port*)((struct Port *)gGpioMap + portn);

    if (value == 1) {
        port->Pn_DAT |= (1 << shift);
    }
    else {
        port->Pn_DAT &= ~(1 << shift);
    }
}


//////////
/*
 * delay:
 *	Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}


/*
 * delayMicroseconds:
 *	This is somewhat intersting. It seems that on the Pi, a single call
 *	to nanosleep takes some 80 to 130 microseconds anyway, so while
 *	obeying the standards (may take longer), it's not always what we
 *	want!
 *
 *	So what I'll do now is if the delay is less than 100uS we'll do it
 *	in a hard loop, watching a built-in counter on the ARM chip. This is
 *	somewhat sub-optimal in that it uses 100% CPU, something not an issue
 *	in a microcontroller, but under a multi-tasking, multi-user OS, it's
 *	wastefull, however we've no real choice )-:
 *
 *      Plan B: It seems all might not be well with that plan, so changing it
 *      to use gettimeofday () and poll on that instead...
 *********************************************************************************
 */

void delayMicrosecondsHard (unsigned int howLong)
{
  struct timeval tNow, tLong, tEnd ;

  gettimeofday (&tNow, NULL) ;
  tLong.tv_sec  = howLong / 1000000 ;
  tLong.tv_usec = howLong % 1000000 ;
  timeradd (&tNow, &tLong, &tEnd) ;

  while (timercmp (&tNow, &tEnd, <))
    gettimeofday (&tNow, NULL) ;
}

void delayMicroseconds (unsigned int howLong)
{
  struct timespec sleeper ;
  unsigned int uSecs = howLong % 1000000 ;
  unsigned int wSecs = howLong / 1000000 ;

  /**/ if (howLong ==   0)
    return ;
  else if (howLong  < 100)
    delayMicrosecondsHard (howLong) ;
  else
  {
    sleeper.tv_sec  = wSecs ;
    sleeper.tv_nsec = (long)(uSecs * 1000L) ;
    nanosleep (&sleeper, NULL) ;
  }
}
