;*******************************************************************************
; Projeto: Contador com seleção (00-99)
; Autor: Augusto H.S.M e Samuel A.O.R
; Data: 30/10/2025
; INC = RB0 / DEC = RB1 / ENTER = RB2 / PULSE = RB3 / LED = RC7
; Unidades = PORTC(0..6) / Dezenas = PORTD(0..6)
;*******************************************************************************
;                                                                              *
;    Known Issues: "Funciona"
;                                                                              *
;*******************************************************************************
;                                                                              *
;    Revision History:                                                         *
;    25/10/2025 - Criacao                                                      *
;                                                                              *
;*******************************************************************************
    INCLUDE "p18f4550.inc"
    CONFIG  FOSC = INTOSCIO_EC, WDT = OFF, LVP = OFF, PBADEN = OFF, MCLRE = OFF
;========================
; Variáveis (RAM) - GPR 0x20
;========================
    CBLOCK 0x20
        val_selecionado
        contador_atual
        div_temp
        div_tens
        pattern_temp
        estado         ; 0=CONFIG,1=WAIT_PULSE,2=COUNTING
        debounce_cnt
        tmp_reg
    ENDC

;========================
; Vetor Reset
;========================
    ORG 0x0000
    GOTO inicio

;========================
; Inicio
;========================
inicio:
    MOVLW   0x72
    MOVWF   OSCCON
    MOVLW   0x0F
    MOVWF   ADCON1

    CLRF    TRISC
    CLRF    TRISD

    BSF     TRISB,0
    BSF     TRISB,1
    BSF     TRISB,2
    BSF     TRISB,3

    BCF     INTCON2,7    ; pull-ups PORTB habilitados

    CLRF    PORTC
    CLRF    LATC
    CLRF    PORTD
    CLRF    val_selecionado
    CLRF    contador_atual
    CLRF    estado
    CLRF    tmp_reg

    CALL    PutZeroOnDisplays

;========================
; Loop Principal
;========================
MainLoop:
    MOVF    estado,W
    BTFSC   STATUS,Z
    GOTO    State_Config
    MOVLW   1
    XORWF   estado,W
    BTFSC   STATUS,Z
    GOTO    State_WaitPulse
    GOTO    State_Counting

;========================
; Estado 0 - CONFIG (seleção do valor)
;========================
State_Config:
    BCF     LATC,7
    CALL    UpdateDisplays

    BTFSC   PORTB,0
    GOTO    SkipInc
    CALL    DebounceCheckRB0
SkipInc:

    BTFSC   PORTB,1
    GOTO    SkipDec
    CALL    DebounceCheckRB1
SkipDec:

    BTFSC   PORTB,2
    GOTO    SkipEnter
    CALL    DebounceCheckRB2
SkipEnter:

    GOTO    MainLoop

;========================
; Estado 1 - WAIT_PULSE
;========================
State_WaitPulse:
    BCF     LATC,7
    CALL    PutZeroOnDisplays

    BTFSC   PORTB,3
    GOTO    SkipPulseWP
    CALL    DebounceCheckRB3
SkipPulseWP:
    GOTO    MainLoop

;========================
; Estado 2 - COUNTING
;========================
State_Counting:
    ; Incrementa contador
    INCF    contador_atual,F
    CALL    UpdateDisplays

    ; Verifica se chegou no valor selecionado
    MOVF    contador_atual,W
    XORWF   val_selecionado,W
    BTFSS   STATUS,Z
        GOTO    CountDelay  ; ainda não chegou

    ; Chegou no valor, acende LED
    BSF     LATC,7

    ; Delay maior para LED visível
    MOVLW   0xFF
    MOVWF   debounce_cnt
    MOVLW   0xFF       ; 0xC8 = 200 loops, LED perceptível
    MOVWF   tmp_reg

LEDDelayOuter:
LEDDelayInner:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    LEDDelayInner
    DECFSZ  tmp_reg,F
    GOTO    LEDDelayOuter

    ; Apaga LED
    BCF     LATC,7

    ; Unidades = 0x3F
    MOVLW   0x3F
    MOVWF   LATC
    ; Mantém bit 7 do LED apagado
    BCF     LATC,7

    ; Dezenas = 0x3F
    MOVLW   0x3F
    MOVWF   PORTD

    ; Volta para seleção
    CLRF    estado
    GOTO    MainLoop

CountDelay:
    ; atraso visível da contagem
    MOVLW   0xFF
    MOVWF   debounce_cnt
    MOVLW   0x64
    MOVWF   tmp_reg

DelayOuter:
DelayInner:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    DelayInner
    DECFSZ  tmp_reg,F
    GOTO    DelayOuter

    GOTO    MainLoop

;========================
; Debounce / ações de botões
;========================
DebounceCheckRB0:      ; INC circular
    MOVLW   50
    MOVWF   debounce_cnt
DCB0_1:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    DCB0_1
    BTFSC   PORTB,0
    RETURN

    MOVF    val_selecionado,W
    ADDLW   1
    MOVWF   val_selecionado
    MOVF    val_selecionado,W
    SUBLW   100
    BTFSC   STATUS,Z
        CLRF    val_selecionado

    CALL    UpdateDisplays

WaitRelInc:
    BTFSC   PORTB,0
    RETURN
    GOTO    WaitRelInc

DebounceCheckRB1:      ; DEC circular
    MOVLW   50
    MOVWF   debounce_cnt
DCB1_1:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    DCB1_1
    BTFSC   PORTB,1
    RETURN

    MOVF    val_selecionado,W
    BTFSC   STATUS,Z
        GOTO    Set99FromDec
    DECF    val_selecionado,F
    GOTO    UpdateDecDone

Set99FromDec:
    MOVLW   99
    MOVWF   val_selecionado

UpdateDecDone:
    CALL    UpdateDisplays

WaitRelDec:
    BTFSC   PORTB,1
    RETURN
    GOTO    WaitRelDec

DebounceCheckRB2:      ; ENTER
    MOVLW   50
    MOVWF   debounce_cnt
DCB2_1:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    DCB2_1
    BTFSC   PORTB,2
    RETURN

    CLRF    contador_atual
    BCF     LATC,7
    CALL    PutZeroOnDisplays
    MOVLW   1
    MOVWF   estado

WaitRelEnter:
    BTFSC   PORTB,2
    RETURN
    GOTO    WaitRelEnter

DebounceCheckRB3:      ; PULSE
    MOVLW   50
    MOVWF   debounce_cnt
DCB3_1:
    NOP
    DECFSZ  debounce_cnt,F
    GOTO    DCB3_1
    BTFSC   PORTB,3
    RETURN

    CLRF    contador_atual
    CALL    UpdateDisplays
    MOVLW   2
    MOVWF   estado

WaitRelPulse:
    BTFSC   PORTB,3
    RETURN
    GOTO    WaitRelPulse

;========================
; UpdateDisplays
;========================
UpdateDisplays:
    MOVF    estado,W
    XORLW   2
    BTFSC   STATUS,Z
    GOTO    UseContador
    MOVF    val_selecionado,W
    GOTO    DoDivide
UseContador:
    MOVF    contador_atual,W

DoDivide:
    MOVWF   div_temp
    CLRF    div_tens

DivLoop:
    MOVF    div_temp,W
    SUBLW   0x09
    BTFSC   STATUS,C
    GOTO    DivDone
    MOVF    div_temp,W
    ADDLW   0xF6
    MOVWF   div_temp
    INCF    div_tens,F
    GOTO    DivLoop

DivDone:
    MOVF    div_temp,W
    CALL    GetPattern
    MOVWF   tmp_reg
    ANDLW   0x7F
    MOVWF   tmp_reg
    MOVF    LATC,W
    ANDLW   0x80
    IORWF   tmp_reg,W
    MOVWF   LATC

    MOVF    div_tens,W
    CALL    GetPattern
    MOVWF   PORTD
    RETURN

;========================
;PutZeroOnDisplays
;========================
PutZeroOnDisplays:
    ; Unidades = 0x3F
    MOVLW   0x3F
    MOVWF   LATC
    ; Mantém bit 7 do LED apagado
    BCF     LATC,7

    ; Dezenas = 0x3F
    MOVLW   0x3F
    MOVWF   PORTD
    RETURN

;========================
; GetPattern
;========================
GetPattern:
    MOVWF   pattern_temp
    MOVF    pattern_temp,W
    BTFSC   STATUS,Z
    RETLW   0x3F
    MOVF    pattern_temp,W
    XORLW   0x01
    BTFSC   STATUS,Z
    RETLW   0x06
    MOVF    pattern_temp,W
    XORLW   0x02
    BTFSC   STATUS,Z
    RETLW   0x5B
    MOVF    pattern_temp,W
    XORLW   0x03
    BTFSC   STATUS,Z
    RETLW   0x4F
    MOVF    pattern_temp,W
    XORLW   0x04
    BTFSC   STATUS,Z
    RETLW   0x66
    MOVF    pattern_temp,W
    XORLW   0x05
    BTFSC   STATUS,Z
    RETLW   0x6D
    MOVF    pattern_temp,W
    XORLW   0x06
    BTFSC   STATUS,Z
    RETLW   0x7D
    MOVF    pattern_temp,W
    XORLW   0x07
    BTFSC   STATUS,Z
    RETLW   0x07
    MOVF    pattern_temp,W
    XORLW   0x08
    BTFSC   STATUS,Z
    RETLW   0x7F
    MOVF    pattern_temp,W
    XORLW   0x09
    BTFSC   STATUS,Z
    RETLW   0x6F
    RETLW   0x3F

    END
