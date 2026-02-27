#include <xc.h>

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

unsigned int AD_Read(void);
void delay_50ms(void);

void main(void) {
    // Configuração do Módulo A/D (ADC)
    // ADCON1:
    // Voltage Reference Configuration Bits (00 = Vref- = VSS, Vref+ = VDD)
    // A/D Port Configuration Control Bits (1110 = Only AN0 as analog, outros digitais)
    ADCON1 = 0b00001110;

    // ADCON2:
    // A/D Result Format Select Bit (1 = Right justified) - ADFM
    // A/D Acquisition Time Select Bits (010 = 4 Tad) - ACQT
    // A/D Conversion Clock Select Bits (110 = Fosc/64) - ADCS
    ADCON2 = 0b10101110; // 1 (ADFM) | 010 (ACQT) | 110 (ADCS)

    // ADCON0:
    // Channel Select Bits (0000 = Channel 0 (AN0)) - CHS
    // Habilita o módulo A/D (1 = On) - ADON
    ADCON0 = 0b00000001; // AN0 selecionado e A/D habilitado (ADON = 1)


    // Configuração de I/O
    // Habilita Pull-Up para o PORTB
    INTCON2bits.RBPU = 0;

    // Configura todos os pinos do PORTD como output (saída)
    TRISD = 0x00;
    // Inicializa o PORTD como nível alto
    LATD = 0xFF; // Todos os LEDs desligados

    unsigned int ad_value;
    unsigned char n_leds_on;
    unsigned char led_mask;

    while(1){
        // Leitura do ADC
        ad_value = AD_Read();

        // 1. Lógica de Divisão (n_leds_on será 0 a 7)
        n_leds_on = ad_value >> 7; 

        // Isso garante que o n_leds_on se torne 8, ligando o último LED.
        if (ad_value == 1023) { 
            n_leds_on = 8;
        }

        // 2-3. Cria a máscara e aciona os LEDs
        led_mask = (0x01 << n_leds_on) - 1;
        
        LATD = ~led_mask;

        delay_50ms();
    }
}

// --- Funções Auxiliares ---

unsigned int AD_Read(void){

    // 1. Inicia a conversão (Sinalizar conversão GO/DONE)
    ADCON0bits.GO_DONE = 1;

    // 2. Espera a conversão terminar
    while(ADCON0bits.GO_DONE);

    // 3. Retorna o resultado
    // Como ADCON2bits.ADFM = 1 (Right Justified), o resultado em ADRESH:ADRESL.
    // Registrador de 16 bits ADRES que faz a união automaticamente.
    return ADRES;
    // Se não usasse ADRES (forma manual):
    // return (ADRESH << 8) + ADRESL; // Desloca ADRESH 8 bits para a esquerda e soma com ADRESL
};

void delay_50ms(void){
    for(int i = 0; i < 5; i++){
        __delay_ms(10); // 5 x 10ms = 50ms
    }
}