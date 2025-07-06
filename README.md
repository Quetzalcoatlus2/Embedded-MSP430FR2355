# Embedded-MSP430FR2355

# Reaction time game

Reflex testing is done through a competitive two-player game. At random time intervals, an LED will light up on the board. The player who presses their button faster after the LED turns on wins the round and receives a point. After a round ends, another begins after a different random time interval. The game continues until one of the players reaches 5 points, at which point that player wins the game. After a game ends, a new game can start with the scores reset if both buttons are pressed simultaneously.

The following information is displayed over UART:

A message announcing the upcoming round, including the round number

Each player’s reaction times

The time difference showing how much faster the quicker player was than the slower player

The score of each player

A game-over message highlighting the winner and the start of a new game


# Signal generator

The signal generator allows configuration of signal parameters via UART.

You can select waveform types: sinusoidal, triangular, ramp, square, and pulse train.

The frequency is stable within the range of 1 to 344 Hz.

The amplitude is configured as a percentage (100% equals 3.3V).

The DC component is configured in millivolts (1–3300 mV).

The phase is configurable as a number of samples (1–300), since 300 samples are used to generate one signal period.

There are three waveform slots available for saving waveform configurations.
