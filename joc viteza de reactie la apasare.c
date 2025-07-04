#pragma PERSISTENT(boot_flag)
unsigned char boot_flag = 0;  // Variabila boot_flag nu va fi reinitializata la soft reset

#include <msp430.h> // MSP430FR2355
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Prototip de functie pentru ajustarea software a frecventei DCO
void Software_Trim();

// Definirea constantelor pentru frecventa si pini
#define MCLK_FREQ_MHZ 2
#define SMCLK_FREQ 2000000  // 2 MHz
#define LED     BIT0    // LED pe P1.0
#define BTN1    BIT3    // P2.3: Butonul 1
#define BTN2    BIT1    // P4.1: Butonul 2

// Variabile globale volatile pentru timpul de start, timpi de apasare etc.
volatile unsigned long start_time = 0;
volatile unsigned long press_time1 = 0;
volatile unsigned long press_time2 = 0;
volatile unsigned int game_active = 0;
volatile unsigned int scores[2] = {0, 0};
volatile unsigned int end_round_requested = 0;
volatile unsigned int timer_overflow_count = 0; // Contor pentru overflow-ul timerului
volatile unsigned int round_count = 0;  // Contor pentru numarul rundelor

// Initializarea ceasului si a sistemului de alimentare
void clock_init(void) {
    PM5CTL0 &= ~LOCKLPM5;  // Deblocheaza pini I/O
    WDTCTL = WDTPW | WDTHOLD;   // Opreste watchdog timer
    FRCTL0 = FRCTLPW | NWAITS_2;

    __bis_SR_register(SCG0);  // Dezactiveaza temporar MCLK
    CSCTL3 = SELREF__REFOCLK; // Seteaza sursa de referinta la REFOCLK
    // Configurare DCO cu ajustare automata
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_1;
    CSCTL2 = FLLD_0 + 60;
    __delay_cycles(3);
    __bic_SR_register(SCG0); // Reactiveaza MCLK
    Software_Trim();          // Ajustare software a DCO
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;
}

// Initializarea UART-ului
void uart_init(void) {
    // Configurarea pinilor P4.2 si P4.3 pentru comunicare UART
    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &= ~(BIT2 | BIT3);
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL_2;  // Selecteaza SMCLK ca sursa de ceas

    // Setarea baud rate-ului la 38400 (ajustare pentru 2MHz SMCLK)
    UCA1BR0 = 0x03;
    UCA1BR1 = 0x00;
    UCA1MCTLW = 0x0241;

    UCA1CTLW0 &= ~UCSWRST;  // Porneste UART-ul
    UCA1IE |= UCRXIE;
}

// Functie pentru trimiterea unui string prin UART
void uart_send(char *str) {
    while (*str) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = *str++;
    }
}

// Functie de intarziere pe baza de numar aleator
void delay_random(void) {
    // Genereaza un numar aleator intre 100 si 999 (ms)
    unsigned int delay = (rand() % 900) + 100;
    volatile unsigned long i;
    for(i = 0; i < ((unsigned long)1000 * delay); i++) {
        __no_operation();
    }
}

void reset_game(void) {
    scores[0] = 0;
    scores[1] = 0;
    round_count = 0;  // Resetăm contorul de runde
    uart_send("\n\rJocul s-a terminat. Asteptati si apasati ambele butoane simultan pentru a incepe un nou joc...\n\r");

    // Așteaptă apăsarea unui buton
    while (1) {
        if (P2IFG & BTN1) {  // Verifică dacă BTN1 (P2.3) este apăsat
            P2IFG &= ~BTN1;  // Curăță flag-ul de întrerupere
            break;  // Ieși din bucla de așteptare
        }
        if (P4IFG & BTN2) {  // Verifică dacă BTN2 (P4.1) este apăsat
            P4IFG &= ~BTN2;  // Curăță flag-ul de întrerupere
            break;  // Ieși din bucla de așteptare
        }
    }

    uart_send("\n\rUn nou joc incepe acum!\n\r");
}



// Functie pentru finalizarea unei runde de joc
void end_round(void) {
    P1OUT &= ~LED; // Stinge LED-ul
    game_active = 0;
    char buffer[80];

    // Calcularea timpilor de reactie in milisecunde (presupunand un scaler de 2000)
    unsigned long reaction_ms1 = press_time1 / 2000;
    unsigned long reaction_ms2 = press_time2 / 2000;

    // Verifica daca ambele apasari au fost detectate
    if (press_time1 > 0 && press_time2 > 0) {
        if (reaction_ms1 < reaction_ms2) {
            scores[0]++;  // Jucatorul 1 a fost mai rapid
            sprintf(buffer, "P1 a fost mai rapid cu %ld ms!\n\r", reaction_ms2 - reaction_ms1);
            uart_send(buffer);
        } else if (reaction_ms2 < reaction_ms1) {
            scores[1]++;  // Jucatorul 2 a fost mai rapid
            sprintf(buffer, "P2 a fost mai rapid cu %ld ms!\n\r", reaction_ms1 - reaction_ms2);
            uart_send(buffer);
        } else {
            scores[0]++;
            scores[1]++;
            uart_send("Ambii jucatori au reactionat la fel de rapid!\n\r");
        }
    } else {
        uart_send("Nu s-a detectat apasare pentru unul sau ambii jucatori.\n\r");
    }

    sprintf(buffer, "Timp - P1: %lu ms, P2: %lu ms\n\r", reaction_ms1, reaction_ms2);
    uart_send(buffer);
    sprintf(buffer, "Scor - P1: %u, P2: %u\n\r", scores[0], scores[1]);
    uart_send(buffer);

    // Verifica daca vreun jucator a atins 10 puncte
    if (scores[0] >= 5) {
        uart_send("\n\r");  // Linie goala inainte de mesaj
        uart_send("Jucatorul 1 castiga jocul!\n\r");
        reset_game();
        return;
    }
    if (scores[1] >= 5) {
        uart_send("\n\r");  // Linie goala inainte de mesaj
        uart_send("Jucatorul 2 castiga jocul!\n\r");
        reset_game();
        return;
    }

    uart_send("\n\r");
}

// Functie pentru inceperea unei noi runde
void start_round(void) {
    char buffer[80];
    round_count++;  // Incrementam contorul de runde
    sprintf(buffer, "\n\rRunda %u incepe curand...\n\r", round_count);
    uart_send(buffer);
    delay_random();

    // Resetarea timpiilor de apasare si contorul de overflow
    press_time1 = 0;
    press_time2 = 0;
    timer_overflow_count = 0;
    game_active = 1;
    start_time = TB0R;
    P1OUT |= LED;  // Aprinde LED-ul pentru a semnala start-ul
}

// Configurarea timerului TB0 pentru modul continuu si activarea intreruperii
void setup_timer(void) {
    TB0CTL = TBSSEL__SMCLK | MC__CONTINUOUS | TBCLR | TBIE; // Sursa SMCLK, mod continuu, reset TB, enable intrerupere la overflow
}

// Configurarea pinilor GPIO pentru LED si butoane
void setup_gpio(void) {
    P1DIR |= LED;   // Configurare LED ca iesire
    P1OUT &= ~LED;  // Asigura ca LED-ul este stins la pornire

    // Configurare buton BTN1 de pe P2.3
    P2DIR &= ~BTN1;   // BTN1 ca intrare
    P2REN |= BTN1;    // Activeaza rezistenta interna
    P2OUT |= BTN1;    // Seteaza rezistenta la pull-up
    P2IES |= BTN1;    // Activeaza intreruperea pe tranzitie descendenta
    P2IFG &= ~BTN1;   // Curata flag-ul de intrerupere
    P2IE  |= BTN1;    // Activeaza intreruperea pentru BTN1

    // Configurare buton BTN2 de pe P4.1
    P4DIR &= ~BTN2;   // BTN2 ca intrare
    P4REN |= BTN2;    // Activeaza rezistenta interna
    P4OUT |= BTN2;    // Seteaza rezistenta la pull-up
    P4IES |= BTN2;    // Activeaza intreruperea pe tranzitie descendenta
    P4IFG &= ~BTN2;   // Curata flag-ul de intrerupere
    P4IE  |= BTN2;    // Activeaza intreruperea pentru BTN2
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    __delay_cycles(10000);
    srand(time(NULL));  // Initializează generatorul de numere aleatoare
    clock_init();       // Inițializează ceasul
    setup_gpio();       // Inițializează pini GPIO
    setup_timer();      // Configurează timerul
    uart_init();        // Inițializează UART-ul

    // Mesajul de start al jocului
    if (!boot_flag) {
        uart_send("UART-ul este initializat...\n\r");
        boot_flag = 1;
    }

    __enable_interrupt();  // Activează întreruperile globale

    // Bucla principală a programului
    while (1) {
        if (!game_active) {
            start_round();  // Pornește o nouă rundă
            unsigned long round_start_time = TB0R;

            // Așteaptă până când ambele apasări sunt înregistrate
            while (game_active) {
                //counter de 16 biti extins la 32 biti ca sa putem contoriza timpi de reactie de maximum 36 de minute, TB0R este timpul curent
                unsigned long elapsed_time = ((unsigned long)timer_overflow_count << 16) + TB0R - round_start_time;
                if (press_time1 > 0 && press_time2 > 0) {
                    game_active = 0;
                    end_round_requested = 1;
                }
            }

            // Dacă s-a solicitat încheierea rundei, se apelează funcția de finalizare
            if (end_round_requested) {
                end_round();
                end_round_requested = 0;
            }
        }
    }
}


// ISR pentru intreruperile pe portul P2 (butonul BTN1)
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void) {
    // Dacă jocul nu este activ, ignoră întreruperea
    if (!game_active) {
        P2IFG &= ~BTN1;
        return;
    }

    // Calcularea timpului de reacție pentru BTN1
    unsigned long reaction_time = ((unsigned long)timer_overflow_count << 16) + TB0R - start_time;

    if (press_time1 == 0) { // Înregistrează timpul doar o singură dată
        press_time1 = reaction_time;
    }

    P2IFG &= ~BTN1;  // Curăță flag-ul de întrerupere

    // Dacă ambele butoane au fost apăsate, se solicită finalul rundei
    if (press_time1 > 0 && press_time2 > 0) {
        game_active = 0;
        end_round_requested = 1;
    }
}

// ISR pentru intreruperile pe portul P4 (butonul BTN2)
#pragma vector=PORT4_VECTOR
__interrupt void Port4_ISR(void) {
    // Dacă jocul nu este activ, ignoră întreruperea
    if (!game_active) {
        P4IFG &= ~BTN2;
        return;
    }

    // Calcularea timpului de reacție pentru BTN2
    unsigned long reaction_time = ((unsigned long)timer_overflow_count << 16) + TB0R - start_time;

    if (press_time2 == 0) { // Înregistrează timpul doar o singură dată
        press_time2 = reaction_time;
    }

    P4IFG &= ~BTN2;  // Curăță flag-ul de întrerupere

    // Dacă ambele butoane au fost apăsate, se solicită finalul rundei
    if (press_time1 > 0 && press_time2 > 0) {
        game_active = 0;
        end_round_requested = 1;
    }
}


// ISR pentru overflow-ul timerului TB0
#pragma vector=TIMER0_B1_VECTOR
__interrupt void Timer_B_ISR(void) {
    if (TB0IV == TB0IV_TBIFG) {
        timer_overflow_count++; // Incrementeaza contorul la fiecare overflow
    }
}

// Functia Software_Trim pentru stabilizarea frecventei DCO
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

    // Se seteaza valorile optime pentru DCO
    CSCTL0 = csCtl0Copy;
    CSCTL1 = csCtl1Copy;
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
}
