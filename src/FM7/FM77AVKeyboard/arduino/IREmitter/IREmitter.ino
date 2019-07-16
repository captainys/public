/*
38KHz 50% duty cycle Infra-Red Signal Emitter
Copyright 2019 CaptainYS (http://www.ysflight.com)  All rights reserved.

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


#define SERIAL_BPS 380000
// Confirmed to work up to 440000bps.
// Stopped working at 460000bps.
// Not tested between 440000 and 460000bps.


#define RECVBUF_SIZE 384
int nRecvBuf=0;
unsigned char recvBuf[RECVBUF_SIZE];
unsigned int cycle[RECVBUF_SIZE/2];
bool transmitMode=false;
unsigned long lastDataReceivedTime=0;
unsigned char processingCmd=0;

#define CMD_RESET 0
#define CMD_IRMAN_COMPATIBLE_MODE 'R'
#define CMD_VERSION 'V'
#define CMD_SELFTEST 't'
#define CMD_SAMPLERMODE 'S'
#define CMD_ENABLE_BYTECOUNT 0x24
#define CMD_ENABLE_TRANSMISSION_NOTIFY 0x25
#define CMD_ENABLE_HANDSHAKE 0x26
#define CMD_TRANSMIT_MICROSEC 0x80
#define CMD_TRANSMIT_30BIT_100US 0x81

#define NOTIFY_READY 'C'
#define NOTIFY_FAIL 'F'

// 50% duty cycle: On and off within 1/38000 sec.  Must togle every 1/76000 sec.
#define TIMER_THRESHOLD_38K_WITH_1xPRESCALE ((F_CPU/38000)/2)
// Experiment with FM77AV40 IR LED receiver:  19000Hz No response 34000-36000 drops key.  38000 perfect.  40000 drops keys.

#define PWM_TIMER_THRESHOLD TIMER_THRESHOLD_38K_WITH_1xPRESCALE

// Succeeded Xfinity remote

#define PIN_OC1A 9
#define PIN_OC1B 10
#define PIN_OC2A 11
#define PIN_OC2B 3

#define PIN_POWER 13
#define PIN_STATUS 8

// PIN9=PB1 (PortB bit 1)
#define SetPin9High  PORTB|=_BV(1);
#define SetPin9Low   PORTB&=~_BV(1);
// PIN10=PB2 (PortB bit 2)
#define SetPin10High PORTB|=_BV(2);
#define SetPin10Low  PORTB&=~_BV(2);
// PIN11=PB3 (PortB bit 3)
#define SetPin11High PORTB|=_BV(3);
#define SetPin11Low  PORTB&=~_BV(3);
// PIN3=PD3 (PortD bit 3)
#define SetPin3High  PORTD|=_BV(3);
#define SetPin3Low   PORTD&=~_BV(3);


#define SetPin9and10Low PORTB&=~(_BV(1)|_BV(2));


#define START_TIMER1 {TCNT1=PWM_TIMER_THRESHOLD-1;TCCR1A=bit(COM1A0)|bit(COM1B0);TCCR1B=bit(CS10)|bit(WGM12);}
#define STOP_TIMER1 {TCCR1A=0;TCCR1B=0;TCNT1=0;}
// ATmega 328 datasheet pp.134
// TCCR1A  COM1A0=1, COM1B0=1 means toggle OC1A and OC1B on compare match
// TCCR1B  WGM10=0,WGM11=0,WGM12=1 means CTC mode
//         CS10=1,CS11=0,CS12=0 means 1x pre-scale (no scaling).
//         CS10=0,CS11=0,CS12=0 means timer stop.

#define SET_OC1A_OC1B_LOW {TCNT1=0;TCCR1A=(bit(COM1A1)|bit(COM1B1));TCCR1C=(bit(FOC1A)|bit(FOC1B));TCCR1C=0;TCCR1A=0;}
// TCCR1A=(bit(COM1A1)|bit(COM1B1));  // Set OC1A low on compare match
// TCCR1C=(bit(FOC1A)|bit(FOC1B));    // Force match
// TCCR1C=0;                          // Do I need to clear?

#define SET_OC1A_OC1B_HIGH {TCNT1=0;TCCR1A=(bit(COM1A0)|bit(COM1B0)|bit(COM1A1)|bit(COM1B1));TCCR1C=(bit(FOC1A)|bit(FOC1B));TCCR1C=0;TCCR1A=0;}
// TCCR1A=(bit(COM1A1)|bit(COM1B1)|bit(COM1A0)|bit(COM1B0)|bit(COM1A1)|bit(COM1B1));  // Set OC1A low on compare match
// TCCR1C=(bit(FOC1A)|bit(FOC1B));    // Force match
// TCCR1C=0;                          // Do I need to clear?


void setup() {
  Serial.begin(SERIAL_BPS);

  pinMode(PIN_POWER,OUTPUT);
  pinMode(PIN_STATUS,OUTPUT);

  pinMode(PIN_OC1A,OUTPUT);
  pinMode(PIN_OC1B,OUTPUT);
  pinMode(PIN_OC2A,OUTPUT);
  pinMode(PIN_OC2B,OUTPUT);

  // Reset timers 1 and 2
  TCCR1A=0;
  TCCR1B=0;
  TCCR2A=0;
  TCCR2B=0;
  TCNT1=0;
  TCNT2=0;

  // Timer 1 (8-bit) for 38K PWM IR LED output
  // ATmega 328 datasheet pp.134
  OCR1A=PWM_TIMER_THRESHOLD;
  OCR1B=0;

  SET_OC1A_OC1B_LOW;

  // Timer 2 for measuring 1us tick.
  TCCR2A=0;
  TCCR2B=bit(CS21);
  // CS20=0,CS21=1,CS22=0 means 8x pre-scaling.  0.5us per tick.  ATmega328 datasheet pp.162
  OCR2A=0;
  OCR2B=0;

  digitalWrite(PIN_POWER,LOW);
  digitalWrite(PIN_STATUS,LOW);
}

void SendCycleHWPWM(unsigned int cycle[])
{
  noInterrupts();

  for(int i=0; cycle[i]!=0xffff; i+=2)
  {
    TCNT2=0;
    START_TIMER1;
    auto w=cycle[i]<<1;
    while(TCNT2<w)
    {
      if(240<=TCNT2)
      {
        TCNT2=0;
        w-=240;
      }
    }

    STOP_TIMER1;
    SetPin9and10Low;

    TCNT2=0;
    w=cycle[i+1]<<1;
    while(TCNT2<w)
    {
      if(240<=TCNT2)
      {
        TCNT2=0;
        w-=240;
      }
    }
  }

  SET_OC1A_OC1B_LOW;
  SetPin9and10Low;

  interrupts();
}

unsigned long MakeCycle(unsigned int cycle[],int nSample,unsigned char sample[])
{
  int k=0;
  unsigned long total=0;
  switch(processingCmd)
  {
  case CMD_TRANSMIT_MICROSEC:
    {
      for(int i=0; i+1<nSample && (sample[i]!=0xff || sample[i+1]!=0xff); i+=2)
      {
        cycle[k]=(sample[i]<<8);
        cycle[k]+=sample[i+1];
        total+=cycle[k];
        ++k;
      }
    }
    break;
  case CMD_TRANSMIT_30BIT_100US:
    {
      unsigned int pulseWidth[3]={100,125,175}; 
      cycle[k]=0;
      for(int i=0; i<30; ++i)
      {
        bool currentBit=(0==(k&1));
        bool nextBit=(sample[i>>3]&(1<<(i&7)));
        if(currentBit!=nextBit)
        {
          ++k;
          cycle[k]=0;
        }
        cycle[k]+=pulseWidth[i%3];
      }
      ++k;
      for(int i=0; i<k; ++i)
      {
        total+=cycle[i];
      }
    }
    break;
  }
  if(0!=(k&1))
  {
    cycle[k]=0;
    ++k;
  }
  cycle[k  ]=0xffff;
  cycle[k+1]=0xffff;

  return total; // Returns total number of microseconds.
}

// Prevent short wave (38K PWM wave cut off in the middle)
void PulseWidthAdjustment(unsigned int cycle[])
{
  for(int i=0; cycle[i]!=0xffff && cycle[i+1]!=0xffff; i+=2)
  {
    // High cycle must be 26us times (integer+0.5) ideally.  
    // Last high then low then the end of high cycle.
    // If high cycle is 26us times integer, it might emit last extremely short flash of IR.
    // So, how about forcing the high cycle to be 26*N+13?
    // Well, to be safe, say 26*N+15.  Last 2us is just no-IR emission.

    auto m=cycle[i]%26;

    if(m<15)
    {
      auto d=15-m;
      cycle[i  ]+=d;
      if(d<=cycle[i+1])
      {
        cycle[i+1]-=d;
      }
    }
    else
    {
      auto d=m-15;
      cycle[i  ]-=d;
      cycle[i+1]+=d;
    }
  }
}

void Transmit()
{
  digitalWrite(PIN_STATUS,HIGH);
  if(MakeCycle(cycle,nRecvBuf,recvBuf)<5000)
  {
    // If the duration goes beyond 4000us, most likely some bytes have been lost.
    // Skip one and try to recover.
    PulseWidthAdjustment(cycle);
    SendCycleHWPWM(cycle);
  }

  transmitMode=false;
  nRecvBuf=0;

  while(0==Serial.availableForWrite());
  Serial.write(NOTIFY_READY);

  digitalWrite(PIN_STATUS,LOW);
}

void loop() {
  bool received=false;
  while(0<Serial.available())
  {
    auto recvByte=Serial.read();
    if(true!=transmitMode)
    {
      processingCmd=recvByte;
      if(CMD_IRMAN_COMPATIBLE_MODE==recvByte)
      {
        Serial.println("OK");
        delayMicroseconds(10);
     }
      else if(CMD_VERSION==recvByte)
      {
        Serial.println("A277");
        delayMicroseconds(10);
      }
      else if(CMD_SAMPLERMODE==recvByte)
      {
        Serial.println("S77");
        delayMicroseconds(10);
      }
      else if(CMD_TRANSMIT_MICROSEC==recvByte ||
              CMD_TRANSMIT_30BIT_100US==recvByte)
      {
        transmitMode=true;
        nRecvBuf=0;
      }
    }
    else
    {
      recvBuf[nRecvBuf++]=recvByte;
      if(CMD_TRANSMIT_30BIT_100US==processingCmd && 4<=nRecvBuf)
      {
        Transmit();
      }
      else if(RECVBUF_SIZE<=nRecvBuf)
      {
        Transmit();
      }
      else if(2<=nRecvBuf && 0xff==recvBuf[nRecvBuf-1] && 0xff==recvBuf[nRecvBuf-2])
      {
        Transmit();
      }
    }
    received=true;
  }

  unsigned long t=millis();
  if(true==received)
  {
    lastDataReceivedTime=t;
    digitalWrite(PIN_POWER,LOW);
  }
  else
  {
    // Second mode of failure.
    // In transmit mode, the FIFO buffer is overwhelmed and starts losing bytes.
    // The terminator 0xffff won't be caught, however, 0x03 included in the
    // pulse-width data put it into the transmitMode again, and never recover.
    // To get out of this mode, the user needs to release the key for 100ms.
    // If no byte is received for 100ms, the program goes back to command mode.
    if(true==transmitMode)
    {
      if(t<lastDataReceivedTime ||               // Timer overflow
         100<t-lastDataReceivedTime) // 100ms no transmittion from host
      {
        transmitMode=false;
        nRecvBuf=0;
        Serial.write(NOTIFY_FAIL);
        Serial.write(NOTIFY_READY);
      }
    }
    int c=(((t-lastDataReceivedTime)/500)&1);
    auto highLow=(0==c ? HIGH : LOW);
    digitalWrite(PIN_POWER,highLow);
  }
}
