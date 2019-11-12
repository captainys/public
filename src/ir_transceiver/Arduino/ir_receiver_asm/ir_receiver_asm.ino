// Experimented if AVR assembler does any better job compared to C compiler.
// Conclusion is C compiler does very good job.

enum
{
  STATE_STANDBY,
  STATE_READY_TX,
};


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
  #define PULSE_BUF_LEN 2048
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



volatile unsigned char state=STATE_STANDBY;
volatile unsigned int nBufFilled=0;
volatile unsigned int pulseBuf[PULSE_BUF_LEN];


// Expanded I/O address of TCNT1.  Common for ATMega328 (UNO) and ATMega2560 (Mega)
#define TCNT1L_ADDR 0x84
#define TCNT1H_ADDR 0x85

// Expanded I/O address of TCNT2.  Common for ATMega328 (UNO) and ATMega2560 (Mega)
#define TCNT2_ADDR  0xB2

// Expanded I/O address of TCNT3.  ATMega2560 (Mega) only
#define TCNT3L_ADDR  0x94
#define TCNT3H_ADDR  0x95



void PinChange(void)
{
  volatile unsigned int nUsed;
  if(STATE_STANDBY!=state)
  {
    return;
  }

  SetPin13High;

  // 16-bit registers:  Write High->Low,  Read Low->High.  pp. 115 of ATMega328 datasheet.  Also no interleave readL->writeHigh->readHigh->WriteLow doesn't work.
  asm volatile (
  "   movw  x,%[pulseBuf]\n"
  "   sts   %[tcnt1H],%[zero]\n"   // 2 clocks
  "   sts   %[tcnt1L],%[zero]\n"   // 2 clocks

  "   lsr   %B[nAvailable]\n"             // 1 clock
  "   ror   %A[nAvailable]\n"             // 1 clock
  
  "HIGHLOOP:\n"
  "   in    %[ioread],%[port]\n"             // 1 clock
  "   andi  %[ioread],%[pin]\n"              // 1 clock
  "   breq  HIGHLOOP\n"                      // 2 clock  (Active Low)

  "   lds   %A[timerReg],%[tcnt1L]\n" // 2 clocks
  "   lds   %B[timerReg],%[tcnt1H]\n" // 2 clocks
  "   st    x+,%A[timerReg]\n"
  "   st    x+,%B[timerReg]\n"

  "   ldi   %[ioread],0\n"
  "   sts   %[tcnt2],%[ioread]\n"      // 2 clocks

  "LOWLOOP:\n"
  "   lds   %[ioread],%[tcnt2]\n"    // Dummy read
  "   cpi   %[ioread],255\n"
  "   breq  END_OF_SIGNAL\n"
  
  "   in    %[ioread],%[port]\n"      // 1 clock
  "   andi  %[ioread],%[pin]\n"       // 1 clock
  "   brne  LOWLOOP\n"                // 2 clock  (Active Low)

  "   lds   %A[timerReg],%[tcnt1L]\n" // 2 clocks
  "   lds   %B[timerReg],%[tcnt1H]\n" // 2 clocks
  "   st    x+,%A[timerReg]\n"
  "   st    x+,%B[timerReg]\n"

  "   sbiw  %[nAvailable],1\n"    // 2 clock
  "   brne  HIGHLOOP\n"           // 2 clock

  "END_OF_SIGNAL:\n"
  "   lsl   %A[nAvailable]\n"
  "   rol   %B[nAvailable]\n"
  "   ldi   %A[nBufUsed],%[bufLenL]\n"
  "   ldi   %B[nBufUsed],%[bufLenH]\n"
  "   sub   %A[nBufUsed],%A[nAvailable]\n"
  "   sbc   %B[nBufUsed],%B[nAvailable]\n"
  "ASM_EXIT:\n"
  :
  [nBufUsed] "=w" (nUsed)   // Needs to be input/output
  :
  [port] "M" (_SFR_IO_ADDR(PINE)),  // Arduino Mega Pin3=PE5
  [pin] "M" (_BV(5)),            // Arduino Mega Pin3=PE5
  [tcnt1L] "M" (TCNT1L_ADDR),
  [tcnt1H] "M" (TCNT1H_ADDR),
  [tcnt2] "M" (TCNT2_ADDR),
  [tcnt3L] "M" (TCNT3L_ADDR),
  [tcnt3H] "M" (TCNT3H_ADDR),
  [pulseBuf] "x" (pulseBuf),
  [ioread] "a" (0),              // "a" Use r16 to r23 
  [timerReg] "w" (0),            //
  [nAvailable] "w" (PULSE_BUF_LEN),
  [bufLenL] "M" (PULSE_BUF_LEN&255),
  [bufLenH] "M" ((PULSE_BUF_LEN>>8)&255),
  [zero] "M" (0)
    );

  if(0<nUsed)
  {
    nBufFilled=nUsed;
    state=STATE_READY_TX;
  }
  SetPin13Low;
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

  // Reset timer 2
  TCCR2A=0;
  // TCCR2B=bit(CS20); // No Pre-Scaling
  // TCCR2B=bit(CS21); // 8x Pre-Scaling
  TCCR2B=bit(CS20)|bit(CS21)|bit(CS22); // 1024x Pre-Scaling
  OCR2A=0;
  OCR2B=0;
  TCNT2=0;


  // Stop interrupt for millis etc.
  TCCR0A=0;
  TCCR0B=0;

  pinMode(3,INPUT);
  pinMode(13,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(3),PinChange,FALLING);
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
    Serial.print(total);
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
