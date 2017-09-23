

//this programm will put out a PPM signal
//////////////////////CONFIGURATION///////////////////////////////
#define chanel_number 4  //set the number of chanels
#define default_servo_value 1520  //set the default servo value
#define PPM_FrLen 20000  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PPM_PulseLen 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 3   //set PPM signal output pin on the arduino
//////////////////////////////////////////////////////////////////

//DC3k
#define PARSE_CHAR ','

//String msg;
char msg;

int value1;
int value2;
int value3;

long prev_count = 0;
long curr_count = 0;

/*this array holds the servo values for the ppm signal
 change theese values in your code (usually servo values move between 1000 and 2000)*/
int ppm[chanel_number];
int inputs[chanel_number];
unsigned int count = 0;
bool enable = false;
unsigned long lastTime = 0;


//inputs: int inputs[4], unsigned long lastTime
//maximal and minimal outputs: [1300, 1700]
//argument: rate of change (change of the output per milliseconds) def
//output: int ppm[4]

void smooth(int inputs[], int ppm[], int steep = 1){
  unsigned long timeNow = millis();
  unsigned long timeDiff = timeNow  - lastTime;
  int stepSize = steep * timeDiff;
  lastTime = timeNow; 

  for(int i = 0;i < chanel_number;i++)
    {
      // input check
      if(inputs[i] > 1700)
      {
        inputs[i] = 1700;
      }
  
      if(inputs[i] < 1300)
      {
        inputs[i] = 1300;
      }
  
      if(inputs[i] != ppm[i])
      {
        int sub = inputs[i] - ppm[i];
  
        if(sub > 0){
  
          if(sub <= stepSize){
            ppm[i] = inputs[i];
          }
          else{
            ppm[i] += stepSize;
          }
        }
        else{
          if(sub >= stepSize){
            ppm[i] = inputs[i];
          }
          else{
            ppm[i] -= stepSize;
          }
        }
        
      }
    }
  

}




void setup(){

  //Serial.begin(9600);
    
  //initiallize default ppm values
  for(int i=0; i<chanel_number; i++){
    ppm[i]= default_servo_value;
    inputs[i] = default_servo_value;
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

void loop(){
  /*
  //put main code here
  static int val = 1;
  int in = 0;
  if(count == 1)
  {
    inputs[1] = 1300;
  }
  else
  {
    if(count == 40)
    {
      inputs[1] = 1700;
    }
    else
    {
      if(count == 80)
      {
        inputs[1] = 1520;
      }
    }
  }

    if(count == 120)
  {
    inputs[0] = 1300;
  }
  else
  {
    if(count == 160)
    {
      inputs[0] = 1700;
    }
    else
    {
      if(count == 200)
      {
        inputs[0] = 1520;
      }
    }
  }

    if(count == 240)
  {
    inputs[1] = 1700;
  }
  else
  {
    if(count == 280)
    {
      inputs[1] = 1300;
    }
    else
    {
      if(count == 320)
      {
        inputs[1] = 1520;
      }
    }
  }

    if(count == 360)
  {
    inputs[0] = 1700;
  }
  else
  {
    if(count == 400)
    {
      inputs[0] = 1300;
    }
    else
    {
      if(count == 440)
      {
        inputs[0] = 1520;
      }
    }
  }

  if(enable){
    count++;
  }


  // read input to inputs[..]
  if(Serial.available() > 0){
    if(Serial.available()){
      String input = Serial.readStringUntil('\n');
      in = 0;

      for(int i = 0; i < input.length(); i++){
          in = (in*10) + (input[i]-'0');

          if(in = 1){
            count = 0;
            enable = true;
            }
  
        }

      
      }
    
  }
  */

  inputs[0] = 1520;
  inputs[1] = 1520;
  if(Serial.available()) {
    /*
    msg = Serial.readString();

    value1 = getValue(msg, PARSE_CHAR, 0).toInt();
    value2 = getValue(msg, PARSE_CHAR, 1).toInt();
    value3 = getValue(msg, PARSE_CHAR, 2).toInt();

    Serial.println("id: " + String(value1));
    Serial.println("forward movement: " + String(value2));
    Serial.println("rotation: " + String(value3));
    */

    msg = Serial.read();

    if(msg == 'w'){
      inputs[1] = 1300;
    }
    else if(msg == 's'){
      inputs[1] = 1700;
    }
    else if(msg == 'a'){
      inputs[0] = 1300;
    }
    else if(msg == 'd'){
      inputs[0] = 1700;
    }
  }

  

  //call it on every iteration
  smooth(inputs, ppm, 3); //param 1 - /mekkora legyen a smooth-olás meredeksége
  
  Serial.print(ppm[0]);
  Serial.print('\t');
  Serial.println(ppm[1]);
  
  delay(20);
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
