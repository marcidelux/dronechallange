/*
 * pelda: 1;1600;5000
 * 
 * 1 tengely
 * 1600 ppm jel
 * 5000 ch3
 */


//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 8  //set the number of chanels
#define default_servo_value 1500  //set the default servo value
#define PPM_FrLen 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 4  //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////

int ppm[chanel_number];

void setup() {
  Serial.begin(9600);  
  Serial.println("Serial started!");

  //initiallize default ppm values
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
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

void loop() {
  while(Serial.available()) {
    
    String a=Serial.readString();// read the incoming data as string
    
    Serial.print("recieved message:");
    Serial.println(a);

    commandInterpreter(a);
  }

  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
  }

//!!!!!!! GAZ
  ppm[2] = 0;
}

void commandInterpreter(String command) {
      int lineSep = -1; 
      
//0;11;222;333;444|0;111;7772;55;248

      do {
        lineSep = command.indexOf('|');
        
        int ch1Sep = command.indexOf(';'); 
        int ch1 = command.substring(0, ch1Sep).toInt(); 
        
        int ch2Sep = command.indexOf(';', ch1Sep+1 ); 
        int ch2 = command.substring(ch1Sep+1, ch2Sep+1).toInt(); 
        
        int ch3Sep = command.indexOf(';', ch2Sep+1 ); 
        int ch3 = command.substring(ch2Sep+1, ch3Sep+1).toInt(); 
        
        int ch4Sep = command.indexOf(';', ch3Sep+1 );
        int ch4 = command.substring(ch3Sep+1, ch4Sep).toInt();

        int delaySep = command.indexOf(';', ch4Sep+1 );
        int delayValue = command.substring(ch4Sep+1, lineSep).toInt();

        command.remove(0,lineSep+1);

        Serial.print("Processed command: ");
        Serial.print(ch1);
        Serial.print(", ");
        Serial.print(ch2);
        Serial.print(", ");
        Serial.print(ch3);
        Serial.print(", ");
        Serial.print(ch4);
        Serial.print(", ");
        Serial.println(delayValue);

        ppm[0] =ch1;
        ppm[1] =ch2;
        //!!!!!GAZ
        ppm[2] =0;//ch3;
        ppm[3] =ch4;
        delay(delayValue);
      }while (lineSep !=-1);
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

    if(cur_chan_numb >= chanel_number){
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
