#define daikin_freq  37
#define daikin_data  27
int count = 0;
byte delay_low = 0;
byte ir_byte_no = 0;
byte ir_header  = 0;
byte ir_state = 0;
char IR_data[daikin_data + 1] = 
{0x11,0xDA,0x27,0xF0,0x00,0x00,0x00,0x20,
//0    1    2   3    4    5     6   7         byte 7  sum
0x11,0xDA,0x27,0x00,0x00,0x30,0x32,0x00,
//0x11,0xDA,0x27,0x00,0x00,0x41,0x1E,0x00,
//8    9   10   11   12    13   14   15
0xAE,0x0A,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x18,0xE3,0x00 };
//16  17    18  19   20    21   22  23   24   25   26      byte 26  sum
/*
byte 13=mode
b7 = 0
b6+b5+b4 = Mode
b3 = 0
b2 = OFF timer set
b1 = ON timer set
b0 = Air Conditioner ON
Modes: b6+b5+b4
011 = Cool
100 = Heat (temp 23)
110 = FAN (temp not shown, but 25)
000 = Fully Automatic (temp 25)
010 = DRY (temp 0xc0 = 96 degrees c)

byte 14=temp*2

byte 16=Fan
FAN control
b7+b6+b5+b4 = Fan speed
b3+b2+b1+b0 = Swing control up/down
Fan: b7+b6+b5+b4
0×30 = 1 bar
0×40 = 2 bar
0×50 = 3 bar
0×60 = 4 bar
0×70 = 5 bar
0xa0 = Auto
0xb0 = Not auto, moon + tree
Swing control up/down:
0000 = Swing up/down off
1111 = Swing up/down on
Swing control left/right:
0000 = Swing left/right off
1111 = Swing left/right on
*/

void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  sendDaikin(daikin_data);
}

void loop() {
delay (500);
IR_data[13] &= 0xFE;
sendDaikin(daikin_freq);
}


void sendDaikin(int khz) {
  //--------------------------------------------- check sum
  
  	uint8_t sum = 0;
	uint8_t i;


	for(i = 0; i <= 6; i++){
		sum += IR_data[i];
	}

        IR_data[7] = sum & 0xFF;
        sum=0;
	for(i = 8; i <= 25; i++){
		sum += IR_data[i];
        }

        IR_data[26] = sum &0xFF;

  //---------------------------------------------
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  delay_low = 1;
  ir_byte_no = 0;
  ir_header  = 1;
  timer1_isr_init();
  timer1_attachInterrupt(dataIROut);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(clockCyclesPerMicrosecond()*500 / khz);

}

void dataIROut() {
static byte resume_tmp;  
static byte data_tmp;  
static int j; 
static int i; 
static int temp;
static int temp_go_low;
static int pwm_enable;

if (resume_tmp){
  resume_tmp = 0;
  delay_low = 0;
  timer1_write(clockCyclesPerMicrosecond()*500 / daikin_freq); // 38khz
}

if (ir_header == 1){
  ir_header = 0;
  j = 0;
  i = 0;
  pwm_enable = 1;
  temp = 384;
  temp_go_low = 256;
  data_tmp = IR_data[ir_byte_no];
} else {
  i++ ;
  if (i > temp)
  {
    i = 0;
    if (data_tmp & (1 << j)){
      temp = 128;    // High
    } else {
      temp = 64;    // Low
    }
    
    pwm_enable = 1;
    temp_go_low = 32;
    j++;
    
    if (j > 7) {
      j = 0;
      ir_byte_no ++;
      data_tmp = IR_data[ir_byte_no];
    }
    
    if (ir_byte_no == daikin_data && j>1){
        timer1_detachInterrupt();
        ir_state = 1;  //IR off      
    }
    
    if (ir_byte_no == 8 && delay_low == 1 && j > 1){
    j = 0;
    timer1_write(clockCyclesPerMicrosecond()*29000); // delay 30ms
    ir_header  = 1;   // call header
    resume_tmp = 1;  //resume
    ir_state = 1;  //IR off
    }
  }


}
    if(i >= temp_go_low) pwm_enable = 0;

    if(pwm_enable ==1){
      ir_state = 1 - ir_state;
      digitalWrite(1, ir_state);
    } else {
      ir_state = 0;
      digitalWrite(1, ir_state);
    }
    
}
