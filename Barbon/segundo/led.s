; Assembler code for turning an LED on on FRDM-KL25Z board
; 0) Configure project
;    Include startup files and paths
; 1) Determine which GPIO port is connected to the on-board LEDs
; 2) Enable clock signal on PORTx
; 3) Configure PORTx as GPIO
; 4) COnfigure PORTx PINx as output
; 5) Turn on LED on PORTx PINx

; #define SIM_BASE 0x40047000

SIM_BASE              EQU  0x40047000 ; Base address
SIM_SCGC5_OFFSET      EQU  0x1038     ; Offset
SIM_SCGC5             EQU  (SIM_BASE+SIM_SCGC5_OFFSET)
SIM_SCGC5_PORTB_MASK  EQU  0x400

PORTB_PCR19_BASE      EQU  0x4004A000
PORTB_PCR19_OFFSET    EQU  0x4C
PORTB_PCR19_ABSOLUTE  EQU  (PORTB_PCR19_BASE+PORTB_PCR19_OFFSET)
; PORTB_PCR19         EQU  0x4004A04C
PORTB_PCR19_MASK      EQU  0x100

PORTB_BASE            EQU  0x400FF040
PORTB_PDOR_OFFSET     EQU  0x00   ; Port Data Output Register, offset: 0x0
PORTB_PDDR_OFFSET     EQU  0x14
PORTB_PDDR            EQU  (PORTB_BASE+PORTB_PDDR_OFFSET)
; load #1 into r1
; SLL r2, r1,#19
PORTB_PDDR_MASK       EQU  0x00080000
PORTB_PDOR_MASK       EQU  0xFFF7FFFF

; 1) Green led is connected to PTB19

  AREA led_prg,CODE,READONLY
  ENTRY
  EXPORT __main

__main
; 2) Enable clock signal on PORTB
;    SIM_SCGC5 pin 10, set to 1
    LDR r1,=SIM_BASE             ; r1 = SIM_BASE == 0x40047000
    LDR r2,=SIM_SCGC5_OFFSET     ; r2 = 0x1038
    LDR r3,=SIM_SCGC5_PORTB_MASK ; r3 = 0x400
    ; MEM[r1+r2] = r3
    ; MEM[SIM_BASE + SIM_SCGC5_OFFSET] = SIM_SCGC5_PORTB_MASK
    ; MEM[0x40048038] = 0x400
    STR r3,[r1,r2]               ; SIM_SCGC5 = 0x400

; 3) Configure PORTB as GPIO
;    PORTB_PCR19
;    MEM[0x4004_A04C] = 0x100
    LDR r1,=PORTB_PCR19_BASE
    LDR r2,=PORTB_PCR19_OFFSET
    LDR r3,=PORTB_PCR19_MASK
    STR r3,[r1,r2]

; 4) COnfigure PORTB PIN19 as output
    LDR r1,=PORTB_BASE
    LDR r2,=PORTB_PDDR_OFFSET
    LDR r3,=PORTB_PDDR_MASK
    STR r3,[r1,r2]