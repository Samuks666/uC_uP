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
#pragma config PWRT = ON         // Power-up Timer habilitado (estabiliza a alimentação na partida)
#pragma config BOR = ON          // Brown-out Reset habilitado (reseta o PIC se a tensão cair)
#pragma config BORV = 3          // Nível de Brown-out Reset em 2.1V
#pragma config VREGEN = ON       // Regulador de tensão interno do USB habilitado (essencial se usar USB)

// CONFIG2H
#pragma config WDT = OFF         // Watchdog Timer DESLIGADO (bom para depuração)

// CONFIG3H
#pragma config MCLRE = ON        // Pino de Master Clear Reset HABILITADO
#pragma config LPT1OSC = OFF     // Timer1 Low-power Oscillator DESLIGADO
#pragma config PBADEN = OFF      // Pinos do PORTB<4:0> configurados como I/O digital na inicialização

// CONFIG4L
#pragma config STVREN = ON       // Reset por estouro de pilha (Stack) HABILITADO
#pragma config LVP = OFF         // Low-Voltage Programming DESLIGADO (essencial para usar RB5 como I/O)
#pragma config ICPRT = OFF       // In-Circuit Debug/Programming Port desabilitado
#pragma config XINST = OFF       // Instruções estendidas (formato C18) DESLIGADO
#pragma config DEBUG = OFF       // Modo de depuração em background DESLIGADO

#define _XTAL_FREQ 48000000UL    // Definição da frequência do clock para as funções de delay.
                                 // ESTE VALOR PRECISA SER IGUAL AO CLOCK DA CPU DEFINIDO NOS BITS DE CONFIGURAÇÃO

void LCD_PutTemperature(unsigned int num_mili, char *buffer);
unsigned int AD_Read(void);

// -----------------------------------------------------------------
// --- Rotinas de Delay para o LCD
// -----------------------------------------------------------------
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
// -----------------------------------------------------------------

void main(void) {
    unsigned int ad_value; 
    unsigned int temp_mili;
    char buffer[10];

    // 1. Configuração do Módulo A/D (ADC)
    TRISAbits.TRISA0 = 1;      // RA0 como entrada digital/analógica
    ADCON1 = 0b00001110;       // Vref- = VSS, Vref+ = VDD. AN0 analógico
    ADCON2 = 0b10010110;       // Right Justified, 12 Tad, Fosc/64
    ADCON0 = 0b00000001;       // AN0 selecionado, ADON = 1

    // 2. Configuração de I/O para o LCD
    TRISD = 0x00; 
    TRISEbits.TRISE0 = 0; 
    TRISEbits.TRISE1 = 0; 
    TRISEbits.TRISE2 = 0; 
    

    // 3. Inicialização do LCD
    OpenXLCD(EIGHT_BIT & LINES_5X7);
    while(BusyXLCD());
    WriteCmdXLCD(0x01);       
    DelayPORXLCD();

    SetDDRamAddr(0x00);       // garante começo da 1ª linha
    putrsXLCD("Temp. LM35: ");

    SetDDRamAddr(0x45);       // linha 2, coluna 4
    WriteDataXLCD(0xDF);
    //WriteDataXLCD(0b11011111); // Se o simbolo não funcionar
    putsXLCD("C ");
    
    while(1)
    {
        // 4. Leitura do ADC
        ad_value = AD_Read(); 

        // 5. Conversão para Temperatura em ºC * 10
        temp_mili = (unsigned int)(((unsigned long)ad_value * 5000UL) / 1023UL);

        // 6. Exibição no LCD
        while(BusyXLCD());
        SetDDRamAddr(0x40); // Segunda linha
        LCD_PutTemperature(temp_mili, buffer);  // Escreve valor formatado
        
        //Muda ponteiro para melhor leitura do LCD
        SetDDRamAddr(0x47);
        WriteDataXLCD(0);
        __delay_ms(200);
    }

    
}

// Leitura do ADC
unsigned int AD_Read(void){
    __delay_us(20);
    ADCON0bits.GO_DONE = 1; 
    while(ADCON0bits.GO_DONE); 
    return ((ADRESH << 8) + ADRESL);
};

// Função para formatar e exibir um número inteiro com ponto decimal no LCD
void LCD_PutTemperature(unsigned int num_mili, char *buffer) {
    unsigned int parte_inteira = num_mili / 10;
    unsigned int parte_decimal = num_mili % 10;
    sprintf(buffer, "%3u.%u", parte_inteira, parte_decimal);
    putsXLCD(buffer);
}
