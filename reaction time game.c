#pragma PERSISTENT(boot_flag)
unsigned char boot_flag = 0;

#include <msp430.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Function prototype for software-based DCO frequency trim.
void Software_Trim();

// Clock and GPIO configuration constants.
#define MCLK_FREQ_MHZ 2
#define SMCLK_FREQ 2000000
#define LED     BIT0
#define BTN1    BIT3
#define BTN2    BIT1

// Global game state and timing variables (used in ISRs and main loop).
volatile unsigned long start_time = 0;
volatile unsigned long press_time1 = 0;
volatile unsigned long press_time2 = 0;
volatile unsigned int game_active = 0;
volatile unsigned int scores[2] = {0, 0};
volatile unsigned int end_round_requested = 0;
volatile unsigned int timer_overflow_count = 0;
volatile unsigned int round_count = 0;

// Initialize clock system and power/flash wait states.
void clock_init(void) {
    PM5CTL0 &= ~LOCKLPM5;
    WDTCTL = WDTPW | WDTHOLD;
    FRCTL0 = FRCTLPW | NWAITS_2;

    __bis_SR_register(SCG0);
    CSCTL3 = SELREF__REFOCLK;

    // Configure DCO and FLL for the target MCLK/SMCLK frequency.
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_1;
    CSCTL2 = FLLD_0 + 60;
    __delay_cycles(3);
    __bic_SR_register(SCG0);
    Software_Trim();
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}

// Initialize UART on eUSCI_A1.
void uart_init(void) {

    // Route P4.2/P4.3 to UART TX/RX function.
    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &= ~(BIT2 | BIT3);
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL_2;


    // Baud-rate settings for SMCLK at 2 MHz.
    UCA1BR0 = 0x03;
    UCA1BR1 = 0x00;
    UCA1MCTLW = 0x0241;

    UCA1CTLW0 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
}

// Send a null-terminated string through UART.
void uart_send(char *str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }
}

// Random delay between rounds.
void delay_random(void) {
    // Generate a pseudo-random delay between 100 and 999 ms.
    unsigned int delay = (rand() % 900) + 100;
    volatile unsigned long i;
    for(i = 0; i < ((unsigned long)1000 * delay); i++) {
        __no_operation();
    }
}

void reset_game(void) {
    scores[0] = 0;
    scores[1] = 0;
    round_count = 0;
    uart_send("\n\rThe game has ended. Wait and press both buttons simultaneously to start a new game...\n\r");

    // Wait for a button press before allowing a new game start.
    while (1) {
        if (P2IFG & BTN1) {
            P2IFG &= ~BTN1;
            break;
        }
        if (P4IFG & BTN2) {
            P4IFG &= ~BTN2;
            break;
        }
    }

    uart_send("\n\rA new game starts now!\n\r");
}



// Finalize one round: compute winner, print stats, update score.
void end_round(void) {
    P1OUT &= ~LED;
    game_active = 0;
    char buffer[80];

    // Convert timer ticks to milliseconds.
    unsigned long reaction_ms1 = press_time1 / 2000;
    unsigned long reaction_ms2 = press_time2 / 2000;

    // Evaluate winner only when both button presses were captured.
    if (press_time1 > 0 && press_time2 > 0) {
        if (reaction_ms1 < reaction_ms2) {
            scores[0]++;
            sprintf(buffer, "P1 was faster by %ld ms!\n\r", reaction_ms2 - reaction_ms1);
            uart_send(buffer);
        } else if (reaction_ms2 < reaction_ms1) {
            scores[1]++;
            sprintf(buffer, "P2 was faster by %ld ms!\n\r", reaction_ms1 - reaction_ms2);
            uart_send(buffer);
        } else {
            scores[0]++;
            scores[1]++;
            uart_send("Both players reacted equally fast!\n\r");
        }
    } else {
        uart_send("No press was detected for one or both players.\n\r");
    }

    sprintf(buffer, "Time - P1: %lu ms, P2: %lu ms\n\r", reaction_ms1, reaction_ms2);
    uart_send(buffer);
    sprintf(buffer, "Score - P1: %u, P2: %u\n\r", scores[0], scores[1]);
    uart_send(buffer);

    // Check end-of-game condition.
    if (scores[0] >= 5) {
        uart_send("\n\r");
        uart_send("Player 1 wins the game!\n\r");
        reset_game();
        return;
    }
    if (scores[1] >= 5) {
        uart_send("\n\r");
        uart_send("Player 2 wins the game!\n\r");
        reset_game();
        return;
    }

    uart_send("\n\r");
}

// Start a new round after a random wait.
void start_round(void) {
    char buffer[80];
    round_count++;
    sprintf(buffer, "\n\rRound %u starts soon...\n\r", round_count);
    uart_send(buffer);
    delay_random();

    // Reset per-round timing and capture state.
    press_time1 = 0;
    press_time2 = 0;
    timer_overflow_count = 0;
    game_active = 1;
    start_time = TB0R;
    P1OUT |= LED;
}

// Configure Timer_B0 in continuous mode and enable overflow interrupt.
void setup_timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__CONTINUOUS | TBCLR | TBIE;
}

// Configure LED and player buttons with pull-ups and interrupts.
void setup_gpio(void) {
    P1DIR |= LED;
    P1OUT &= ~LED;

    // Configure BTN1 on P2.3.
    P2DIR &= ~BTN1;
    P2REN |= BTN1;
    P2OUT |= BTN1;
    P2IES |= BTN1;
    P2IFG &= ~BTN1;
    P2IE  |= BTN1;

    // Configure BTN2 on P4.1.
    P4DIR &= ~BTN2;
    P4REN |= BTN2;
    P4OUT |= BTN2;
    P4IES |= BTN2;
    P4IFG &= ~BTN2;
    P4IE  |= BTN2;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    __delay_cycles(10000);
    srand(time(NULL));
    clock_init();
    setup_gpio();
    setup_timer();
    uart_init();

    // Print boot message once after power-up.
    if (!boot_flag) {
        uart_send("UART is initialized...\n\r");
        boot_flag = 1;
    }

    __enable_interrupt();

    // Main game loop.
    while (1) {
        if (!game_active) {
            start_round();
            unsigned long round_start_time = TB0R;

            // Wait until both reaction times are captured.
            while (game_active) {
                // Extend 16-bit TB0R timing with software overflow counter.
                unsigned long elapsed_time = ((unsigned long)timer_overflow_count << 16) + TB0R - round_start_time;
                if (press_time1 > 0 && press_time2 > 0) {
                    game_active = 0;
                    end_round_requested = 1;
                }
            }

            // End round when requested by ISR/main logic.
            if (end_round_requested) {
                end_round();
                end_round_requested = 0;
            }
        }
    }
}


// ISR for BTN1 (Port 2).
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
    // Ignore button interrupts when no round is active.
    if (!game_active) {
        P2IFG &= ~BTN1;
        return;
    }

    // Capture player 1 reaction time.
    unsigned long reaction_time = ((unsigned long)timer_overflow_count << 16) + TB0R - start_time;

    if (press_time1 == 0) {
        press_time1 = reaction_time;
    }

    P2IFG &= ~BTN1;

    // Request round end when both players have reacted.
    if (press_time1 > 0 && press_time2 > 0) {
        game_active = 0;
        end_round_requested = 1;
    }
}

// ISR for BTN2 (Port 4).
#pragma vector=PORT4_VECTOR
__interrupt void Port4_ISR(void) {
    // Ignore button interrupts when no round is active.
    if (!game_active) {
        P4IFG &= ~BTN2;
        return;
    }

    // Capture player 2 reaction time.
    unsigned long reaction_time = ((unsigned long)timer_overflow_count << 16) + TB0R - start_time;

    if (press_time2 == 0) {
        press_time2 = reaction_time;
    }

    P4IFG &= ~BTN2;

    // Request round end when both players have reacted.
    if (press_time1 > 0 && press_time2 > 0) {
        game_active = 0;
        end_round_requested = 1;
    }
}


// ISR for Timer_B0 overflow.
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B_ISR(void) {
    if (TB0IV == TB0IV_TBIFG) {
        timer_overflow_count++;
    }
}

// Fine DCO trim helper provided by TI-style clock setup flow.
void Software_Trim(void) {
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do {
        CSCTL0 = 0x100;
        do {
            CSCTL7 &= ~DCOFFG;
        } while (CSCTL7 & DCOFFG);

        __delay_cycles(3000 * MCLK_FREQ_MHZ);
        while ((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;
        csCtl1Read = CSCTL1;

        oldDcoTap = newDcoTap;
        newDcoTap = csCtl0Read & 0x01ff;
        dcoFreqTrim = (csCtl1Read & 0x0070) >> 4;

        if(newDcoTap < 256) {
            newDcoDelta = 256 - newDcoTap;
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256))
                endLoop = 1;
            else {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
            }
        } else {
            newDcoDelta = newDcoTap - 256;
            if(oldDcoTap < 256)
                endLoop = 1;
            else {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
            }
        }

        if(newDcoDelta < bestDcoDelta) {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }
    } while(endLoop == 0);

    // Apply best CSCTL0/CSCTL1 values found by the trim loop.
    CSCTL0 = csCtl0Copy;
    CSCTL1 = csCtl1Copy;
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
}
