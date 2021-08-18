// RFID based door lock

#include <stdio.h>
#include <string.h>
#include <reg51.h>

#define LCD P2

sbit rs = P2^0;
sbit rw = P2^1;
sbit en = P2^2;
sbit motor1 = P1^3;
sbit motor2 = P1^2;

char keyinput();
void delay(int dtime);
void lcdcmd(unsigned char ch);
void lcddata(unsigned char ch);
void lcdprint(char *str);
void daten();
void cmden();
void init();
void uart_init();

char i;
char rfid[13], ch = 0;

void main() {

    uart_init();    // initialize uart
    init();         // initialize lcd

    lcdprint(" ===RFID LOCK===");
    lcdcmd(0xc0);
    lcdprint(" Door is Locked"); 
    delay(500);

    // loop to take card scan
    while(1) {

        lcdcmd(1);
        lcdprint(" Scan Your Card");

        // taking input
        i = 0;
        for(i=0; i<12; i++) {

            rfid[i] = keyinput();

        }
        rfid[i] = '\0';

        lcdcmd(1);
		lcdprint("Processing Card");
        lcdcmd(0xc0);

        for(i=0; i<12; i++) {

            lcdprint("*");

        }
        delay(100);

        // compare the input with the predefined data
        if(strncmp(rfid, "AA2006527052", 12) == 0) {

            lcdcmd(1);             
            lcdprint(" Card Accepted!");
            delay(300);
            lcdcmd(1);
            lcdprint(" =Door Unlocked=");
            delay(200);
            
            // spin the motor to unlock the door
            motor1 = 1;
            motor2 = 0;
            delay(500);

            motor1 = 0;
            motor2 = 1;
            delay(400);

            motor1 = 0;
            motor2 = 0;

        }
        // if the card not exist
        else {

           lcdcmd(1);
           lcdprint("CardNotRegistered!");
           lcdcmd(0xc0);
           lcdprint("Try Another Card");
           delay(300);

        }
    }
}

char keyinput() {

    // wait for keyboard interrupt
    while(!RI);
    ch = SBUF;
    RI = 0;
    return ch;

}

void delay(int dtime) {

    // delay for the designated time
    unsigned char i;
    for(dtime; dtime>0; dtime--) {

        for(i=250; i>0; i--);
        for(i=248; i>0; i--);

    }
}

void lcdcmd(unsigned char ch) {

    LCD = ch & 0xF0;
    cmden();
    LCD = (ch<<4) & 0xF0;
    cmden();

}

void lcddata(unsigned char ch) {

    LCD = ch & 0xF0;
    daten();
    LCD = (ch<<4) & 0xF0;
    daten();

}

void lcdprint(char *str) {

    // print text to the lcd
    while(*str) {

        lcddata(*str++);

    }
}

void daten() {

    // reset lcd display
    rs = 1;
    rw = 0;
    en = 1;
    delay(5);
	en = 0;

}

void cmden() {

    rs = 0;
    en = 1;
    delay(5);
    en = 0;

}

void init() {

    lcdcmd(0x02);
    lcdcmd(0x28);   // 4-bit mode, 2 line, 4x7 dots
    lcdcmd(0x0E);   // display on cursor ON
    lcdcmd(0x01);   // clear display

}

void uart_init() {

    TMOD = 0x20;
    SCON = 0x50;
    TH1 = 0xFD;
    TR1 = 1;

}
