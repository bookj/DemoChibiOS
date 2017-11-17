// Example of mutex to prevent print calls from being scrambled.
#include "ChRt.h"

// Set USE_MUTEX to zero for scrambled print.
#define USE_MUTEX 1

// Declare and initialize a mutex for limiting access to threads.
MUTEX_DECL(demoMutex);

// Data structures and stack for thread 2.
static THD_WORKING_AREA(waTh2, 200);

// Data structures and stack for thread 3.
static THD_WORKING_AREA(waTh3, 200);

// Data structures and stack for thread 4.
static THD_WORKING_AREA(waTh4, 200);

int LED[3] = {2, 3, 4}; // connect the LED 2-4
int i;
String pin = "";
//------------------------------------------------------------------------------
static THD_FUNCTION(thdFcn, arg) {
  while (true) {
    #if USE_MUTEX
    // Wait to enter print region.
    chMtxLock(&demoMutex);
    #endif  // USE_MUTEX
  
    // Only one thread in this region while doing prints.
    Serial.print("Hello, I am turning on my LED. I am thread ");
    Serial.println((const char*)arg);
    pin = "";
    pin += (char*)arg;
    digitalWrite(pin.toInt(), HIGH);
    chThdSleep(500);
    digitalWrite(pin.toInt(), LOW);
    chThdSleep(500);
 
    #if USE_MUTEX 
    // Exit protected region.
    chMtxUnlock(&demoMutex);
    #endif  // USE_MUTEX 
  }
}
//------------------------------------------------------------------------------
void chStartup() {
  // Schedule thread 2.
  chThdCreateStatic(waTh2, sizeof(waTh2), NORMALPRIO, thdFcn, (void*)"2");

  // Schedule thread 3.
  chThdCreateStatic(waTh3, sizeof(waTh3), NORMALPRIO, thdFcn, (void*)"3");

  // Schedule thread 4.
  chThdCreateStatic(waTh4, sizeof(waTh4), NORMALPRIO, thdFcn, (void*)"4");

  // Main thread is thread 1 at NORMALPRIO.
//  thdFcn((void*)"Th 1");  
}
//------------------------------------------------------------------------------
void setup() {
  for (i = 0; i < 3; i++) {
    pinMode(LED[i], OUTPUT);
  }
  
  Serial.begin(9600);
  // Wait for USB Serial.
  while (!Serial) {}
  
  // Initialize and start ChibiOS.
  chBegin(chStartup);
  // chBegin() resets stacks and should never return.
  while (true) {}
}
//------------------------------------------------------------------------------
void loop() {/* Not used. */}
