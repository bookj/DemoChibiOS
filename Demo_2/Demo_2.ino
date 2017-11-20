#include <ChRt.h>

// Pin output to LED
const int OUTPUT_PIN = 2;
// Pin input from push switch to generate interrupt
const int INPUT_PIN = 3;

// Initialize semaphore as taken. 
BSEMAPHORE_DECL(isrSem, true);

//------------------------------------------------------------------------------
// ISR function
void isrFcn() {

  // On ARM CH_IRQ_PROLOGUE is void.
  CH_IRQ_PROLOGUE();
  // IRQ handling code, preemptable if the architecture supports it.

  chSysLockFromISR();
  // Invocation of some I-Class system APIs, never preemptable.

  // Signal handler task.
  chBSemSignalI(&isrSem);
  chSysUnlockFromISR();

  // More IRQ handling code, again preemptable.

  // Perform rescheduling if required.
  CH_IRQ_EPILOGUE();
}
//------------------------------------------------------------------------------
// Handler task for interrupt.
static THD_WORKING_AREA(waThd1, 200);

static THD_FUNCTION(handler, arg) {
  (void)arg;
  while (true) {
    // wait for event
    chBSemWait(&isrSem);
    
    Serial.println("EXIT");
    digitalWrite(OUTPUT_PIN, LOW);
    exit(0);
  }
}
//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Wait for USB Serial.
  while (!Serial) {}

  chBegin(mainThread);
  // chBegin() resets stacks and should never return.
  while (true) {}
}
//------------------------------------------------------------------------------
void mainThread() {
  // Setup and check pins.
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  
  // Start handler task.
  chThdCreateStatic(waThd1, sizeof(waThd1), NORMALPRIO + 1, handler, NULL);
 
  // Attach interrupt function.
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), isrFcn, RISING);
  while (true) {
    // Cause an interrupt - normally done by external event.
    Serial.println("High");
    digitalWrite(OUTPUT_PIN, HIGH);
    chThdSleepMilliseconds(200);
    Serial.println("Low");
    digitalWrite(OUTPUT_PIN, LOW);
    chThdSleepMilliseconds(200);
  }
}
//------------------------------------------------------------------------------
void loop() {
  // Not used.
}
