//this programm will put out a PPM signal

//////////////////////CONFIGURATION///////////////////////////////
#define channel_number 5  //set the number of chanels
#define default_ppm_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 3  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////


/*this array holds the servo values for the ppm signal
 change theese values in your code (usually ppm values move between 1000 and 2000)*/
int ppm[channel_number];
int upLimit = 0;
int lowLimit = 0;

void setup(){  
  Serial.begin(115200);
  Serial.write("USB2 OK");
  //initiallize default ppm values
  
  for(int i=0; i<channel_number; i++){
    ppm[i]= default_ppm_value;
  }

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

void loop(){
  //put main code here
  static int val = 1;
  for(int i = 0; i < channel_number; ){
      ppm[i] = ppm[i] + val;
      if(ppm[i] >= 2000){ val = -1; upLimit = 1;}
      if(ppm[i] <= 1000){ val = 1; lowLimit = 1;}
      if(upLimit == 1 && lowLimit == 1){ i++; upLimit = 0; lowLimit = 0;}
  delay(10);
  }

}

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if(state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PPM_PulseLen * 2;
    state = false;
  }
  else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= channel_number){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PPM_PulseLen;// 
      OCR1A = (PPM_FrLen - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb];
      cur_chan_numb++;
    }     
  }
}
