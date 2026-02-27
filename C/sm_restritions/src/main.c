#include <xc.h>
// PIC18F4520 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1H
#pragma config OSC = INTIO67      // Oscillator Selection bits (External RC oscillator, port function on RA6)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Controle do motor de passo
// RB0-RB3 controle das bobinas
// RD0 e RD1 botões para controle
// RC0-RC1 
#define _XTAL_FREQ 8000000
const unsigned char steps_cw[4] = {0b0001, 0b0100, 0b0010, 0b1000};
const unsigned char steps_ccw[4] = {0b1000, 0b0010, 0b0100, 0b0001};
int position = 0;             // posição atual do motor (0 = meio)
unsigned int maxSteps = 0;    // número total de passos entre os dois fins de curso
int rightSteps = 0;
int leftSteps = 0;


int checkBtn() {
    if (PORTDbits.RD0 != PORTDbits. RD1) { // Checa se um dos botões foi apertado
        if (!PORTDbits.RD0) { // Checa se o RD0 foi apertado
            return 1;
        } else { // Checa se o RD1 foi apertado
            return 2;
        }
    }
    else { // Os dois ou nenhum botão apertado
        return 0;
    }
}

void rotateMotor(int direction) {
    if (direction == 1) { // Revisar para ver se as bobinas estão na ordem correta
        // Sentido horário
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps_cw[i]; // Atualiza RB0-RB3
            __delay_ms(50);  // Delay entre passos
        }
    } 
    else if (direction == 2) {  // Revisar para ver se as bobinas estão na ordem correta
        // Sentido anti-horário
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps_ccw[i]; // Atualiza RB0-RB3
            __delay_ms(50); // Delay entre passos
        }
    } 
    else {
        // Nenhum botão ou ambos pressionados  desliga motor
        LATB &= 0xF0;
    }
}

void calibrateMotor() {
    rightSteps = 0;
    leftSteps = 0;
    position = 0;

    while (!PORTCbits.RC0) {
        rotateMotor(1);
        rightSteps++;
    }
    for (int i = 0; i < rightSteps; i++) {
        rotateMotor(2);  // Gira para esquerda
        __delay_ms(10);
    }
    while (!PORTCbits.RC1) {
        rotateMotor(2);
        leftSteps--;
    }
    for (int i = leftSteps; i < 0; i++) {
        rotateMotor(1);  // Gira para direita
        __delay_ms(10);
    }
    position = 0;  // Está no centro agora
}

void main(void) {
    ADCON1 = 0x0F;        // Tudo digital

    TRISB &= ~0x0F;       // RB0-RB3 como saída
    TRISC |= 0x03;        // RC0-RC1 como entrada
    TRISD |= 0x03;        // RD0-RD1 como entrada

    calibrateMotor();

    int direction = 0;

    while (1) {
       __delay_ms(20);
        direction = checkBtn();

        // Girar para a direita
        if (direction == 1 && position < rightSteps) {
            rotateMotor(1);
            position++;
        }
        // Girar para a esquerda
        else if (direction == 2 && position > leftSteps) {
            rotateMotor(2);
            position--;
        }
       __delay_ms(20);
    }
}
