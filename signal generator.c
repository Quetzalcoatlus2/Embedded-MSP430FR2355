










#include <msp430.h>

#include <stdlib.h>

#include <string.h>




void Init_GPIO();




unsigned int sine_wave[300] = {
                           2048, 2090, 2133, 2176, 2219, 2262, 2304, 2347, 2389, 2431, 2473, 2515, 2557, 2598, 2639, 2680, 2721, 2761, 2801, 2841,
                           2880, 2919, 2958, 2996, 3034, 3071, 3108, 3145, 3181, 3216, 3251, 3285, 3319, 3353, 3385, 3418, 3449, 3480, 3510, 3540,
                           3569, 3597, 3625, 3652, 3678, 3704, 3729, 3753, 3776, 3799, 3821, 3842, 3862, 3881, 3900, 3918, 3935, 3951, 3967, 3981,
                           3995, 4008, 4020, 4031, 4041, 4050, 4059, 4066, 4073, 4079, 4084, 4088, 4091, 4093, 4095, 4095, 4095, 4093, 4091, 4088,
                           4084, 4079, 4073, 4066, 4059, 4050, 4041, 4031, 4020, 4008, 3995, 3981, 3967, 3951, 3935, 3918, 3900, 3881, 3862, 3842,
                           3821, 3799, 3776, 3753, 3729, 3704, 3678, 3652, 3625, 3597, 3569, 3540, 3510, 3480, 3449, 3418, 3385, 3353, 3319, 3285,
                           3251, 3216, 3181, 3145, 3108, 3071, 3034, 2996, 2958, 2919, 2880, 2841, 2801, 2761, 2721, 2680, 2639, 2598, 2557, 2515,
                           2473, 2431, 2389, 2347, 2304, 2262, 2219, 2176, 2133, 2090, 2048, 2005, 1962, 1919, 1876, 1833, 1791, 1748, 1706, 1664,
                           1622, 1580, 1538, 1497, 1456, 1415, 1374, 1334, 1294, 1254, 1215, 1176, 1137, 1099, 1061, 1024, 987, 950, 914, 879,
                           844, 810, 776, 742, 710, 677, 646, 615, 585, 555, 526, 498, 470, 443, 417, 391, 366, 342, 319, 296,
                           274, 253, 233, 214, 195, 177, 160, 144, 128, 114, 100, 87, 75, 64, 54, 45, 36, 29, 22, 16,
                           11, 7, 4, 2, 0, 0, 0, 2, 4, 7, 11, 16, 22, 29, 36, 45, 54, 64, 75, 87,
                           100, 114, 128, 144, 160, 177, 195, 214, 233, 253, 274, 296, 319, 342, 366, 391, 417, 443, 470, 498,
                           526, 555, 585, 615, 646, 677, 710, 742, 776, 810, 844, 879, 914, 950, 987, 1024, 1061, 1099, 1137, 1176,
                           1215, 1254, 1294, 1334, 1374, 1415, 1456, 1497, 1538, 1580, 1622, 1664, 1706, 1748, 1791, 1833, 1876, 1919, 1962, 2005};



unsigned int ramp_wave[300] = { 0, 14, 27, 41, 55, 68, 82, 96, 110, 123, 137, 151, 164, 178, 192, 205,
                              219, 233, 246, 260, 274, 287, 301, 315, 329, 342, 356, 370, 383, 397, 411, 424,
                              438, 452, 465, 479, 493, 507, 520, 534, 548, 561, 575, 589, 602, 616, 630, 643,
                              657, 671, 685, 698, 712, 726, 739, 753, 767, 780, 794, 808, 821, 835, 849, 862,
                              876, 890, 904, 917, 931, 945, 958, 972, 986, 999, 1013, 1027, 1040, 1054, 1068, 1082,
                              1095, 1109, 1123, 1136, 1150, 1164, 1177, 1191, 1205, 1218, 1232, 1246, 1259, 1273, 1287, 1301,
                              1314, 1328, 1342, 1355, 1369, 1383, 1396, 1410, 1424, 1437, 1451, 1465, 1479, 1492, 1506, 1520,
                              1533, 1547, 1561, 1574, 1588, 1602, 1615, 1629, 1643, 1656, 1670, 1684, 1698, 1711, 1725, 1739,
                              1752, 1766, 1780, 1793, 1807, 1821, 1834, 1848, 1862, 1876, 1889, 1903, 1917, 1930, 1944, 1958,
                              1971, 1985, 1999, 2012, 2026, 2040, 2054, 2067, 2081, 2095, 2108, 2122, 2136, 2149, 2163, 2177,
                              2190, 2204, 2218, 2231, 2245, 2259, 2273, 2286, 2300, 2314, 2327, 2341, 2355, 2368, 2382, 2396,
                              2409, 2423, 2437, 2451, 2464, 2478, 2492, 2505, 2519, 2533, 2546, 2560, 2574, 2587, 2601, 2615,
                              2628, 2642, 2656, 2670, 2683, 2697, 2711, 2724, 2738, 2752, 2765, 2779, 2793, 2806, 2820, 2834,
                              2848, 2861, 2875, 2889, 2902, 2916, 2930, 2943, 2957, 2971, 2984, 2998, 3012, 3025, 3039, 3053,
                              3067, 3080, 3094, 3108, 3121, 3135, 3149, 3162, 3176, 3190, 3203, 3217, 3231, 3245, 3258, 3272,
                              3286, 3299, 3313, 3327, 3340, 3354, 3368, 3381, 3395, 3409, 3423, 3436, 3450, 3464, 3477, 3491,
                              3505, 3518, 3532, 3546, 3559, 3573, 3587, 3600, 3614, 3628, 3642, 3655, 3669, 3683, 3696, 3710,
                              3724, 3737, 3751, 3765, 3778, 3792, 3806, 3820, 3833, 3847, 3861, 3874, 3888, 3902, 3915, 3929,
                              3943, 3956, 3970, 3984, 3997, 4011, 4025, 4039, 4052, 4066, 4080, 4093
                          };



unsigned int triangle_wave[300] = {0, 27, 55, 82, 110, 137, 164, 192, 219, 246, 274, 301, 329, 356, 383, 411,
    438, 465, 493, 520, 548, 575, 602, 630, 657, 685, 712, 739, 767, 794, 821, 849,
    876, 904, 931, 958, 986, 1013, 1040, 1068, 1095, 1123, 1150, 1177, 1205, 1232, 1259, 1287,
    1314, 1342, 1369, 1396, 1424, 1451, 1479, 1506, 1533, 1561, 1588, 1615, 1643, 1670, 1698, 1725,
    1752, 1780, 1807, 1834, 1862, 1889, 1917, 1944, 1971, 1999, 2026, 2054, 2081, 2108, 2136, 2163,
    2190, 2218, 2245, 2273, 2300, 2327, 2355, 2382, 2409, 2437, 2464, 2492, 2519, 2546, 2574, 2601,
    2628, 2656, 2683, 2711, 2738, 2765, 2793, 2820, 2848, 2875, 2902, 2930, 2957, 2984, 3012, 3039,
    3067, 3094, 3121, 3149, 3176, 3203, 3231, 3258, 3286, 3313, 3340, 3368, 3395, 3423, 3450, 3477,
    3505, 3532, 3559, 3587, 3614, 3642, 3669, 3696, 3724, 3751, 3778, 3806, 3833, 3861, 3888, 3915,
    3943, 3970, 3997, 4025, 4052, 4080, 4095, 4080, 4052, 4025, 3997, 3970, 3943, 3915, 3888, 3861,
    3833, 3806, 3778, 3751, 3724, 3696, 3669, 3642, 3614, 3587, 3559, 3532, 3505, 3477, 3450, 3423,
    3395, 3368, 3340, 3313, 3286, 3258, 3231, 3203, 3176, 3149, 3121, 3094, 3067, 3039, 3012, 2984,
    2957, 2930, 2902, 2875, 2848, 2820, 2793, 2765, 2738, 2711, 2683, 2656, 2628, 2601, 2574, 2546,
    2519, 2492, 2464, 2437, 2409, 2382, 2355, 2327, 2300, 2273, 2245, 2218, 2190, 2163, 2136, 2108,
    2081, 2053, 2026, 1999, 1971, 1944, 1917, 1889, 1862, 1834, 1807, 1780, 1752, 1725, 1698, 1670,
    1643, 1615, 1588, 1561, 1533, 1506, 1479, 1451, 1424, 1396, 1369, 1342, 1314, 1287, 1259, 1232,
    1205, 1177, 1150, 1123, 1095, 1068, 1040, 1013, 986, 958, 931, 904, 876, 849, 821, 794,
    767, 739, 712, 684, 657, 630, 602, 575, 548, 520, 493, 465, 438, 411, 383, 356,
    329, 301, 274, 246, 219, 192, 164, 137, 110, 82, 55, 27
};



unsigned int square_wave[300];


unsigned int pulse_train_wave[300];



unsigned int *waveform_slots[3];
unsigned int current_assignment_slot_index = 0;
unsigned int current_output_slot_index = 0;





char ADC_char[3][12] = {
    {'A','x','=',' ','x','x','x','x',10,13,0},
    {'A','x','=',' ','x','x','x','x',10,13,0},
    {'A','x','=',' ','x','x','x','x',10,13,0}
};

unsigned int i,j;


unsigned int current_waveform_index = 0;


void Software_Trim();
void uart_send_string(const char *str);
void uart_send_uint(unsigned int val);



#define MCLK_FREQ_MHZ 24


unsigned int ADC_Result[3];

unsigned char ADC_i;



unsigned int DAC_data=0;


#define FREQ_INPUT_BUFFER_SIZE 6
char freq_input_buffer[FREQ_INPUT_BUFFER_SIZE];
unsigned char freq_input_buffer_idx = 0;


#define AMPLITUDE_INPUT_BUFFER_SIZE 4
char amplitude_input_buffer[AMPLITUDE_INPUT_BUFFER_SIZE];
unsigned char amplitude_input_buffer_idx = 0;
unsigned int current_amplitude_percentage = 100;



volatile unsigned int current_amplitude_scaling_factor;


#define OFFSET_INPUT_BUFFER_SIZE 5
char offset_input_buffer[OFFSET_INPUT_BUFFER_SIZE];
unsigned char offset_input_buffer_idx = 0;
unsigned int current_offset_mv = 1650;



volatile long precalculated_offset_dac_units;


#define PHASE_INPUT_BUFFER_SIZE 4
char phase_input_buffer[PHASE_INPUT_BUFFER_SIZE];
unsigned char phase_input_buffer_idx = 0;
unsigned int current_phase_offset_samples = 0;





typedef enum {
    NORMAL_COMMAND_MODE,
    FREQUENCY_INPUT_MODE,
    AMPLITUDE_INPUT_MODE,
    OFFSET_INPUT_MODE,
    PHASE_INPUT_MODE
} UART_RX_State;


UART_RX_State uart_rx_state = NORMAL_COMMAND_MODE;







volatile unsigned int timer_b0_ccr0_increment = 666;



int main(void)
{



    WDTCTL = WDTPW | WDTHOLD;




    for(i=0; i<300; i++)
    {
        if(i < 150) square_wave[i] = 4095;
        else square_wave[i] = 0;
    }




    for(i=0; i<300; i++)
    {
        if(i < 3) pulse_train_wave[i] = 4095;
        else pulse_train_wave[i] = 0;
    }



    waveform_slots[0] = sine_wave;
    waveform_slots[1] = sine_wave;
    waveform_slots[2] = sine_wave;


    current_amplitude_percentage = 100;

    current_amplitude_scaling_factor = ((long)current_amplitude_percentage * 256) / 100;
    current_offset_mv = 1650;

    precalculated_offset_dac_units = ((long)current_offset_mv * 4095L) / 3300L;
    current_phase_offset_samples = 0;




    FRCTL0 = FRCTLPW | NWAITS_2;



    __bis_SR_register(SCG0);

    CSCTL3 = SELREF__REFOCLK;



    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_6;




    CSCTL2 = FLLD_0 + 731;

    __delay_cycles(3);

    __bic_SR_register(SCG0);

    Software_Trim();



    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;


    current_waveform_index = 0;





    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;



    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &=~(BIT2 | BIT3);



    UCA1CTLW0 |= UCSWRST;

    UCA1CTLW0 |= UCSSEL_2;



    UCA1BR0 = 13;
    UCA1BR1 = 0x00;




    UCA1MCTLW = (0x44 << 8) | (0 << 4) | UCOS16;



    P3DIR |= BIT0; P3SEL0 |= BIT0; P3SEL1 &= ~BIT0;

    P3DIR |= BIT4; P3SEL0 |= BIT4; P3SEL1 &= ~BIT4;



    P1SEL0 |=  BIT0 + BIT1 + BIT2;
    P1SEL1 |=  BIT0 + BIT1 + BIT2;



    PM5CTL0 &= ~LOCKLPM5;




    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN;

    __delay_cycles(400);


    UCA1CTLW0 &= ~UCSWRST;


    uart_send_string("\r\n--- MSP430 Waveform Generator ---\r\n");
    uart_send_string("Waveform assignment (applies to the selected assignment slot):\r\n");
    uart_send_string(" s : Sine\r\n t : Triangle\r\n r : Ramp\r\n p : Square\r\n c : Pulse\r\n");
    uart_send_string("Select assignment slot:\r\n X:Slot 0, Y:Slot 1, Z:Slot 2\r\n");
    uart_send_string("Select DAC output slot:\r\n L:Slot 0, M:Slot 1, N:Slot 2\r\n");
    uart_send_string("Other commands:\r\n");
    uart_send_string(" F : Set Frequency (1-10000 Hz)\r\n");
    uart_send_string(" A : Set Amplitude (0-100 %)\r\n");
    uart_send_string(" O : Set Offset (0-3300 mV)\r\n");
    uart_send_string(" H : Set Phase (0-299 samples)\r\n");
    uart_send_string(" # : Send ADC Data\r\n");
    uart_send_string("current: Slot 0->0, Freq ~100Hz, Ampl 100%, Offset 1650mV, Phase 0.\r\n");
    uart_send_string("> ");



    UCA1IE |= UCRXIE;




    ADCCTL0 |= ADCSHT_10 | ADCON;



    ADCCTL1 |= ADCSHP | ADCSHS_2 | ADCCONSEQ_3;


    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;



    ADCMCTL0 |= ADCINCH_2;

    ADCIE |= ADCIE0;





    SAC1DAC = DACSREF_0 + DACLSEL_0 + DACIE;

    SAC1DAT = DAC_data;

    SAC1DAC |= DACEN;






    SAC1OA = NMUXEN + PMUXEN + PSEL_1 + NSEL_1;
    SAC1OA |= OAPM_0;


    SAC1PGA = MSEL_1;


    SAC1OA |= SACEN + OAEN;




    TB2CCR0 = timer_b0_ccr0_increment;

    TB2CCR1 = 10;

    TB2CCTL0 = CCIE;

    TB2CCTL1 = CCIE;







    TB2CTL |= TBSSEL__SMCLK | ID_0 | TBCLR | MC__CONTINOUS| TBIE;




    TB1CCR0 = 326;



    TB1CCTL1 = OUTMOD_7;

    TB1CCR1 = 163;




    TB1CTL = TBSSEL__ACLK | MC__UP | TBCLR;


    ADC_i = 2;

    i=0;


    ADCCTL0 |= ADCENC;

    TB1CTL |= TBCLR;



    __bis_SR_register(GIE);



    __no_operation();
}


void uart_send_string(const char *str)
{

    while (*str)
    {


        while (!(UCA1IFG & UCTXIFG));

        UCA1TXBUF = *str++;
    }


    while (UCA1STATW & UCBUSY);
}



void uart_send_uint(unsigned int val) {
    char buffer[6];
    int k = 0;


    if (val == 0) {
        while(!(UCA1IFG&UCTXIFG)); UCA1TXBUF = '0';
        return;
    }

    unsigned int temp = val;

    do {
        buffer[k++] = (temp % 10) + '0';
        temp /= 10;
    } while (temp > 0 && k < 5);


    while (k > 0) {
        while(!(UCA1IFG&UCTXIFG)); UCA1TXBUF = buffer[--k];
    }
}



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{


    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG:

            ADC_Result[ADC_i] = ADCMEM0;





            if(ADC_i == 0)
            {
                __no_operation();
                ADC_i = 2;
            }
            else
            {
                ADC_i--;
            }
            break;
        default:
            break;
    }
}




#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR(void)
#else
#error Compiler not supported!
#endif
{
  char received_char;


  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        received_char = UCA1RXBUF;


        while(!(UCA1IFG&UCTXIFG));
        UCA1TXBUF = received_char;


        if (uart_rx_state == FREQUENCY_INPUT_MODE)
        {
            if (received_char == '\r' || received_char == '\n')
            {
                uart_send_string("\r\n");
                if (freq_input_buffer_idx > 0)
                {
                    freq_input_buffer[freq_input_buffer_idx] = '\0';
                    unsigned long requested_freq_hz = atoi(freq_input_buffer);


                    if (requested_freq_hz >= 1 && requested_freq_hz <= 10000)
                    {
                        unsigned long calc_increment;



                        calc_increment = 24000000UL / requested_freq_hz;
                        calc_increment /= 300UL;


                        const unsigned int MIN_CCR_INCREMENT = 7;
                        const unsigned int MAX_CCR_INCREMENT = 65535;

                        if (calc_increment < MIN_CCR_INCREMENT) {
                            timer_b0_ccr0_increment = MIN_CCR_INCREMENT;
                        } else if (calc_increment > MAX_CCR_INCREMENT) {
                            timer_b0_ccr0_increment = MAX_CCR_INCREMENT;
                        } else {
                            timer_b0_ccr0_increment = (unsigned int)calc_increment;
                        }



                        unsigned long term_calc = (24000000UL / 300UL) * 1000UL;
                        unsigned long actual_freq_milihz = term_calc / (unsigned long)timer_b0_ccr0_increment;

                        unsigned int actual_hz = actual_freq_milihz / 1000;
                        unsigned int actual_tenths = (actual_freq_milihz % 1000) / 100;

                        uart_send_string("Target: "); uart_send_uint(requested_freq_hz); uart_send_string("Hz. ");
                        uart_send_string("Actual: "); uart_send_uint(actual_hz); uart_send_string("."); uart_send_uint(actual_tenths);
                        uart_send_string("Hz (CCR Inc: "); uart_send_uint(timer_b0_ccr0_increment); uart_send_string(")\r\n> ");
                    }
                    else
                    {

                        uart_send_string("Invalid frequency. Valid range: 1-10000 Hz.\r\nEffective range approx 1-11428 Hz.\r\n> ");
                    }
                } else {
                     uart_send_string("No frequency was entered.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE;
                freq_input_buffer_idx = 0;
                memset(freq_input_buffer, 0, FREQ_INPUT_BUFFER_SIZE);
            }

            else if (received_char >= '0' && received_char <= '9' && freq_input_buffer_idx < (FREQ_INPUT_BUFFER_SIZE - 1))
            {
                freq_input_buffer[freq_input_buffer_idx++] = received_char;
            }

            else if ((received_char == '\b' || received_char == 127) && freq_input_buffer_idx > 0)
            {
                freq_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else if (uart_rx_state == AMPLITUDE_INPUT_MODE)
        {
            if (received_char == '\r' || received_char == '\n')
            {
                uart_send_string("\r\n");
                if (amplitude_input_buffer_idx > 0)
                {
                    amplitude_input_buffer[amplitude_input_buffer_idx] = '\0';
                    int requested_amplitude = atoi(amplitude_input_buffer);


                    if (requested_amplitude >= 0 && requested_amplitude <= 100)
                    {
                        current_amplitude_percentage = (unsigned int)requested_amplitude;

                        current_amplitude_scaling_factor = ((long)current_amplitude_percentage * 256) / 100;
                        uart_send_string("Amplitude set to: ");
                        uart_send_uint(current_amplitude_percentage);
                        uart_send_string(" %\r\n> ");
                    }
                    else
                    {
                        uart_send_string("Invalid amplitude. Enter a value between 0-100 %.\r\n> ");
                    }
                } else {
                     uart_send_string("No amplitude was entered.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE;
                amplitude_input_buffer_idx = 0;
                memset(amplitude_input_buffer, 0, AMPLITUDE_INPUT_BUFFER_SIZE);
            }

            else if (received_char >= '0' && received_char <= '9' && amplitude_input_buffer_idx < (AMPLITUDE_INPUT_BUFFER_SIZE - 1))
            {
                amplitude_input_buffer[amplitude_input_buffer_idx++] = received_char;
            }

            else if ((received_char == '\b' || received_char == 127) && amplitude_input_buffer_idx > 0)
            {
                amplitude_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else if (uart_rx_state == OFFSET_INPUT_MODE)
        {
            if (received_char == '\r' || received_char == '\n')
            {
                uart_send_string("\r\n");
                if (offset_input_buffer_idx > 0)
                {
                    offset_input_buffer[offset_input_buffer_idx] = '\0';

                    long requested_offset_mv = atol(offset_input_buffer);


                    if (requested_offset_mv >= 0 && requested_offset_mv <= 3300)
                    {
                        current_offset_mv = (unsigned int)requested_offset_mv;

                        precalculated_offset_dac_units = ((long)current_offset_mv * 4095L) / 3300L;
                        uart_send_string("Offset set to: ");
                        uart_send_uint(current_offset_mv);
                        uart_send_string(" mV\r\n> ");
                    }
                    else
                    {
                        uart_send_string("Invalid offset. Enter a value between 0-3300 mV.\r\n> ");
                    }
                } else {
                     uart_send_string("No offset value was entered.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE;
                offset_input_buffer_idx = 0;
                memset(offset_input_buffer, 0, OFFSET_INPUT_BUFFER_SIZE);
            }

            else if (received_char >= '0' && received_char <= '9' && offset_input_buffer_idx < (OFFSET_INPUT_BUFFER_SIZE - 1))
            {
                offset_input_buffer[offset_input_buffer_idx++] = received_char;
            }

            else if ((received_char == '\b' || received_char == 127) && offset_input_buffer_idx > 0)
            {
                offset_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else if (uart_rx_state == PHASE_INPUT_MODE)
        {
            if (received_char == '\r' || received_char == '\n')
            {
                uart_send_string("\r\n");
                if (phase_input_buffer_idx > 0)
                {
                    phase_input_buffer[phase_input_buffer_idx] = '\0';
                    int requested_phase = atoi(phase_input_buffer);


                    if (requested_phase >= 0 && requested_phase < 300)
                    {
                        current_phase_offset_samples = (unsigned int)requested_phase;
                        uart_send_string("Phase set to: ");
                        uart_send_uint(current_phase_offset_samples);
                        uart_send_string(" samples\r\n> ");
                    }
                    else
                    {
                        uart_send_string("Invalid phase. Enter a value between 0-299 samples.\r\n> ");
                    }
                } else {
                     uart_send_string("No phase value was entered.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE;
                phase_input_buffer_idx = 0;
                memset(phase_input_buffer, 0, PHASE_INPUT_BUFFER_SIZE);
            }

            else if (received_char >= '0' && received_char <= '9' && phase_input_buffer_idx < (PHASE_INPUT_BUFFER_SIZE - 1))
            {
                phase_input_buffer[phase_input_buffer_idx++] = received_char;
            }

            else if ((received_char == '\b' || received_char == 127) && phase_input_buffer_idx > 0)
            {
                phase_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else
        {
            if (received_char == 'F' || received_char == 'f')
            {
                uart_rx_state = FREQUENCY_INPUT_MODE;
                freq_input_buffer_idx = 0;
                memset(freq_input_buffer, 0, FREQ_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nEnter Frequency (1-10000Hz), then Enter: ");
            }
            else if (received_char == 'A' || received_char == 'a')
            {
                uart_rx_state = AMPLITUDE_INPUT_MODE;
                amplitude_input_buffer_idx = 0;
                memset(amplitude_input_buffer, 0, AMPLITUDE_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nEnter Amplitude (0-100%), then Enter: ");
            }
            else if (received_char == 'O' || received_char == 'o')
            {
                uart_rx_state = OFFSET_INPUT_MODE;
                offset_input_buffer_idx = 0;
                memset(offset_input_buffer, 0, OFFSET_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nEnter Offset (0-3300mV), then Enter: ");
            }
            else if (received_char == 'H' || received_char == 'h')
            {
                uart_rx_state = PHASE_INPUT_MODE;
                phase_input_buffer_idx = 0;
                memset(phase_input_buffer, 0, PHASE_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nEnter Phase (0-299 samples), then Enter: ");
            }
            else if (received_char == '#')
            {

                for(j=0;j<3;j++)
                {
                   ADC_char[j][2] = j+48;

                   ADC_char[j][4] = ((ADC_Result[j]/1000)%10)+48;
                   ADC_char[j][6] = ((ADC_Result[j]/100)%10)+48;
                   ADC_char[j][5] = ((ADC_Result[j]/10)%10)+48;
                   ADC_char[j][7] = (ADC_Result[j]%10)+48;
                }

                for(j=0;j<3;j++) {
                    for(i=0;i<12;i++) {
                        while(!(UCA1IFG&UCTXIFG));
                        UCA1TXBUF = ADC_char[j][i];
                    }
                }
                uart_send_string("\r\n> ");
            }

            else if(received_char == 's') {
                waveform_slots[current_assignment_slot_index] = sine_wave;
                uart_send_string("\r\nSine assigned to Slot "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 't') {
                waveform_slots[current_assignment_slot_index] = triangle_wave;
                uart_send_string("\r\nTriangle assigned to Slot "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'r') {
                waveform_slots[current_assignment_slot_index] = ramp_wave;
                uart_send_string("\r\nRamp assigned to Slot "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'p') {
                waveform_slots[current_assignment_slot_index] = square_wave;
                uart_send_string("\r\nSquare assigned to Slot "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'c') {
                waveform_slots[current_assignment_slot_index] = pulse_train_wave;
                uart_send_string("\r\nPulse train assigned to Slot "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }

            else if(received_char == 'X' || received_char == 'x') {
                current_assignment_slot_index = 0;
                uart_send_string("\r\nSlot 0 for assignment.\r\n> ");
            }
            else if(received_char == 'Y' || received_char == 'y') {
                current_assignment_slot_index = 1;
                uart_send_string("\r\nSlot 1 for assignment.\r\n> ");
            }
            else if(received_char == 'Z' || received_char == 'z') {
                current_assignment_slot_index = 2;
                uart_send_string("\r\nSlot 2 for assignment.\r\n> ");
            }

            else if(received_char == 'L' || received_char == 'l') {
                current_output_slot_index = 0;
                uart_send_string("\r\nSlot 0 for DAC output.\r\n> ");
            }
            else if(received_char == 'M' || received_char == 'm') {
                current_output_slot_index = 1;
                uart_send_string("\r\nSlot 1 for DAC output.\r\n> ");
            }
            else if(received_char == 'N' || received_char == 'n') {
                current_output_slot_index = 2;
                uart_send_string("\r\nSlot 2 for DAC output.\r\n> ");
            }
            else if (received_char == '\r' || received_char == '\n')
            {
                uart_send_string("\r\n> ");
            }
            else
            {
                uart_send_string("\r\nUnknown command.\r\n> ");
            }
        }
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}




#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_B0_VECTOR
__interrupt void Timer2_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_B0_VECTOR))) Timer2_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{


    TB2CCR0 += timer_b0_ccr0_increment;


    current_waveform_index++;
    if(current_waveform_index >= 300)
        current_waveform_index = 0;


    unsigned int temp_phased_index = current_waveform_index + current_phase_offset_samples;
    if (temp_phased_index >= 300) {
        temp_phased_index -= 300;
    }
    unsigned int phased_waveform_index = temp_phased_index;



    unsigned int raw_sample = waveform_slots[current_output_slot_index][phased_waveform_index];


    long dac_raw_midpoint = 2048;




    long sample_relative_to_raw_mid = (long)raw_sample - dac_raw_midpoint;






    long scaled_relative_sample = (sample_relative_to_raw_mid * current_amplitude_scaling_factor) >> 8;


    long offset_dac_units = precalculated_offset_dac_units;



    long final_value_long = offset_dac_units + scaled_relative_sample;


    if (final_value_long < 0) final_value_long = 0;
    if (final_value_long > 4095) final_value_long = 4095;



    SAC1DAT = (unsigned int)final_value_long;
}



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_B1_VECTOR
__interrupt void Timer2_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_B1_VECTOR))) Timer2_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{

    switch(TB2IV)
    {
        case TBIV__NONE:
            break;
        case TBIV__TBCCR1:
            TB2CCR1 += 10;


            break;
        case TBIV__TBIFG:
            break;
        default:
            break;
    }
}




void Software_Trim()
{

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

    do
    {

        CSCTL0 = 0x100;
        do
        {


            CSCTL7 &= ~DCOFFG;
        }while (CSCTL7 & DCOFFG);



        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);



        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));


        csCtl0Read = CSCTL0;
        csCtl1Read = CSCTL1;

        oldDcoTap = newDcoTap;
        newDcoTap = csCtl0Read & 0x01ff;
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;



        if(newDcoTap < 256)
        {
            newDcoDelta = 256 - newDcoTap;

            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256))
                endLoop = 1;
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else
        {
            newDcoDelta = newDcoTap - 256;

            if(oldDcoTap < 256)
                endLoop = 1;
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }


        if(newDcoDelta < bestDcoDelta)
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);


    CSCTL0 = csCtl0Copy;
    CSCTL1 = csCtl1Copy;

    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
}
