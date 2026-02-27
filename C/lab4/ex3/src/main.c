#include <xc.h>
#include <stdio.h>
#include "xlcd.h"

// CONFIG1L
#pragma config PLLDIV = 5        // Divide o clock do cristal por 5 (20 MHz / 5 = 4 MHz)
#pragma config CPUDIV = OSC1_PLL2// Clock do CPU vem do PLL (96MHz) dividido por 2 = 48MHz
#pragma config USBDIV = 2        // Clock do USB vem do PLL (96MHz) dividido por 2 = 48MHz

// CONFIG1H
#pragma config FOSC = HSPLL_HS   // Usa o oscilador HS (High Speed) com o PLL habilitado

// CONFIG2L
#pragma config PWRT = ON         // Power-up Timer habilitado (estabiliza a alimenta??o na partida)
#pragma config BOR = ON          // Brown-out Reset habilitado (reseta o PIC se a tens?o cair)
#pragma config BORV = 3          // N?vel de Brown-out Reset em 2.1V
#pragma config VREGEN = ON       // Regulador de tens?o interno do USB habilitado (essencial se usar USB)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer DESLIGADO (bom para depura??o)

// CONFIG3H
#pragma config MCLRE = ON        // Pino de Master Clear Reset HABILITADO
#pragma config LPT1OSC = OFF     // Timer1 Low-power Oscillator DESLIGADO
#pragma config PBADEN = OFF      // Pinos do PORTB<4:0> configurados como I/O digital na inicializa??o

// CONFIG4L
#pragma config STVREN = ON       // Reset por estouro de pilha (Stack) HABILITADO
#pragma config LVP = OFF         // Low-Voltage Programming DESLIGADO (essencial para usar RB5 como I/O)
#pragma config ICPRT = OFF       // In-Circuit Debug/Programming Port desabilitado
#pragma config XINST = OFF       // Instru??es estendidas (formato C18) DESLIGADO
#pragma config DEBUG = OFF       // Modo de depura??o em background DESLIGADO

#define _XTAL_FREQ 48000000UL    // Defini??o da frequ?ncia do clock para as fun??es de delay.
                                 // ESTE VALOR PRECISA SER IGUAL AO CLOCK DA CPU DEFINIDO NOS BITS DE CONFIGURA??O

// ======= PROTÓTIPOS =======
void LCD_PutTemperature(unsigned int num_mili, char *buffer);
unsigned int AD_Read(void);
void DelayFor18TCY(void);
void DelayPORXLCD(void);
void DelayXLCD(void);

// ======= DELAYS DO LCD =======
void DelayFor18TCY(void)
{
    __delay_us(10);   // atraso de aproximadamente 10 µs
}

void DelayPORXLCD(void)
{
    __delay_ms(8);    // atraso de aproximadamente 8 ms
}

void DelayXLCD(void)
{
    __delay_ms(3);    // atraso de aproximadamente 3 ms
}

// ======= FUNÇÃO PRINCIPAL =======
void main(void) {
    unsigned int ad_value; 
    unsigned int temp_mili;
    char buffer[10];

    // --- CONFIGURA ADC ---
    TRISAbits.TRISA0 = 1;      // RA0 como entrada
    ADCON1 = 0b00001110;       // AN0 analógico, Vref+ = VDD
    ADCON2 = 0b10010110;       // Right justified, 4 Tad, Fosc/64
    ADCON0 = 0b00000001;       // Canal AN0, ADC ligado

    // --- CONFIGURA SAÍDAS DE CONTROLE ---
    TRISCbits.TRISC1 = 0;  // RC1 resistência
    TRISCbits.TRISC2 = 0;  // RC2 ventoinha
    LATCbits.LATC1 = 1;
    LATCbits.LATC2 = 0;

    // --- CONFIGURA LCD ---
    TRISD = 0x00;
    TRISEbits.TRISE0 = 0; 
    TRISEbits.TRISE1 = 0; 
    TRISEbits.TRISE2 = 0;
    
    OpenXLCD(EIGHT_BIT & LINES_5X7);
    while(BusyXLCD());
    WriteCmdXLCD(0x01);
    DelayPORXLCD();
    putrsXLCD("Temp. LM35: ");
    DelayFor18TCY();

    // ====== LOOP PRINCIPAL ======
    while(1)
    {
        // --- LER ADC E CALCULAR TEMPERATURA ---
        ad_value = AD_Read();
        temp_mili = (unsigned int)(((unsigned long)ad_value * 5000UL) / 1023UL);

        // --- MOSTRAR TEMPERATURA ---
        while(BusyXLCD());
        SetDDRamAddr(0x40); // Segunda linha
        LCD_PutTemperature(temp_mili, buffer);
        putrsXLCD(" ");
        WriteDataXLCD(0b11011111);
        putrsXLCD("C");

        // --- CONTROLE DE TEMPERATURA ---
        if (temp_mili < 320) {
            LATCbits.LATC1 = 1;  // Liga resistência
            LATCbits.LATC2 = 0;  // Desliga ventoinha
            
        } else if (temp_mili > 400) {
            LATCbits.LATC1 = 0;  // Desliga resistência
            LATCbits.LATC2 = 1;  // Liga ventoinha
        }
        
        //Muda ponteiro para melhor leitura do LCD
        SetDDRamAddr(0x48);
        WriteDataXLCD(0);
        for (int i = 0; i < 6; i++) {
           for (int j = 0; j < 5; j++) {
           		__delay_ms(10); 
           }
        }
    }
}

// ======= FUNÇÕES AUXILIARES =======

unsigned int AD_Read(void) {
    __delay_us(20);
    ADCON0bits.GO_DONE = 1;
    while(ADCON0bits.GO_DONE);
    return ((ADRESH << 8) + ADRESL);
}

void LCD_PutTemperature(unsigned int num_mili, char *buffer) {
    unsigned int parte_inteira = num_mili / 10;
    unsigned int parte_decimal = num_mili % 10;
    sprintf(buffer, "%3u.%u", parte_inteira, parte_decimal);
    putsXLCD(buffer);
}
