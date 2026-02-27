#include <xc.h>

#pragma config FOSC = INTOSCIO_EC   // Oscilador interno, RA6/RA7 como I/O
#pragma config WDT = OFF            // Desliga watchdog
#pragma config LVP = OFF            // Desliga programacao em baixa tensao
#pragma config PBADEN = OFF         // PORTB<4:0> como digitais
#pragma config MCLRE = ON           // MCLR habilitado

#define _XTAL_FREQ 8000000
#define right (!PORTDbits.RD0)
#define left (!PORTDbits.RD1)
#define up (!PORTDbits.RD2)
#define down (!PORTDbits.RD3)

// Sequencia de rotacao horario
const unsigned char steps_cw[4] = {0b0001, 0b0100, 0b0010, 0b1000};
// Sequencia de rotacao anti-horario
const unsigned char steps_ccw[4] = {0b1000, 0b0010, 0b0100, 0b0001};

int positionXY = 0;           // posicao atual do motorXY
int positionZ = 0;            // posicao atual do motorZ
int rightSteps = 0;           // numero de passos para a direita
int leftSteps = 0;            // numero de passos para a esquerda
int upSteps = 0;              // numero de passos cima
int downSteps = 0;            // numero de passos baixo

void checkAllBtns (int rtn[2]) {
    if ((up && down) || (right && left)) {
        rtn[0] = 0;
        rtn[1] = 0;
        return;
    }
    
    unsigned char select = (up << 3) | (down << 2) | (left << 1) | right;
    switch (select){
        case 0b0001: // Apenas Direita
            rtn[0] = 0;
            rtn[1] = 1;
            break;
        case 0b0010: // Apenas Esquerda
            rtn[0] = 0;
            rtn[1] = 2;
            break;               
        case 0b0100: // Apenas Baixo
            rtn[0] = 1;
            rtn[1] = 0;
            break;
        case 0b1000: // Apenas Cima
            rtn[0] = 2;
            rtn[1] = 0;
            break;
        case 0b1010: // Cima + Esquerda
            rtn[0] = 2;
            rtn[1] = 2;
            break;
        case 0b1001: // Cima + Direita
            rtn[0] = 2;
            rtn[1] = 1;
            break;
        case 0b0110: // Baixo + Direita
            rtn[0] = 1;
            rtn[1] = 2;
            break;
        case 0b0101: // Baixo + Esquerda
            rtn[0] = 1;
            rtn[1] = 1;
            break;
        default: // qualquer outra combinação
            rtn[0] = 0;
            rtn[1] = 0; 
            break;
    }
}

void rotateMotorXY(int direction) {
    if (direction == 1) {
        // Sentido horario
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps_cw[i]; // Atualiza RB0-RB3
            __delay_ms(50);  // Delay entre passos
        }
    } 
    else if (direction == 2) {
        // Sentido anti-horario
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0xF0) | steps_ccw[i]; // Atualiza RB0-RB3
            __delay_ms(50); // Delay entre passos
        }
    } 
    else {
        LATB &= 0xF0;
    }
}

void rotateMotorZ(int direction) {
    if (direction == 1) {
        // Sentido horario
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0x0F) | (steps_cw[i] << 4); // RB4-RB7
            __delay_ms(50);
        }
    } 
    else if (direction == 2) {
        // Sentido anti-horario
        for (int i = 0; i < 4; i++) {
            LATB = (LATB & 0x0F) | (steps_ccw[i] << 4); // RB4-RB7
            __delay_ms(50);
        }
    } 
    else {
        LATB &= 0x0F; // apaga RB4-RB7
    }
}

void calibrateMotorXY() {
    rightSteps = 0;
    leftSteps = 0;
    positionXY = 0;

    while (!PORTCbits.RC0) {
        rotateMotorXY(1);
        rightSteps++;
    }
    for (int i = 0; i < rightSteps; i++) {
        rotateMotorXY(2);  // Gira para esquerda
        __delay_ms(10);
    }
    while (!PORTCbits.RC1) {
        rotateMotorXY(2);
        leftSteps--;
    }
    for (int i = leftSteps; i < 0; i++) {
        rotateMotorXY(1);  // Gira para direita
        __delay_ms(10);
    }
    positionXY = 0;  // Volta ao posição zero
}

void calibrateMotorZ() {
    upSteps = 0;
    downSteps = 0;
    positionZ = 0;

    while (!PORTCbits.RC4) {
        rotateMotorZ(1);
        downSteps++;
    }
    for (int i = 0; i < downSteps; i++) {
        rotateMotorZ(2);  // Gira para cima
        __delay_ms(10);
    }
    while (!PORTCbits.RC5) {
        rotateMotorZ(2);
        upSteps--;
    }
    for (int i = upSteps; i < 0; i++) {
        rotateMotorZ(1);  // Gira para baixo
        __delay_ms(10);
    }
    positionZ = 0;  // Volta a posição zero
}

void setup(void) {
    ADCON1 = 0x0F;        // Tudo digital
    OSCCON = 0x70;        // IRCF = 111 (8Mhz) | SCS = 0 Interno
    
    // Saidas para motores
    TRISB &= ~0xFF;       // RB0-RB7 como saida
    // Entrada Switchs
    TRISC |= 0x33;        // RC0-RC1 e RC4-RC5 como entrada
    // Entradas botoes
    TRISD |= 0x0F;        // RD0-RD3 como entrada
    // Leds 
    TRISA &= ~0x7;        // RA0-RA2 como saida

    LATAbits.LATA0 = 1;
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    __delay_ms(20);
    calibrateMotorXY();
    LATAbits.LATA0 = 0;
    LATAbits.LATA1 = 1;
    LATAbits.LATA2 = 0;
    __delay_ms(20);
    calibrateMotorZ();
    LATAbits.LATA0 = 1;
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    __delay_ms(20);
}

void main(void) {
    setup();
    int direction[2] = {0};

    while (1) {
       __delay_ms(20);
       checkAllBtns(direction);
        // Movimento XY
        if (direction[1] == 1 && positionXY < rightSteps) {
            rotateMotorXY(1);  // direita
            positionXY++;
        }
        if (direction[1] == 2 && positionXY > leftSteps) {
            rotateMotorXY(2);  // esquerda
            positionXY--;
        }

        // Movimento Z
        if (direction[0] == 1 && positionZ < downSteps) {
            rotateMotorZ(1);   // baixo
            positionZ++;
        }
        if (direction[0] == 2 && positionZ > upSteps) {
            rotateMotorZ(2);   // cima
            positionZ--;
        }
       __delay_ms(20);
    }
}
