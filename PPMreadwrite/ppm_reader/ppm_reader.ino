/*This program puts the ppm values into an array,
 reagrdless of channel number, polarity, ppm frame length, etc...
 You can even change these while scanning!*/

#define PPM_Pin 3  //this must be 2 or 3
#define multiplier (F_CPU/8000000)  //leave this alone
int ppm[16];  //array for storing up to 16 ppm signals
//#define DEBUG

void setup()
{
  Serial.begin(115200);
  Serial.println("USB1 ready");
 
  pinMode(PPM_Pin, INPUT);
  attachInterrupt(PPM_Pin - 2, read_ppm, CHANGE);

  TCCR1A = 0;  //reset timer1
  TCCR1B = 0;
  TCCR1B |= (1 << CS11);  //set timer1 to increment every 0,5 us
}

void loop()
{
  int count = 0;

  while(count < 10){  //print out the ppm values
    Serial.print(ppm[count]);
    Serial.print("  ");
    count++;
  }
  count = 0;
  Serial.println("");
  delay(10);  
}



void read_ppm(){  //leave this alone
  static unsigned int pulse;
  static unsigned long counter;
  static byte channel;
  static unsigned long last_micros;

  counter = TCNT1;
  TCNT1 = 0;

  if(counter < 700*multiplier){  //must be a pulse if less than 710us
    pulse = counter;
    #if defined(servoOut)
    if(sizeof(servo) > channel) digitalWrite(servo[channel], HIGH);
    if(sizeof(servo) >= channel && channel != 0) digitalWrite(servo[channel-1], LOW);
    #endif
  }
  else if(counter > 1900*multiplier){  //sync pulses over 1910us
    channel = 0;
    #if defined(DEBUG)
    Serial.print("PPM Frame Len: ");
    Serial.println(micros() - last_micros);
    last_micros = micros();
    #endif
  }
  else{  //servo values between 710us and 2420us will end up here
    ppm[channel] = (counter + pulse)/multiplier;
    #if defined(DEBUG)
    Serial.print(ppm[channel]);
    Serial.print("  ");
    #endif
    
    channel++;
  }
}

