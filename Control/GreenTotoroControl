
//this programm will put out a PPM signal

//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 8  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 3  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
  change theese values in your code (usually servo values move between 1000 and 2000)*/
int ppm[chanel_number];

void setup() {
  //initiallize default ppm values
  for (int i = 0; i < chanel_number; i++) {
    current[i] = ppm[i] = default_servo_value;
  }

  Serial.begin(9600);

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)

  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;

  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void loop() {

  for (int i = 0; i < 1; i++) {
      Serial.println("Foward");
      kereszt(1, 500);
      delay(1000);
      
      Serial.println("Left");
      csuro(-1, 500);
      delay(1000);
      
      Serial.println("Backward");
      kereszt(-1, 500);
      delay(1000);
      
      Serial.println("Right");
      csuro(1, 500);
  }

  while (1);
}


/*  500ms mellet 1 m -tesz meg 
 *  ha direct = 1 -> előre megy, 
 *  ha direct = -1 -> hátra megy,
 */
void kereszt(int direct, int delay_val)
{ 
  direct *= -1;
  int current = default_servo_value;
  
  current = smooth(current, default_servo_value + direct * 200, 10, 1); // Start mmoving foward or backward
  delay(delay_val);

  current = smooth(current, default_servo_value - direct * 400, 1, 1);
  delay(150);
  
  smooth(current, default_servo_value, 1, 1);
}

/*
 *  ha direct = 1 -> jobbra megy
 *  ha direct = -1 -> balra megy
 */
void csuro(int direct, int delay_val)
{
  int current = default_servo_value;
  
  current = smooth(current, default_servo_value + direct * 200, 10, 0); // Start mmoving right or left
  delay(delay_val);

  current = smooth(current, default_servo_value - direct * 400, 1, 0);
  delay(150);
  
  smooth(current, default_servo_value, 1, 0);
}

void forgo(int direct, int delay_val)
{

}

int smooth(int current, int goal, int delay_val, int channel)
{
    int grow = (goal - current) / 10;
  
    if (current < goal) {
        while (current < goal) {
          current += grow;
          ppm[channel] = current;
          Serial.println(String(current));
          delay(delay_val); 
        }
        
    } else {
        while (current > goal) {
          current += grow;
          ppm[channel] = current;
          Serial.println(String(current));
          delay(delay_val); 
        }
    }
    
    return current;
}

ISR(TIMER1_COMPA_vect) { //leave this alone
  static boolean state = true;

  TCNT1 = 0;

  if (state) { //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else { //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    digitalWrite(sigPin, !onState);
    state = true;

    if (cur_chan_numb >= chanel_number) {
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;//
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}
