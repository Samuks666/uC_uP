#include <xc.h>

void delay_200ms();
void delay_500ms();
char HexaValue;
extern volatile int tempo_1s;
extern volatile int tempo_200ms;

void interrupt high_priority hp_intrp();
void interrupt low_priority lp_intrp();

//Exercicio 1.3:

void interrupt high_priority hp_intrp(){
    INTCON3bits.INT1IF = 0;  
    //RD2 = LATD3 Como identificar de interrupcao
    LATDbits.LATD3 = 1;    
    tempo_1s = 1;
    tempo_200ms = 0;
    
}
 
void interrupt low_priority lp_intrp(){
    INTCON3bits.INT2IF = 0;  
    //RD2 = LATD3 Como identificar de interrupcao
    LATDbits.LATD3 = 0;
    tempo_1s = 0;
    tempo_200ms = 1;
}


//Exercicio 1.2:
// Nao esquecer de definir RCONbits.IPEN = 0; 
/*
void __interrupt intrp(void){
    //RB1
    if(INTCON3bits.INT1IF == 1){
        INTCON3bits.INT1IF = 0;   
        for(int i = 0; i < 3; i++) {
            LATD = 0b10101010;  
            delay_500ms();
            LATD = 0b01010101;  
            delay_500ms();
        }
    }
    
    //RB2
    if(INTCON3bits.INT2IF == 1){
        INTCON3bits.INT2IF = 0;    

        for(int j = 0; j < 3; j++) {   
            HexaValue = 0x80;

            for(int i = 0; i < 7; i++) {
                HexaValue = HexaValue >> 1;
                LATD = ~HexaValue;
                delay_200ms();
            }

            for(int k = 0; k < 7; k++) {
                HexaValue = HexaValue << 1;
                LATD = ~HexaValue;
                delay_200ms();
            }       
        }
    }   
}
*/

//Exercicio 1.1:
/*
void interrupt high_priority hp_intrp();
void interrupt low_priority lp_intrp();

void interrupt high_priority hp_intrp(){
        INTCON3bits.INT1IF = 0;  
        for(int i = 0; i < 3; i++) {
            LATD = 0b10101010;  
            delay_500ms();
            LATD = 0b01010101;  
            delay_500ms();
        }
}
 
    //A nova logica para a baixa interrupcao eh utilizando uma variavel dummy, 
    //ela começa com valor 80 em hexadecimal que representa 10000000 em binario
    //nessa variavel tu vai fazendo a troca de bits, e depois joga o valor dessa variavel invertido dentro do LATD
    //pq os bits de LED acendem com logica inversa      

void interrupt low_priority lp_intrp(){
    INTCON3bits.INT2IF = 0;  
     
    for(int j = 0; j < 3; j++) {   
        HexaValue = 0x80;

        for(int i = 0; i < 7; i++) {
            HexaValue = HexaValue >> 1;
            LATD = ~HexaValue;
            delay_200ms();
        }
        
        for(int k = 0; k < 7; k++) {
            HexaValue = HexaValue << 1;
            LATD = ~HexaValue;
            delay_200ms();
        }       
    }
}
*/

//Exercicio 1.1 Extra baixa interrupcao (logica simples)
/*
void interrupt low_priority lp_intrp(){
    INTCON3bits.INT2IF = 0;   
    for(int j = 0; j < 3; j++) { 
 
            //Esquerda para direta       
            LATD = 0b10000000;
            delay_200ms();
            LATD = 0b11000000;
            delay_200ms();
            LATD = 0b11100000;  
            delay_200ms();
            LATD = 0b11110000;  
            delay_200ms();
            LATD = 0b11111000;
            delay_200ms();
            LATD = 0b11111100;
            delay_200ms();
            LATD = 0b11111110;
            delay_200ms();
            LATD = 0b11111111;  
            delay_200ms();
        
            //Direita para esquerda
            LATD = 0b00000001;
            delay_200ms();
            LATD = 0b00000011;  
            delay_200ms();
            LATD = 0b00000111;
            delay_200ms();
            LATD = 0b00001111;
            delay_200ms();
            LATD = 0b00011111;  
            delay_200ms();
            LATD = 0b00111111;
            delay_200ms();
            LATD = 0b00111111;  
            delay_200ms();
            LATD = 0b01111111;  
            delay_200ms();
            LATD = 0b11111111;  
            delay_200ms();       
    }
}
*/

