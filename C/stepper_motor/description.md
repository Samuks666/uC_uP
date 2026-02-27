# LabB_4 - Controle de motor de passo em 1 eixo

Miniprojeto em *C* que tem como objetivo controle em um eixo de um motor de passo por meio de um PIC18F4520,
com interface feita por 2 botões.

## Estrutura de Pastas

- **prj/**  
  Pasta do projeto, incluindo o diretório `Stepper_Motor.X`,
  que  contém os arquivos de firmware e configuração do projeto no MPLABX.

- **sim/**  
  Arquivos de simulação, testar o funcionamento do motor antes da implementação física.
  - `stepper_motor_sim.pdsprj`  
    Projeto de simulação (Proteus).
  - `Project Backups/`  
    Backups de projetos de simulação.

- **src/**  
  Código-fonte em C para o controle do motor de passo.
  - `stepper.c`  
    Implementação de um algoritmo simples de controle para o motor de passo.

## Como Utilizar

1. **Simulação:**  
   Use os arquivos presentes em `sim/` para abrir o projeto de
   simulação no Proteus.
3. **Desenvolvimento:**  
   O código para o microcontrolador está em `src/stepper.c`
   e o projeto do MPLABX no diretório `prj/Stepper_Motor.X`.
   
## Requisitos
- Ferramenta de simulação PROTEUS
- Ambiente de desenvolvimento para microcontrolador MPLABX
- Compilador C compatível (XC8)
