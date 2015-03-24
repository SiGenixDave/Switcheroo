#include <TimerHelpers.h>

const byte EDGE_DETECT_PIN = 2;

const byte LED_PIN = 13;
const byte TEST_60_OUT_PIN = 8;

void setup() {
	
	Serial.begin (19200);
	pinMode (LED_PIN, OUTPUT); 
	pinMode (TEST_60_OUT_PIN, OUTPUT);

	pinMode (EDGE_DETECT_PIN, INPUT);

	SetupCarrierFrequency (1.0E6, 0.50);
	SetupInterruptTrigger (CHANGE);
	SetupTimer2 ();

}	// end of setup


void loop()
{
	// TEST ONLY, jumper TEST_60_OUT_PIN arduino pin to pin 2 of the arduino
	// to simulate 60 Hz edge signal
	delay (7);
	digitalWrite (TEST_60_OUT_PIN, !digitalRead (TEST_60_OUT_PIN));
	//BlinkLed();
	//Serial.println("hello");
}

// Timer2 Overflow Interrupt Vector, called every 500 microseconds
ISR (TIMER2_OVF_vect) {

	Timer2Reset ();

	// For test only
	BlinkLed ();
	BlinkLed ();
};  

void Timer2Reset ()
{
	TCNT2 = (255 - 250) + 2;	//Reset Up Timer to base count (2 usecs * 250 = 500 usecs)
	TIFR2 = 0x00;				//Timer2 INT Flag Reg: Clear Timer Overflow Flag
}


void SetupTimer2 () 
{
	// Setup Timer2 to fire every 500 usces
	TCCR2B = 0x00;			//Disable Timer2 while we set it up
  
	Timer2Reset ();
  
	TIMSK2 = 0x01;			//Timer2 INT Reg: Timer2 Overflow Interrupt Enable
	TCCR2A = 0x00;			//Timer2 Control Reg A: Normal port operation, Wave Gen Mode normal
	TCCR2B = 0x03;			//Timer2 Control Reg B: Timer PreScaler set to 32 (2 usecs / counter tick)
}


// Uses Timer1 so as not to affect other Arduino functionality (delay()) and 
// therefore the output pin must be pin 10... can't change unless another Timer is used
// valid range for dutyCycle argument is 0.0 to 1.0. By enabling this function
// kills the serial debug port.
void SetupCarrierFrequency (double Hz, double dutyCycle)
{
	const byte timer1OutputB = 10;

	// convert from MHz to integer
	// Base Arduino clock is 16 MHz (16E6)
	int freqCnt = (int)(16E6 / Hz) - 1;
	int dutyCycleCnt = (int)((freqCnt + 1) * dutyCycle) - 1;

	pinMode (timer1OutputB, OUTPUT); 
	TIMSK1 = 0;				// no interrupts
	Timer1::setMode (15, Timer1::PRESCALE_1, Timer1::CLEAR_B_ON_COMPARE);
	
	OCR1A = freqCnt;		// pulse period
	OCR1B = dutyCycleCnt;   // pulse length
	
}

// Trigger the interrupt to occur on both edges of the input to Arduino pin 2. The 
// input must be pin 2 for interrupt 0
void SetupInterruptTrigger (int edge)
{
	attachInterrupt (0, ServiceEdgeDetectInterrupt, edge);
}

// Fired when signal on pin 2 changes state, depends on whether the interrupt 
// is setup for rising, falling or both
void ServiceEdgeDetectInterrupt ()
{
	// Reset 500 usecs timer service so that next 500 usecs ISR occurs 500 usecs after
	// edge is detected
	Timer2Reset ();

	//TODO TOMMY service any addition edge detect needs
}

void BlinkLed ()
{
	digitalWrite(LED_PIN, !digitalRead(LED_PIN));
}
