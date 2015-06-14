#define IR_pin 15

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define daikin_freq  37
#define daikin_data  27
#define On  1
#define Off  0
#define Mode_Auto 0x00
#define Mode_Dry  0x02
#define Mode_Cool 0x06
#define Mode_Heat 0x01
#define Mode_Fan  0x03
#define Fan_Auto  0x00
#define Fan_Night 0x05
#define Fan_1  0x0C
#define Fan_2  0x02
#define Fan_3  0x0A
#define Fan_4  0x06
#define Fan_5  0x0E

int count = 0;
byte delay_low = 0;
byte ir_byte_no = 0;
byte ir_header  = 0;
byte ir_state = 0;
char IR_data[daikin_data + 1] =
{   0x11,0xDA,0x27,0xF0,0x00,0x00,0x00,0x20,
//0    1    2   3    4    5     6   7         byte 7  sum
    0x11,0xDA,0x27,0x00,0x00,0x30,0x32,0x00,
//0x11,0xDA,0x27,0x00,0x00,0x41,0x1E,0x00,
//8    9   10   11   12    13   14   15
    0xAE,0x0A,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x18,0xE3,0x00
};
//16  17    18  19   20    21   22  23   24   25   26      byte 26  sum
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*



*/
//  functiOn  //
/*
void sendDaikin(int khz) ; //sent command
void PowerDaikin(int value) ; // On-Off
void Timer_On_Daikin(int value) ; // On-Off
void Timer_On_set_Daikin(int value) ;
void Timer_Off_Daikin(int value) ; // On-Off
void Timer_Off_set_Daikin(int value) ;
void Mode_Daikin(int value) ; // Mode_Auto,  Mode_Dry,  Mode_Cool,  Mode_Heat,  Mode_Fan
void Temp_Daikin(int value) ; // add_temp
void Fan_Daikin(int value) ;  //  Fan_Auto, Fan_Night, Fan_1, Fan_2, Fan_3, Fan_4, Fan_5
void Vertical_Swing_Daikin(int value) ; // On-Off
void HorizOntal_Swing_Daikin(int value) ; // On-Off
void Sensor_Daikin(int value) ; // On-Off
void Mold_Proof_Daikin(int value) ; // On-Off
void Silent_Daikin(int value) ; // On-Off
void Powerful_Daikin(int value) ; // On-Off
void EcOno_Daikin(int value) ; // On-Off
*/
void setup() {
    pinMode(IR_pin, OUTPUT);
    digitalWrite(IR_pin, LOW);
    sendDaikin(daikin_data);
}


void loop() {
    delay (500);
    IR_data[13] &= 0xFE;
    sendDaikin(daikin_freq);
}

//  functiOn  //

void PowerDaikin(int value) { // On-Off
    if (value) {
        IR_data[21] &= 0x7F  ;
    } else {
        IR_data[21] |= (0x7F+1)>>1  ;
    }
}
void Timer_On_Daikin(int value) { // On-Off
    if (value) {
        IR_data[21] &= 0xBF  ;
    } else {
        IR_data[21] |= (0xBF+1)<<1  ;
    }
}
void Timer_On_set_Daikin(int value) { // minutes from 00:00 (midnight)
    IR_data[26] &= ((value+1)<<1)>>3 ;
    IR_data[27] &= (((value+1)<<1)<<5) | 0x0F ;
}
void Timer_Off_Daikin(int value) { // On-Off
    if (value) {
        IR_data[21] &= 0xDF  ;
    } else {
        IR_data[21] |= (0xDF+1)<<1  ;
    }
}
void Timer_Off_set_Daikin(int value) { // minutes from 00:00 (midnight)
    IR_data[27] &= (((value+1)<<1)>>7) | 0xE0 ;
    IR_data[28] &=  (value+1)<<2  ;
}
void Mode_Daikin(int value) { // Mode_Auto,  Mode_Dry,  Mode_Cool,  Mode_Heat,  Mode_Fan
    IR_data[21] &= 0xF0  ;
    IR_data[21] |= value<<1 ;
}
void Temp_Daikin(int value) { // Temp
    IR_data[22] |= ((value+1)<<2) | 0x7E  ;
}
void Fan_Daikin(int value) { //  Fan_Auto, Fan_Night, Fan_1, Fan_2, Fan_3, Fan_4, Fan_5
    IR_data[24] |= value | 0xF0 ;
}
void Vertical_Swing_Daikin(int value) { // On-Off
    if (value) {
        IR_data[24] &= 0x0F  ;
    } else {
        IR_data[24] |= (0x0F+1)<<1  ;
    }
}
void HorizOntal_Swing_Daikin(int value) { // On-Off
    if (value) {
        IR_data[24] &= 0xF0  ;
    } else {
        IR_data[24] |= (0xF0+1)<<1  ;
    }
}
void Sensor_Daikin(int value) { // On-Off
    if (value) {
        IR_data[32] &= 0xBF  ;
    } else {
        IR_data[32] |= (0xBF+1)<<1  ;
    }
}
void Mold_Proof_Daikin(int value) { // On-Off
    if (value) {
        IR_data[33] &= 0xBF  ;
    } else {
        IR_data[33] |= (0xBF+1)<<1  ;
    }
}
void Silent_Daikin(int value) { // On-Off
    if (value) {
        IR_data[29] &= 0xFB  ;
    } else {
        IR_data[29] |= (0xFB+1)<<1  ;
    }
}
void Powerful_Daikin(int value) { // On-Off
    if (value) {
        IR_data[29] &= 0x7F  ;
    } else {
        IR_data[29] |= (0x7F+1)<<1  ;
    }
}
void EcOno_Daikin(int value) { // On-Off
    if (value) {
        IR_data[32] &= 0xDF  ;
    } else {
        IR_data[32] |= (0xDF+1)<<1  ;
    }
}

void sendDaikin(int khz) {
    //--------------------------------------------- check sum

    uint8_t sum = 0;
    uint8_t i;


    for(i = 0; i <= 6; i++) {
        sum += IR_data[i];
    }

    IR_data[7] = sum & 0xFF;
    sum=0;
    for(i = 8; i <= 25; i++) {
        sum += IR_data[i];
    }

    IR_data[26] = sum &0xFF;

    //---------------------------------------------
    // Enables IR output.  The khz value cOntrols the modulatiOn frequency in kilohertz.
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

    if (resume_tmp) {
        resume_tmp = 0;
        delay_low = 0;
        timer1_write(clockCyclesPerMicrosecond()*500 / daikin_freq); // 38khz
    }

    if (ir_header == 1) {
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
            if (data_tmp & (1 << j)) {
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

            if (ir_byte_no == daikin_data && j>1) {
                timer1_detachInterrupt();
                ir_state = 1;  //IR Off
            }

            if (ir_byte_no == 8 && delay_low == 1 && j > 1) {
                j = 0;
                timer1_write(clockCyclesPerMicrosecond()*29000); // delay 30ms
                ir_header  = 1;   // call header
                resume_tmp = 1;  //resume
                ir_state = 1;  //IR Off
            }
        }


    }
    if(i >= temp_go_low) pwm_enable = 0;

    if(pwm_enable ==1) {
        ir_state = 1 - ir_state;
        digitalWrite(IR_pin, ir_state);
    } else {
        ir_state = 0;
        digitalWrite(IR_pin, ir_state);
    }

}
