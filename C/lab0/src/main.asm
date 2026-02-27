;*******************************************************************************
;                                                                              *
;    Filename: main                                                            *
;    Date: 25/10/2025                                                          *
;    File Version: 0.1                                                         *
;    Author: Augusto e Samuel                                                  *
;                                                                              *
;*******************************************************************************
;                                                                              *
;    Known Issues: 
;                                                                              *
;*******************************************************************************
;                                                                              *
;    Revision History:                                                         *
;    25/10/2025 - Criacao                                                      *
;                                                                              *
;*******************************************************************************
;*******************************************************************************
; PIC18F4550 Configuration Bit Settings
;*******************************************************************************
#include "p18f4550.inc"

; CONFIG1L
  CONFIG  PLLDIV = 1            ; PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
  CONFIG  CPUDIV = OSC1_PLL2    ; System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
  CONFIG  USBDIV = 1            ; USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

; CONFIG1H
  CONFIG  FOSC = INTOSCIO_EC    ; Oscillator Selection bits (XT oscillator (XT))
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = ON             ; Power-up Timer Enable bit (PWRT enabled)
  CONFIG  BOR = OFF             ; Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
  CONFIG  BORV = 0              ; Brown-out Reset Voltage bits (Maximum setting 4.59V)
  CONFIG  VREGEN = OFF          ; USB Voltage Regulator Enable bit (USB voltage regulator disabled)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 1             ; Watchdog Timer Postscale Select bits (1:1)

; CONFIG3H
  CONFIG  CCP2MX = OFF          ; CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
  CONFIG  PBADEN = OFF          ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = OFF           ; MCLR Pin Enable bit (RE3 input pin enabled; MCLR pin disabled)

; CONFIG4L
  CONFIG  STVREN = OFF          ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  ICPRT = OFF           ; Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = ON              ; Code Protection bit (Block 0 (000800-001FFFh) is code-protected)
  CONFIG  CP1 = ON              ; Code Protection bit (Block 1 (002000-003FFFh) is code-protected)
  CONFIG  CP2 = ON              ; Code Protection bit (Block 2 (004000-005FFFh) is code-protected)
  CONFIG  CP3 = ON              ; Code Protection bit (Block 3 (006000-007FFFh) is code-protected)

; CONFIG5H
  CONFIG  CPB = ON              ; Boot Block Code Protection bit (Boot block (000000-0007FFh) is code-protected)
  CONFIG  CPD = ON              ; Data EEPROM Code Protection bit (Data EEPROM is code-protected)

; CONFIG6L
  CONFIG  WRT0 = ON             ; Write Protection bit (Block 0 (000800-001FFFh) is write-protected)
  CONFIG  WRT1 = ON             ; Write Protection bit (Block 1 (002000-003FFFh) is write-protected)
  CONFIG  WRT2 = ON             ; Write Protection bit (Block 2 (004000-005FFFh) is write-protected)
  CONFIG  WRT3 = ON             ; Write Protection bit (Block 3 (006000-007FFFh) is write-protected)

; CONFIG6H
  CONFIG  WRTC = ON             ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are write-protected)
  CONFIG  WRTB = ON             ; Boot Block Write Protection bit (Boot block (000000-0007FFh) is write-protected)
  CONFIG  WRTD = ON             ; Data EEPROM Write Protection bit (Data EEPROM is write-protected)

; CONFIG7L
  CONFIG  EBTR0 = ON            ; Table Read Protection bit (Block 0 (000800-001FFFh) is protected from table reads executed in other blocks)
  CONFIG  EBTR1 = ON            ; Table Read Protection bit (Block 1 (002000-003FFFh) is protected from table reads executed in other blocks)
  CONFIG  EBTR2 = ON            ; Table Read Protection bit (Block 2 (004000-005FFFh) is protected from table reads executed in other blocks)
  CONFIG  EBTR3 = ON            ; Table Read Protection bit (Block 3 (006000-007FFFh) is protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = ON            ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is protected from table reads executed in other blocks)

CBLOCK 0x80
    digit_counter
    pattern_temp
    div_temp        
    div_tens        
ENDC
  
RES_VECT  CODE    0x0000
    GOTO    inicio
    
inicio:
    ; Coloca INTOSC em 8 MHz
    MOVLW   0x72    ; 0111 0010 = 8 MHz INTOSC
    MOVWF   OSCCON
    ; Entradas analogicas desabilitadas (Todas Digitais)
    MOVLW   0x0F
    MOVWF   ADCON1
    
    CLRF    TRISB
    BSF     TRISB, 0        ; RB0 como entrada (incremento)
    BSF     TRISB, 1        ; RB1 como entrada (decremento)
    BCF     INTCON2, 7      ; Pull-up
    CLRF    TRISD           ; PORTD como saida
    CLRF    TRISC           ; PORTC como saida
    
    CLRF    PORTD
    CLRF    PORTC
    CLRF    digit_counter   ; Inicia em 0
    CALL    UpdateDisplays
    
MainLoop:
    BTFSC   PORTB, 0
    GOTO    CheckDecrement      ; Se incremento nao esta pressionado
    
    INCF    digit_counter, F    ; Incrementa contador 
    
    MOVF    digit_counter, W
    XORLW   0x64                ; Compara o valor com 100 (0x64 = 100 em decimal)
    BTFSC   STATUS, Z   
    CLRF    digit_counter       ; Se chegou a 100, zera o contador 
    
    CALL    UpdateDisplays
    
WaitReleaseInc:
    BTFSS   PORTB, 0
    GOTO    WaitReleaseInc
    GOTO    MainLoop
    
CheckDecrement:
    BTFSC   PORTB, 1
    GOTO    MainLoop
    
    MOVF    digit_counter, W
    BTFSC   STATUS, Z
    GOTO    SetTo99
    
    DECF    digit_counter, F
    GOTO    DecrementDone
    
SetTo99:
    MOVLW   0x63              ; 99 em hexadecimal
    MOVWF   digit_counter
    
DecrementDone:
    CALL    UpdateDisplays
    
WaitReleaseDec:
    BTFSS   PORTB, 1
    GOTO    WaitReleaseDec
    GOTO    MainLoop
    
; Divide digit_counter por 10 para obter dezena e unidade
UpdateDisplays:
    MOVF    digit_counter, W
    MOVWF   div_temp          ; Temp = numero original
    CLRF    div_tens          ; Contador de dezenas
    
DivideLoop:
    MOVF    div_temp, W
    SUBLW   0x09              ; Compara com 9
    BTFSC   STATUS, C         ; Se <= 9, terminou
    GOTO    DivideDone
    
    MOVLW   0x0A              ; Subtrai 10
    SUBWF   div_temp, F
    INCF    div_tens, F       ; Incrementa dezena
    GOTO    DivideLoop
    
DivideDone:
    MOVF    div_temp, W       ; Unidade
    CALL    GetPattern
    MOVWF   PORTC
    
    MOVF    div_tens, W       ; Dezena
    CALL    GetPattern
    MOVWF   PORTD
    
    RETURN
            
; Compara o valor da variavel com um valor numerico, se for igual retorna o valor para mostrar no display
GetPattern:
    MOVWF   pattern_temp
    
    MOVF    pattern_temp, W
    BTFSC   STATUS, Z
    RETLW   0x3F    ; 0
    
    MOVF    pattern_temp, W
    XORLW   0x01
    BTFSC   STATUS, Z
    RETLW   0x06    ; 1
    
    MOVF    pattern_temp, W
    XORLW   0x02
    BTFSC   STATUS, Z
    RETLW   0x5B    ; 2
    
    MOVF    pattern_temp, W
    XORLW   0x03
    BTFSC   STATUS, Z
    RETLW   0x4F    ; 3
    
    MOVF    pattern_temp, W
    XORLW   0x04
    BTFSC   STATUS, Z
    RETLW   0x66    ; 4
    
    MOVF    pattern_temp, W
    XORLW   0x05
    BTFSC   STATUS, Z
    RETLW   0x6D    ; 5
    
    MOVF    pattern_temp, W
    XORLW   0x06
    BTFSC   STATUS, Z
    RETLW   0x7D    ; 6
    
    MOVF    pattern_temp, W
    XORLW   0x07
    BTFSC   STATUS, Z
    RETLW   0x07    ; 7
    
    MOVF    pattern_temp, W
    XORLW   0x08
    BTFSC   STATUS, Z
    RETLW   0x7F    ; 8
    
    MOVF    pattern_temp, W
    XORLW   0x09
    BTFSC   STATUS, Z
    RETLW   0x6F    ; 9
    
    RETLW   0x3F    ; Default
END