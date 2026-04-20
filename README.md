# Embedded-MSP430FR2355

This repository contains two embedded applications for the **MSP430FR2355** microcontroller:

- a two-player **reaction time game**
- a UART-controlled **signal generator**

## Repository contents

- `reaction time game.c` – two-player reflex game with UART status output
- `signal generator.c` – waveform generator with UART command interface

## 1) Reaction Time Game

A competitive two-player reflex game:

1. A round is announced over UART.
2. After a random delay, the board LED turns on.
3. Each player presses their button as quickly as possible.
4. The faster player gets a point.
5. First player to reach **5 points** wins.

After the game ends, the score is reset and a new game starts when the players press the buttons again.

### UART output includes

- round announcements
- player reaction times (ms)
- reaction-time difference between players
- current score
- game winner message

## 2) Signal Generator

A UART-controlled waveform generator with selectable waveform slots and runtime parameter configuration.

### Supported waveforms

- sine
- triangle
- ramp
- square
- pulse train

### Configurable parameters

- frequency
- amplitude (%)
- DC offset (mV)
- phase (sample offset)

### Slot model

- **assignment slot**: choose which waveform is stored in slot 0/1/2
- **DAC output slot**: choose which slot is sent to DAC output

### UART commands

- `s` → assign Sine to current assignment slot
- `t` → assign Triangle to current assignment slot
- `r` → assign Ramp to current assignment slot
- `p` → assign Square to current assignment slot
- `c` → assign Pulse train to current assignment slot
- `X`, `Y`, `Z` → select assignment slot 0/1/2
- `L`, `M`, `N` → select DAC output slot 0/1/2
- `F` → set frequency
- `A` → set amplitude
- `O` → set offset
- `H` → set phase
- `#` → print ADC data

## Notes

- UART text and documentation are in English.
- Frequency accuracy/range depends on timer settings and MCU clock configuration in `signal generator.c`.
