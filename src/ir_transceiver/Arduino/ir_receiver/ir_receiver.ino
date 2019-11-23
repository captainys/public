/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

enum
{
  STATE_STANDBY,
  STATE_READY_TX,
};


// Timer 1 is used for recording pulse timings.
// Don't care about overflow.  Just record 16-bit values.
// Later overflow will be taken into account.

// Timer 2 is used for detecting long IR off.
// Using 1024x scaling and if it goes up to 255, it is taken as IR signal is done.


#if defined(__AVR_ATmega2560__)    // Arduino Mega 2560
  #define PULSE_BUF_LEN 3072

  #define Pin3State (PINE&_BV(5))

  #define SetPin0High  PORTE|=_BV(0);
  #define SetPin0Low   PORTE&=~_BV(0);
  #define SetPin1High  PORTE|=_BV(1);
  #define SetPin1Low   PORTE&=~_BV(1);
  #define SetPin2High  PORTE|=_BV(4);
  #define SetPin2Low   PORTE&=~_BV4();
  #define SetPin3High  PORTE|=_BV(5);
  #define SetPin3Low   PORTE&=~_BV(5);
  #define SetPin4High  PORTG|=_BV(5);
  #define SetPin4Low   PORTG&=~_BV(5);
  #define SetPin5High  PORTE|=_BV(3);
  #define SetPin5Low   PORTE&=~_BV(3);
  #define SetPin6High  PORTH|=_BV(3);
  #define SetPin6Low   PORTH&=~_BV(3);
  #define SetPin7High  PORTH|=_BV(4);
  #define SetPin7Low   PORTH&=~_BV(4);

  #define SetPin8High  PORTH|=_BV(5);
  #define SetPin8Low   PORTH&=~_BV(5);
  #define SetPin9High  PORTH|=_BV(6);
  #define SetPin9Low   PORTH&=~_BV(6);
  #define SetPin10High PORTB|=_BV(4);
  #define SetPin10Low  PORTB&=~_BV(4);
  #define SetPin11High PORTB|=_BV(5);
  #define SetPin11Low  PORTB&=~_BV(5);
  #define SetPin12High PORTB|=_BV(6);
  #define SetPin12Low  PORTB&=~_BV(6);
  #define SetPin13High PORTB|=_BV(7);
  #define SetPin13Low  PORTB&=~_BV(7);
#elif defined(__AVR_ATmega328P__)          // Arduino Uno (Can be Nano)
  #define PULSE_BUF_LEN 768
  #define Pin3State (PIND&_BV(3))

  #define SetPin0High  PORTD|=_BV(0);
  #define SetPin0Low   PORTD&=~_BV(0);
  #define SetPin1High  PORTD|=_BV(1);
  #define SetPin1Low   PORTD&=~_BV(1);
  #define SetPin2High  PORTD|=_BV(2);
  #define SetPin2Low   PORTD&=~_BV(2);
  #define SetPin3High  PORTD|=_BV(3);
  #define SetPin3Low   PORTD&=~_BV(3);
  #define SetPin4High  PORTD|=_BV(4);
  #define SetPin4Low   PORTD&=~_BV(4);
  #define SetPin5High  PORTD|=_BV(5);
  #define SetPin5Low   PORTD&=~_BV(5);
  #define SetPin6High  PORTD|=_BV(6);
  #define SetPin6Low   PORTD&=~_BV(6);
  #define SetPin7High  PORTD|=_BV(7);
  #define SetPin7Low   PORTD&=~_BV(7);

  #define SetPin8High  PORTB|=_BV(0);
  #define SetPin8Low   PORTB&=~_BV(0);
  #define SetPin9High  PORTB|=_BV(1);
  #define SetPin9Low   PORTB&=~_BV(1);
  #define SetPin10High PORTB|=_BV(2);
  #define SetPin10Low  PORTB&=~_BV(2);
  #define SetPin11High PORTB|=_BV(3);
  #define SetPin11Low  PORTB&=~_BV(3);
  #define SetPin12High PORTB|=_BV(4);
  #define SetPin12Low  PORTB&=~_BV(4);
  #define SetPin13High PORTB|=_BV(5);
  #define SetPin13Low  PORTB&=~_BV(5);

  #define SetPin9and10Low PORTB&=~(_BV(1)|_BV(2));

//#elif defined(__AVR_ATMega4809__)          // Arduino Nano Every
//#elif defined(__AVR_ATmega328__)           // Arduino Nano?
//#elif defined(__AVR_ATmega32U4__)          // Arduino Leonardo
//  #elif defined(__AVR_ATmega__)
//  #elif defined(__AVR_ATmega__)
//  #elif defined(__AVR_ATmega__)
#else
  #error Unable to identify Board Type
#endif


volatile int state=STATE_STANDBY;
volatile unsigned int nBufFilled=0;
volatile unsigned int pulseBuf[PULSE_BUF_LEN];


void PinChange(void)
{
  TCNT1=0;

  if(STATE_STANDBY!=state)
  {
    return;
  }
  
  SetPin13High;

  while(nBufFilled<PULSE_BUF_LEN)
  {
    while(0==Pin3State)
    {
    }

    pulseBuf[nBufFilled]=TCNT1;
    ++nBufFilled;

    // Infra-Red OFF (Pin3=Active Low)
    TCNT2=0;
    while(0!=Pin3State)
    {
      if(255==TCNT2)
      {
        goto END_OF_SIGNAL;
      }
    }

    pulseBuf[nBufFilled]=TCNT1;
    ++nBufFilled;
  }
END_OF_SIGNAL:

  SetPin13Low;
  state=STATE_READY_TX;
}


void ClearBuffer(void)
{
  nBufFilled=0;
  for(int i=0; i<PULSE_BUF_LEN; ++i)
  {
    pulseBuf[i]=0;  
  }
}

void setup()
{
  state=STATE_STANDBY;
  ClearBuffer();


  // Reset timer 1
  TCCR1A=0;
  // TCCR1B=bit(CS10); // No Pre-Scaling
  TCCR1B=bit(CS11); // 8x Pre-Scaling
  OCR1A=0;
  OCR1B=0;
  TCNT1=0;

  TCCR2A=0;
  // TCCR2B=bit(CS20); // No Pre-Scaling
  // TCCR2B=bit(CS21); // 8x Pre-Scaling
  TCCR2B=bit(CS20)|bit(CS21)|bit(CS22); // 1024x Pre-Scaling
  OCR2A=0;
  OCR2B=0;
  TCNT2=0;


  pinMode(3,INPUT);
  pinMode(13,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),PinChange,FALLING);

  Serial.begin(115200);
}

void loop()
{
  if(STATE_READY_TX==state)
  {
    Serial.begin(115200);
    Serial.println("Begin");
    long int total=0;
    unsigned long int prevPtr=0;
    unsigned long int basePtr=0;
    int balance=0;
    for(int i=0; i<nBufFilled; ++i)
    {
      if(0<i && pulseBuf[i]<pulseBuf[i-1])
      {
        basePtr+=0x10000;
      }
      unsigned long int curPtr=basePtr+pulseBuf[i];
      if(0!=(curPtr&1)) // 1 tick = 0.5us.  Need to make everything even, and send tick/2.
      {
        if(0<balance && prevPtr+2<curPtr)
        {
          --curPtr;
          --balance;
        }
        else
        {
          ++curPtr;
          ++balance;
        }
      }
      unsigned long int dt=curPtr-prevPtr;
      prevPtr=curPtr;

      Serial.print(dt/2);
      Serial.print(" ");

      total+=dt;
    }

    Serial.println("");
    Serial.println("End");
    Serial.print("Total=");
    Serial.print((long int)total);
    Serial.println("");
    Serial.print("nSample=");
    Serial.print(nBufFilled);
    Serial.println("");
    Serial.end();

    ClearBuffer();
    state=STATE_STANDBY;
  }

  TCCR0A=0;
  TCCR0B=0;
}
