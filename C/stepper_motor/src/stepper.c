/*
 * File:   stepper.c
 * Author: lab-l
 *
 * Created on 9 de Setembro de 2025, 10:12
 */


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
#define _XTAL_FREQ 8000000

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
        const unsigned char steps[4] = {0b0001, 0b0100, 0b0010, 0b1000};
        // Sentido horário
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps[i]; // Atualiza RB0-RB3
            __delay_ms(50);  // Delay entre passos
        }
    } 
    else if (direction == 2) {  // Revisar para ver se as bobinas estão na ordem correta
        const unsigned char steps[4] = {0b1000, 0b0010, 0b0100, 0b0001};
        // Sentido anti-horário
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps[i]; // Atualiza RB0-RB3
            __delay_ms(50); // Delay entre passos
        }
    } 
    else {
        // Nenhum botão ou ambos pressionados  desliga motor
        LATB &= 0xF0;
    }
}

void main(void) {
    // Desabilita as entradas analógicas (todas as portas como digitais)
    ADCON1 = 0x0F;  // Configura todas as portas como digitais
    
    TRISB &= ~0x0F;         // Configura RB0, RB1, RB2, RB3 como Saídas
    TRISD |=0x03;           // Configura RD0, RD1 como saídas
    
    int direction = 0;
    
    while(1) {
        __delay_ms(20); // Delay (Talvez precise de um módulo debounce)
        direction = checkBtn(); // Checa Qual botão foi pressionado
        rotateMotor(direction); // Rotaciona o motor na direção correta
        __delay_ms(20); // Delay para melhorar leitura dos botões (sem ruídos)
    }
    return;
}
