// Descriere: Acest exemplu functioneaza in modul Repetare-secventa-de-canale
// cu TB1.1 ca sursa de semnal de trigger ACLK.
// A2/A1/A0 sunt esantionate cu 16 ceasuri ADCclks cu referinta la 3,3V.
// Oscilatorul intern temporizeaza esantionarea (16x) si conversia (13x).
// in interiorul ADC_ISR, valoarea esantionului A2/A1/A0 este pusa in tabloul ADC_Result[3].
// ACLK = REFO implicit ~32768Hz, MCLK = SMCLK = DCODIV implicit ~1MHz.
// Frecventa MCLK/SMCLK este setata la 24MHz in functia main().


// Includerea fisierului header specific pentru microcontrolerele MSP430,
// care defineste registrele, bitii si functiile intrinseci.
#include <msp430.h>
// Includerea bibliotecii standard C pentru functii precum atoi (conversie sir la intreg).
#include <stdlib.h>
// Includerea bibliotecii standard C pentru functii de manipulare a sirurilor, cum ar fi memset.
#include <string.h>


// Prototipul functiei Init_GPIO, desi functia nu este definita sau utilizata in acest fisier.
// Poate fi un vestigiu dintr-o versiune anterioara sau destinata unei extinderi viitoare.
void Init_GPIO();

// Tablou de cautare (LUT) pentru forma de unda sinusoidala.
// Contine 300 de esantioane pe 12 biti (0-4095), reprezentand un ciclu complet.
// Valoarea 2048 reprezinta punctul de mijloc (0V analogic, presupunand o scalare adecvata).
unsigned int sinus[300] = {
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

// Tablou de cautare (LUT) pentru forma de unda rampa (dinte de fierastrau).
// Contine 300 de esantioane pe 12 biti, crescand liniar de la 0 la 4093.
unsigned int rampa[300] = { 0, 14, 27, 41, 55, 68, 82, 96, 110, 123, 137, 151, 164, 178, 192, 205,
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
// Tablou de cautare (LUT) pentru forma de unda triunghiulara.
// Contine 300 de esantioane pe 12 biti, crescand liniar pana la 4095 la jumatatea ciclului,
// apoi descrescand liniar inapoi la 0.
unsigned int triunghi[300] = {0, 27, 55, 82, 110, 137, 164, 192, 219, 246, 274, 301, 329, 356, 383, 411,
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

// Declaratie pentru tabloul de forma de unda dreptunghiulara (patrata).
// Va fi initializat in functia main() cu un factor de umplere de 50%.
unsigned int patrat[300];
// Declaratie pentru tabloul de forma de unda tren de impulsuri.
// Va fi initializat in functia main() cu un factor de umplere mic (1% in acest caz).
unsigned int tren_impulsuri[300];

// Variabile globale pentru gestionarea sloturilor de forme de unda.
// Permite utilizatorului sa atribuie diferite forme de unda la 3 sloturi si sa selecteze unul pentru iesire.
unsigned int *waveform_slots[3]; // Un tablou de 3 pointeri la unsigned int. Fiecare pointer va indica catre unul din tablourile de forme de unda (sinus, rampa etc.).
unsigned int current_assignment_slot_index = 0; // Indexul slotului selectat curent pentru atribuirea unei noi forme de unda (0, 1, sau 2).
unsigned int current_output_slot_index = 0;     // Indexul slotului selectat curent pentru generarea semnalului de iesire DAC (0, 1, sau 2).

// Tablou bidimensional de caractere folosit pentru a formata valorile ADC citite
// intr-un sir de caractere ce poate fi trimis prin UART.
// Fiecare rand corespunde unui canal ADC si contine un format predefinit.
// Ex: "Ax= xxxx\n\r\0" unde x sunt cifrele valorii ADC.
char ADC_char[3][12] = {
    {'A','x','=',' ','x','x','x','x',10,13,0}, // Pentru ADC_Result[0]
    {'A','x','=',' ','x','x','x','x',10,13,0}, // Pentru ADC_Result[1]
    {'A','x','=',' ','x','x','x','x',10,13,0}  // Pentru ADC_Result[2]
};
// Variabile de contorizare generice, folosite in diverse bucle.
unsigned int i,j;
// Indexul curent in tabloul de forma de unda selectat pentru iesire.
// Este incrementat in ISR-ul timer-ului pentru a parcurge esantioanele formei de unda.
unsigned int current_waveform_index = 0;

// Prototipuri pentru functiile definite ulterior in fisier.
void Software_Trim(); // Functie pentru calibrarea fina a oscilatorului DCO.
void uart_send_string(const char *str); // Functie pentru trimiterea unui sir de caractere prin UART.
void uart_send_uint(unsigned int val); // Functie pentru trimiterea unui intreg fara semn prin UART.

// Defineste frecventa ceasului principal (MCLK) si a ceasului sub-sistemului (SMCLK) in MHz.
// Aceasta este folosita in functia Software_Trim si pentru calculele de temporizare/frecventa.
#define MCLK_FREQ_MHZ 24

// Tablou pentru stocarea rezultatelor conversiilor ADC pentru cele 3 canale.
unsigned int ADC_Result[3];
// Index utilizat in ISR-ul ADC pentru a plasa rezultatul conversiei in `ADC_Result`.
unsigned char ADC_i;
// Variabila globala pentru datele DAC. Desi este declarata, valoarea finala pentru DAC
// este calculata si scrisa direct in registrul SAC1DAT in Timer2_B0_ISR.
// Poate fi un vestigiu sau pentru o utilizare initiala.
unsigned int DAC_data=0;

// Variabile globale pentru gestionarea intrarii de la utilizator pentru setarea frecventei semnalului.
#define FREQ_INPUT_BUFFER_SIZE 6 // Dimensiunea buffer-ului pentru intrarea frecventei (ex: "10000" + terminator null).
char freq_input_buffer[FREQ_INPUT_BUFFER_SIZE]; // Buffer pentru stocarea caracterelor (cifrelor) introduse de utilizator pentru frecventa.
unsigned char freq_input_buffer_idx = 0; // Index curent in `freq_input_buffer`.

// Variabile globale pentru gestionarea intrarii de la utilizator pentru setarea amplitudinii semnalului.
#define AMPLITUDE_INPUT_BUFFER_SIZE 4 // Dimensiunea buffer-ului pentru intrarea amplitudinii (ex: "100" + terminator null). Trebuie sa fie suficient de mare pentru a stoca numarul maxim de cifre plus caracterul null.
char amplitude_input_buffer[AMPLITUDE_INPUT_BUFFER_SIZE]; // Buffer pentru stocarea caracterelor (cifrelor) introduse de utilizator pentru amplitudine, inainte de conversia in numar.
unsigned char amplitude_input_buffer_idx = 0; // Index curent in `amplitude_input_buffer`, indica urmatoarea pozitie libera.
unsigned int current_amplitude_percentage = 100; // Procentajul curent al amplitudinii semnalului (0-100%), implicit 100%. Aceasta valoare este setata de utilizator.
// Factor de scalare precalculat pentru amplitudine, folosit in rutina de intrerupere (ISR) Timer2_B0_ISR pentru optimizare.
// Este calculat o singura data cand amplitudinea este setata, pentru a evita calcule costisitoare (impartiri) in ISR.
// Formula: ((long)current_amplitude_percentage * 256) / 100. Multiplicarea cu 256 si apoi deplasarea la dreapta cu 8 biti in ISR este mai rapida decat impartirea directa la 100.
volatile unsigned int current_amplitude_scaling_factor;

// Variabile globale pentru gestionarea intrarii de la utilizator pentru setarea offset-ului DC al semnalului.
#define OFFSET_INPUT_BUFFER_SIZE 5 // Dimensiunea buffer-ului pentru intrarea offset-ului (ex: "3300" + terminator null).
char offset_input_buffer[OFFSET_INPUT_BUFFER_SIZE]; // Buffer pentru stocarea caracterelor introduse de utilizator pentru offset.
unsigned char offset_input_buffer_idx = 0; // Index curent in `offset_input_buffer`.
unsigned int current_offset_mv = 1650; // Valoarea curenta a offset-ului DC in milivolti, implicit 1650mV (VCC/2 pentru o referinta de 3.3V).
// Valoarea offset-ului precalculata in unitati DAC (Digital-to-Analog Converter), folosita in ISR pentru optimizare.
// Este calculata o singura data cand offset-ul este setat.
// Formula: ((long)current_offset_mv * 4095L) / 3300L (presupunand un DAC pe 12 biti si o referinta de 3.3V).
volatile long precalculated_offset_dac_units;

// Variabile globale pentru gestionarea intrarii de la utilizator pentru setarea defazajului semnalului.
#define PHASE_INPUT_BUFFER_SIZE 4 // Dimensiunea buffer-ului pentru intrarea fazei (ex: "299" + terminator null, deoarece sunt 300 de esantioane, indexate 0-299).
char phase_input_buffer[PHASE_INPUT_BUFFER_SIZE]; // Buffer pentru stocarea caracterelor introduse de utilizator pentru faza.
unsigned char phase_input_buffer_idx = 0; // Index curent in `phase_input_buffer`.
unsigned int current_phase_offset_samples = 0; // Offset-ul curent al fazei, exprimat in numar de esantioane (0-299), implicit 0 (fara defazaj).


// Enumerare pentru a defini starile posibile ale logicii de receptie UART (Universal Asynchronous Receiver/Transmitter).
// Aceasta permite microcontrolerului sa interpreteze corect datele primite de la utilizator,
// in functie de comanda anterioara (ex: daca utilizatorul a apasat 'F', se asteapta cifre pentru frecventa).
typedef enum {
    NORMAL_COMMAND_MODE,    // Mod implicit: Asteapta comenzi generale de un singur caracter (ex: 's' pentru sinus, 'F' pentru a intra in modul de setare a frecventei).
    FREQUENCY_INPUT_MODE,   // Mod de intrare frecventa: Asteapta caractere numerice pentru noua valoare a frecventei.
    AMPLITUDE_INPUT_MODE,   // Mod de intrare amplitudine: Asteapta caractere numerice pentru noua valoare a amplitudinii.
    OFFSET_INPUT_MODE,      // Mod de intrare offset: Asteapta caractere numerice pentru noua valoare a offset-ului.
    PHASE_INPUT_MODE        // Mod de intrare faza (stare noua): Asteapta caractere numerice pentru noua valoare a fazei.
} UART_RX_State;
// Variabila care stocheaza starea curenta a masinii de stari a receptorului UART.
// Este initializata in `NORMAL_COMMAND_MODE`.
UART_RX_State uart_rx_state = NORMAL_COMMAND_MODE;

// Incrementul pentru registrul TB2CCR0 al Timer-ului B2. Aceasta este o variabila 'volatile' deoarece
// poate fi modificata in rutina de intrerupere UART si citita in rutina de intrerupere a timer-ului.
// Valoarea sa determina perioada dintre intreruperile timer-ului care genereaza esantioanele DAC.
// Frecventa semnalului = Frecventa SMCLK / (timer_b0_ccr0_increment * Numar_Esantioane_Pe_Ciclu).
// Valoarea implicita 666, cu 300 de esantioane si SMCLK de 24MHz, ar da: 24.000.000 / (666 * 300) = ~120.12 Hz.
// Comentariul original "Default for 100Hz" era probabil pentru o alta configuratie de ceas sau numar de esantioane.
volatile unsigned int timer_b0_ccr0_increment = 666;


// Functia principala a programului.
int main(void)
{
    // Opreste Watchdog Timer-ul (WDT). Este o practica standard la inceputul programelor pentru MSP430
    // pentru a preveni resetarea neasteptata a microcontrolerului in timpul initializarilor sau executiei.
    // WDTPW este parola pentru WDT, WDTHOLD opreste timer-ul.
    WDTCTL = WDTPW | WDTHOLD;

    // Initializeaza tabloul `patrat` pentru a genera o forma de unda dreptunghiulara (square wave).
    // Primele 150 de esantioane (jumatate din ciclu) sunt setate la valoarea maxima a DAC-ului (4095).
    // Urmatoarele 150 de esantioane sunt setate la 0. Acest lucru creeaza un factor de umplere de 50%.
    for(i=0; i<300; i++)
    {
        if(i < 150) patrat[i] = 4095; // Nivel inalt
        else patrat[i] = 0;          // Nivel scazut
    }

    // Initializeaza tabloul `tren_impulsuri` pentru a genera un tren de impulsuri.
    // Primele 3 esantioane sunt setate la 4095, restul la 0.
    // Pentru 300 de esantioane, aceasta corespunde unui factor de umplere de 3/300 = 1%.
    for(i=0; i<300; i++)
    {
        if(i < 3) tren_impulsuri[i] = 4095; // Impuls activ
        else tren_impulsuri[i] = 0;         // Nivel scazut
    }

    // Initializeaza sloturile de forme de unda.
    // La pornire, toate cele trei sloturi sunt setate sa indice catre forma de unda sinusoidala.
    waveform_slots[0] = sinus;
    waveform_slots[1] = sinus;
    waveform_slots[2] = sinus;

    // Initializeaza parametrii semnalului la valorile lor implicite.
    current_amplitude_percentage = 100; // Amplitudine 100%
    // Precalculeaza factorul de scalare pentru amplitudine pentru utilizare optimizata in ISR.
    current_amplitude_scaling_factor = ((long)current_amplitude_percentage * 256) / 100;
    current_offset_mv = 1650;          // Offset DC de 1650 mV (VCC/2 pentru 3.3V)
    // Precalculeaza valoarea offset-ului in unitati DAC pentru utilizare optimizata in ISR.
    precalculated_offset_dac_units = ((long)current_offset_mv * 4095L) / 3300L;
    current_phase_offset_samples = 0;  // Faza 0 esantioane (fara defazaj initial)

    // Configurarea sistemului de ceas (Clock System - CS).
    // Seteaza numarul de stari de asteptare pentru memoria Flash, necesar pentru frecvente MCLK mai mari.
    // NWAITS_2 corespunde la 2 stari de asteptare, potrivite pentru gama de frecventa 16-24MHz.
    FRCTL0 = FRCTLPW | NWAITS_2;

    // Opreste temporar FLL (Frequency Locked Loop) in timpul configurarii DCO.
    // SCG0 (System Clock Generator 0) in registrul de stare (SR) opreste FLL.
    __bis_SR_register(SCG0);
    // Selecteaza REFOCLK (oscilator de referinta intern, ~32768 Hz) ca sursa de referinta pentru FLL.
    CSCTL3 = SELREF__REFOCLK;
    // Configureaza DCO (Digitally Controlled Oscillator).
    // DCOFTRIMEN_1 activeaza auto-ajustarea DCO. DCOFTRIM0/1 sunt biti de ajustare fina.
    // DCORSEL_6 selecteaza gama de frecventa a DCO pentru 17-24MHz.
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_6;
    // Configureaza FLL. FLLD_0 selecteaza un divizor de 1 pentru FLL.
    // FLLN (multiplicatorul FLL) este setat la 731.
    // Frecventa DCO = (FLLN + 1) * Frecventa_Referinta_FLL / FLLD
    // Frecventa DCO = (731 + 1) * 32768 Hz / 1 = 732 * 32768 Hz = 24,000,000 Hz (24MHz).
    CSCTL2 = FLLD_0 + 731;
    // O mica intarziere pentru stabilizarea DCO/FLL.
    __delay_cycles(3);
    // Reporneste FLL stergand bitul SCG0.
    __bic_SR_register(SCG0);
    // Apeleaza functia Software_Trim pentru a calibra fin frecventa DCO.
    Software_Trim();
    // Selecteaza sursele de ceas pentru MCLK, SMCLK si ACLK.
    // SELMS__DCOCLKDIV: MCLK si SMCLK sunt derivate din DCOCLKDIV (DCO impartit la 1).
    // SELA__REFOCLK: ACLK este derivat din REFOCLK.
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

    // Reseteaza indexul curent al formei de unda.
    current_waveform_index = 0;
    // fmulti=1; // Linie comentata, probabil un vestigiu.

    // Configurare pini GPIO (General Purpose Input/Output).
    // Configureaza P1.5 pentru functia sa alternativa (ex: iesire ACLK sau timer).
    // PxSEL0 si PxSEL1 controleaza selectia functiei pinului.
    P1SEL0 |= BIT5;
    P1SEL1 |= BIT5;

    // Configureaza P4.2 (UCA1TXD) si P4.3 (UCA1RXD) pentru modulul UART eUSCI_A1.
    // P4SEL0 = 1, P4SEL1 = 0 selecteaza functia primara a modulului.
    P4SEL0 |= BIT2 | BIT3;
    P4SEL1 &=~(BIT2 | BIT3);

    // Configurare UART (eUSCI_A1) pentru comunicatie seriala.
    // Pune modulul eUSCI_A1 in stare de reset (UCSWRST = 1) pentru configurare.
    UCA1CTLW0 |= UCSWRST;
    // Selecteaza SMCLK (24MHz) ca sursa de ceas pentru UART (UCSSEL_2).
    UCA1CTLW0 |= UCSSEL_2;
    // Calcule si setari pentru baud rate-ul de 115200 la 24MHz SMCLK.
    // N = Fbrclk / Baudrate = 24.000.000 / 115200 = 208.333
    // UCBRx (divizorul principal) = floor(N/16) = floor(208.333/16) = floor(13.0208) = 13.
    UCA1BR0 = 13; // Partea inferioara a divizorului UCBRx.
    UCA1BR1 = 0x00; // Partea superioara a divizorului UCBRx.
    // Configureaza partea de modulatie a baud rate-ului.
    // UCBRFx (divizor fractionar) = floor((N/16 - UCBRx)*16) = floor((13.0208 - 13)*16) = floor(0.0208*16) = floor(0.3328) = 0.
    // UCBRSx (selectie model de modulatie) este ales din tabelul de referinta pentru N - floor(N) = 0.333, care este 0x44.
    // UCOS16 = 1 activeaza modul de supraesantionare.
    UCA1MCTLW = (0x44 << 8) | (0 << 4) | UCOS16;

    // Configureaza pinii pentru iesirea SAC (Smart Analog Combo) / DAC.
    // P3.0 (SAC1OAO / OA0O) ca iesire pentru DAC.
    P3DIR |= BIT0; P3SEL0 |= BIT0; P3SEL1 &= ~BIT0;
    // P3.4 (SAC1OAP / OA0P) ca intrare/iesire pentru SAC (daca este folosit).
    P3DIR |= BIT4; P3SEL0 |= BIT4; P3SEL1 &= ~BIT4;

    // Configureaza pinii P1.0, P1.1, P1.2 ca intrari analogice pentru ADC (A0, A1, A2).
    // PxSEL0 = 1, PxSEL1 = 1 selecteaza functia analogica.
    P1SEL0 |=  BIT0 + BIT1 + BIT2;
    P1SEL1 |=  BIT0 + BIT1 + BIT2;

    // Deblocheaza configurarea pinilor GPIO (necesar pe unele familii MSP430 dupa un reset
    // pentru a permite modificarea registrelor PxSELx, PxDIRx etc. daca LOCKLPM5 este setat).
    PM5CTL0 &= ~LOCKLPM5;

    // Activeaza referinta de tensiune interna (INTREFEN).
    // Aceasta poate fi folosita de ADC sau DAC.
    // PMMCTL0_H = PMMPW_H este necesar pentru a scrie in registrele PMM protejate.
    PMMCTL0_H = PMMPW_H;
    PMMCTL2 |= INTREFEN;
    // Asteapta un timp pentru stabilizarea referintei interne (aproximativ 400 de cicluri de ceas).
    __delay_cycles(400);

    // Scoate modulul eUSCI_A1 din starea de reset (UCSWRST = 0), activand UART-ul.
    UCA1CTLW0 &= ~UCSWRST;

    // Trimite un meniu de ajutor si informatii initiale catre utilizator prin UART.
    uart_send_string("\r\n--- Generator de forme de unda MSP430 ---\r\n");
    uart_send_string("Atribuire forma de unda (se aplica slotului de atribuire selectat):\r\n");
    uart_send_string("  s : Sinus\r\n  t : Triunghi\r\n  r : Rampa\r\n  p : Dreptunghiulara\r\n  c : Puls\r\n");
    uart_send_string("Selectati slotul de atribuire:\r\n  X:Slot 0, Y:Slot 1, Z:Slot 2\r\n");
    uart_send_string("Selectati slotul de iesire DAC:\r\n  L:Slot 0, M:Slot 1, N:Slot 2\r\n");
    uart_send_string("Alte comenzi:\r\n");
    uart_send_string("  F : Setare Frecventa (1-10000 Hz)\r\n");
    uart_send_string("  A : Setare Amplitudine (0-100 %)\r\n");
    uart_send_string("  O : Setare Offset (0-3300 mV)\r\n");
    uart_send_string("  H : Setare Faza (0-299 esantioane)\r\n");
    uart_send_string("  # : Trimite Date ADC\r\n");
    uart_send_string("in prezent: Slot 0->0, Frecv ~100Hz, Ampl 100%, Offset 1650mV, Faza 0.\r\n"); // Mesaj actualizat
    uart_send_string("> "); // Prompt pentru utilizator.

    // Activeaza intreruperea de receptie UART (UCRXIE).
    // Rutina de intrerupere `Rutina_USCI_A1` va fi apelata la primirea unui caracter.
    UCA1IE |= UCRXIE;

    // Configurare ADC12_B (Convertor Analog-Digital pe 12 biti).
    // ADCSHT_10: Seteaza timpul de esantionare si mentinere (Sample & Hold) la 256 cicluri ADC12CLK.
    // ADCON: Porneste ADC-ul (dar nu incepe conversiile inca).
    ADCCTL0 |= ADCSHT_10 | ADCON;
    // ADCSHP: Semnalul de esantionare este generat de timer-ul de esantionare intern al ADC-ului.
    // ADCSHS_2: Selecteaza TimerB1.OUT1 ca sursa de trigger pentru esantionare (SHI - Sample-and-Hold Input).
    // ADCCONSEQ_3: Mod de conversie repetata a unei secvente de canale.
    ADCCTL1 |= ADCSHP | ADCSHS_2 | ADCCONSEQ_3;
    // ADCRES: Seteaza rezolutia ADC.
    // ADCRES_2: Rezolutie de 12 biti.
    ADCCTL2 &= ~ADCRES;
    ADCCTL2 |= ADCRES_2;
    // ADCINCH_2: Selecteaza canalul de intrare initial pentru secventa la A2 (P1.2).
    // Desi este setat modul secventa, codul pare sa gestioneze un singur canal activ la un moment dat
    // sau se bazeaza pe ISR pentru a comuta canalele (desi ISR-ul ADC nu face asta explicit).
    ADCMCTL0 |= ADCINCH_2;
    // ADCIE0: Activeaza intreruperea pentru ADC (ADCIFG0) cand o conversie din secventa este finalizata.
    ADCIE |= ADCIE0;

    // Configurare SAC1 (Smart Analog Combo) pentru a functiona ca DAC (Convertor Digital-Analog).
    // DACSREF_0: Referinta pozitiva VCC (3.3V), referinta negativa VSS (GND).
    // DACLSEL_0: Sursa de incarcare rapida pentru DAC.
    // DACIE: Activeaza intreruperea DAC (desi nu pare a fi folosita o ISR specifica pentru DAC in acest cod).
    SAC1DAC = DACSREF_0 + DACLSEL_0 + DACIE;
    // incarca o valoare initiala in registrul de date DAC (0 in acest caz).
    SAC1DAT = DAC_data;
    // DACEN: Activeaza DAC-ul din modulul SAC.
    SAC1DAC |= DACEN;
    // Configureaza Amplificatorul Operational (OA) din SAC1 pentru a functiona ca buffer pentru iesirea DAC.
    // NMUXEN, PMUXEN: Activeaza multiplexoarele pentru intrarile OA.
    // PSEL_1: Conecteaza intrarea pozitiva (non-inverting) a OA la iesirea DAC-ului intern.
    // NSEL_1: Conecteaza intrarea negativa (inverting) a OA la pinul extern OA0N (P3.1, daca configurat).
    //         in configuratia buffer, intrarea negativa este conectata la iesirea OA.
    // OAPM_0: Mod de consum redus pentru OA.
    SAC1OA = NMUXEN + PMUXEN + PSEL_1 + NSEL_1; // Configuratie initiala
    SAC1OA |= OAPM_0; // Seteaza modul de putere.
    // Configureaza PGA (Programmable Gain Amplifier) din SAC1.
    // MSEL_1: Mod buffer (castig unitar).
    SAC1PGA = MSEL_1;
    // SACEN: Activeaza modulul SAC.
    // OAEN: Activeaza amplificatorul operational.
    SAC1OA |= SACEN + OAEN;

    // Configurare Timer_B2 (TB2) pentru generarea formei de unda (actualizarea DAC-ului).
    // incarca valoarea initiala in registrul Capture/Compare 0 (TB2CCR0).
    // intreruperea `Timer2_B0_ISR` va fi generata cand contorul timer-ului (TB2R) ajunge la aceasta valoare.
    TB2CCR0 = timer_b0_ccr0_increment;
    // Seteaza o valoare pentru TB2CCR1. Folosit in `Timer2_B1_ISR`.
    TB2CCR1 = 10;
    // CCIE (CCR0 Interrupt Enable): Activeaza intreruperea pentru TB2CCR0.
    TB2CCTL0 = CCIE;
    // CCIE (CCR1 Interrupt Enable): Activeaza intreruperea pentru TB2CCR1.
    TB2CCTL1 = CCIE;
    // Configureaza registrul de control al Timer-ului B2 (TB2CTL).
    // TBSSEL__SMCLK: Selecteaza SMCLK (24MHz) ca sursa de ceas pentru Timer B2.
    // ID_0: Divizor de intrare /1 (fara divizare).
    // TBCLR: Reseteaza contorul timer-ului, flag-urile de intrerupere si directia de numarare.
    // MC__CONTINOUS: Mod de numarare continua (numara de la 0 la 0xFFFF si reia).
    // TBIE (Timer B Interrupt Enable): Activeaza intreruperea de overflow a timer-ului (TBIFG).
    // Desi activata, ISR-ul `Timer2_B1_ISR` nu trateaza explicit TBIFG intr-un mod specific.
    TB2CTL |= TBSSEL__SMCLK | ID_0 | TBCLR | MC__CONTINOUS| TBIE;

    // Configurare Timer_B1 (TB1) pentru a genera semnalul de trigger pentru ADC.
    // TB1CCR0 seteaza perioada timer-ului. Cu ACLK (~32768Hz), 32768 / (326+1) = ~100.2 Hz.
    // Aceasta va fi frecventa de declansare a conversiilor ADC.
    TB1CCR0 = 326;
    // TB1CCTL1 configureaza modul de iesire pentru CCR1.
    // OUTMOD_7 (Reset/Set): Iesirea (legata intern la ADC SHI) este setata cand TB1R = TB1CCR1
    // si resetata cand TB1R = TB1CCR0. Aceasta genereaza un impuls PWM.
    TB1CCTL1 = OUTMOD_7;
    // TB1CCR1 seteaza factorul de umplere al impulsului de trigger (aproximativ 50%).
    TB1CCR1 = 163;
    // Configureaza registrul de control al Timer-ului B1 (TB1CTL).
    // TBSSEL__ACLK: Selecteaza ACLK (~32kHz) ca sursa de ceas.
    // MC__UP: Mod de numarare crescatoare (numara de la 0 la TB1CCR0).
    // TBCLR: Reseteaza contorul timer-ului.
    TB1CTL = TBSSEL__ACLK | MC__UP | TBCLR;

    // Initializeaza indexul pentru tabloul de rezultate ADC.
    ADC_i = 2; // incepe cu ultimul index pentru logica descrescatoare din ADC_ISR.
    // Reseteaza contorul `i` (folosit in bucle).
    i=0;
    // ADCENC (ADC Enable Conversion): Activeaza conversiile ADC.
    // ADC-ul va incepe acum conversiile la fiecare trigger de la Timer B1.
    ADCCTL0 |= ADCENC;
    // Reseteaza din nou Timer B1 (TBCLR) pentru a asigura sincronizarea corecta la pornire.
    TB1CTL |= TBCLR;

    // Activeaza intreruperile globale (GIE - Global Interrupt Enable) in registrul de stare (SR).
    // Microcontrolerul va incepe acum sa raspunda la evenimentele de intrerupere configurate.
    __bis_SR_register(GIE);
    // O instructiune care nu face nimic (No Operation).
    // Adesea folosita ca punct de oprire pentru debugger sau pentru a umple un ciclu de ceas.
    // Programul principal se incheie efectiv aici; functionalitatea ulterioara este gestionata de rutinele de intrerupere.
    __no_operation();
} // Sfarsitul functiei main

// Functie ajutatoare pentru trimiterea unui sir de caractere (terminat cu null) prin UART (eUSCI_A1).
void uart_send_string(const char *str)
{
    // Parcurge sirul de caractere pana la terminatorul null.
    while (*str)
    {
        // Asteapta pana cand buffer-ul de transmisie UART (UCA1TXBUF) este gol
        // (flag-ul UCTXIFG din UCA1IFG este setat).
        while (!(UCA1IFG & UCTXIFG));
        // Scrie caracterul curent in buffer-ul de transmisie.
        UCA1TXBUF = *str++;
    }
    // Asteapta pana cand toate datele sunt transmise si linia UART nu mai este ocupata
    // (flag-ul UCBUSY din UCA1STATW este 0).
    while (UCA1STATW & UCBUSY);
}

// Functie ajutatoare pentru trimiterea unui intreg fara semn (unsigned int) prin UART.
// Converteste numarul in sir de caractere si il trimite cifra cu cifra.
void uart_send_uint(unsigned int val) {
    char buffer[6]; // Buffer local pentru stocarea cifrelor (max 5 cifre pentru 65535 + terminator null).
    int k = 0;      // Index pentru buffer-ul local (redenumit din 'i' pentru a evita conflictul cu globalul 'i').

    // Cazul special pentru valoarea 0.
    if (val == 0) {
        while(!(UCA1IFG&UCTXIFG)); UCA1TXBUF = '0'; // Trimite '0'.
        return;
    }

    unsigned int temp = val; // Copie temporara a valorii.
    // Extrage cifrele de la dreapta la stanga si le stocheaza in buffer.
    do {
        buffer[k++] = (temp % 10) + '0'; // Converteste cifra in caracter ASCII.
        temp /= 10;
    } while (temp > 0 && k < 5); // Continua cat timp mai sunt cifre si buffer-ul nu e plin.

    // Trimite cifrele din buffer in ordinea corecta (inversa fata de cum au fost stocate).
    while (k > 0) {
        while(!(UCA1IFG&UCTXIFG)); UCA1TXBUF = buffer[--k];
    }
}

// Rutina de tratare a intreruperilor pentru ADC12_B (ADC_VECTOR).
// Se executa cand o conversie ADC este finalizata.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR // Specifica vectorul de intrerupere pentru compilatoarele TI si IAR.
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void) // Specifica pentru GCC.
#else
#error Compiler not supported!
#endif
{
    // Utilizeaza `__even_in_range` pentru a optimiza gestionarea surselor de intrerupere ADC.
    // ADCIV (ADC Interrupt Vector) contine codul sursei de intrerupere.
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:        // Nicio intrerupere pending.
            break;
        case ADCIV_ADCOVIFG:    // intrerupere de overflow al memoriei de conversie.
            break;
        case ADCIV_ADCTOVIFG:   // intrerupere de timeout al conversiei.
            break;
        case ADCIV_ADCHIIFG:    // intrerupere de depasire a limitei superioare a ferestrei.
            break;
        case ADCIV_ADCLOIFG:    // intrerupere de depasire a limitei inferioare a ferestrei.
            break;
        case ADCIV_ADCINIFG:    // intrerupere de depasire a limitei interioare a ferestrei.
            break;
        case ADCIV_ADCIFG:      // intrerupere de finalizare a conversiei (ADCIFG0 in acest caz).
            // Citeste rezultatul conversiei din registrul de memorie ADC (ADCMEM0).
            ADC_Result[ADC_i] = ADCMEM0;
            // Logica pentru a gestiona indexul `ADC_i`.
            // Pare a fi configurat pentru a citi secvential canalele A2, A1, A0,
            // desi configurarea ADC este pentru repetare secventa pe un singur canal (A2 initial).
            // Aceasta logica ar putea fi un vestigiu sau necesita o configurare ADC diferita
            // pentru a functiona cum este intentionat (ex: ADCMCTL0 actualizat dinamic).
            if(ADC_i == 0)
            {
                __no_operation(); // Punct de oprire sau placeholder.
                ADC_i = 2;        // Reseteaza indexul la 2.
            }
            else
            {
                ADC_i--;          // Decrementeaza indexul.
            }
            break;
        default:
            break;
    }
}


// Rutina de tratare a intreruperilor pentru UART eUSCI_A1 (USCI_A1_VECTOR).
// Se executa la primirea unui caracter prin UART sau la alte evenimente UART.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void Rutina_USCI_A1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) Rutina_USCI_A1(void)
#else
#error Compiler not supported!
#endif
{
  char received_char; // Variabila locala pentru stocarea caracterului receptionat.
  // Utilizeaza `__even_in_range` pentru a optimiza gestionarea surselor de intrerupere UART.
  // UCA1IV (eUSCI_A1 Interrupt Vector) contine codul sursei de intrerupere.
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break; // Nicio intrerupere.
    case USCI_UART_UCRXIFG: // Flag de receptie (UCRXIFG) - un caracter a fost primit.
        received_char = UCA1RXBUF; // Citeste caracterul din buffer-ul de receptie.

        // Ecoul caracterului: trimite inapoi caracterul primit pentru a fi vizibil in terminalul utilizatorului.
        while(!(UCA1IFG&UCTXIFG)); // Asteapta ca buffer-ul de transmisie sa fie gol.
        UCA1TXBUF = received_char;   // Trimite caracterul.

        // Masina de stari pentru procesarea intrarii de la utilizator.
        if (uart_rx_state == FREQUENCY_INPUT_MODE) // Daca suntem in modul de introducere a frecventei.
        {
            if (received_char == '\r' || received_char == '\n') // Tasta Enter a fost apasata.
            {
                uart_send_string("\r\n"); // Trimite newline.
                if (freq_input_buffer_idx > 0) // Daca s-a introdus ceva.
                {
                    freq_input_buffer[freq_input_buffer_idx] = '\0'; // Adauga terminatorul null la sir.
                    unsigned long requested_freq_hz = atoi(freq_input_buffer); // Converteste sirul in numar.

                    // Valideaza frecventa introdusa (1 Hz - 10000 Hz).
                    if (requested_freq_hz >= 1 && requested_freq_hz <= 10000)
                    {
                        unsigned long calc_increment;
                        // Calculeaza incrementul necesar pentru TB2CCR0 pentru a obtine frecventa dorita.
                        // Frecventa semnalului = SMCLK / (calc_increment * Numar_Esantioane)
                        // calc_increment = SMCLK / (Frecventa_Dorita * Numar_Esantioane)
                        calc_increment = 24000000UL / requested_freq_hz; // SMCLK (24MHz) / Frecventa_Dorita
                        calc_increment /= 300UL; // imparte la numarul de esantioane (300).

                        // Limiteaza valoarea incrementului pentru a evita frecvente nerealizabile sau overflow.
                        const unsigned int MIN_CCR_INCREMENT = 7; // Limita inferioara pentru increment (frecventa maxima).
                        const unsigned int MAX_CCR_INCREMENT = 65535; // Limita superioara (valoare maxima pentru un registru de 16 biti).

                        if (calc_increment < MIN_CCR_INCREMENT) {
                            timer_b0_ccr0_increment = MIN_CCR_INCREMENT;
                        } else if (calc_increment > MAX_CCR_INCREMENT) {
                            timer_b0_ccr0_increment = MAX_CCR_INCREMENT;
                        } else {
                            timer_b0_ccr0_increment = (unsigned int)calc_increment;
                        }

                        // Calculeaza si afiseaza frecventa reala obtinuta cu noul increment.
                        // Se foloseste calculul in milihertz pentru o mai buna precizie a afisarii.
                        unsigned long term_calc = (24000000UL / 300UL) * 1000UL; // (SMCLK / Esantioane) * 1000
                        unsigned long actual_freq_milihz = term_calc / (unsigned long)timer_b0_ccr0_increment;

                        unsigned int actual_hz = actual_freq_milihz / 1000; // Partea intreaga a frecventei.
                        unsigned int actual_tenths = (actual_freq_milihz % 1000) / 100; // Prima zecimala.

                        uart_send_string("Target: "); uart_send_uint(requested_freq_hz); uart_send_string("Hz. ");
                        uart_send_string("Actual: "); uart_send_uint(actual_hz); uart_send_string("."); uart_send_uint(actual_tenths);
                        uart_send_string("Hz (CCR Inc: "); uart_send_uint(timer_b0_ccr0_increment); uart_send_string(")\r\n> ");
                    }
                    else // Frecventa invalida.
                    {
                        // Frecventa maxima teoretica cu SMCLK=24MHz, 300 esantioane, CCR_inc=7 este 24.000.000 / (7 * 300) = ~11428 Hz.
                        uart_send_string("Frecventa invalida. Domeniu valid: 1-10000 Hz.\r\nDomeniu efectiv aprox 1-11428 Hz.\r\n> ");
                    }
                } else { // Nu s-a introdus nicio frecventa.
                     uart_send_string("Nu s-a introdus nicio frecventa.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE; // Revine la modul normal de comanda.
                freq_input_buffer_idx = 0; // Reseteaza indexul buffer-ului.
                memset(freq_input_buffer, 0, FREQ_INPUT_BUFFER_SIZE); // sterge buffer-ul.
            }
            // Daca s-a primit o cifra si buffer-ul nu este plin.
            else if (received_char >= '0' && received_char <= '9' && freq_input_buffer_idx < (FREQ_INPUT_BUFFER_SIZE - 1))
            {
                freq_input_buffer[freq_input_buffer_idx++] = received_char; // Adauga cifra la buffer.
            }
            // Daca s-a primit Backspace (cod ASCII 8 sau 127) si buffer-ul nu este gol.
            else if ((received_char == '\b' || received_char == 127) && freq_input_buffer_idx > 0)
            {
                freq_input_buffer_idx--; // Decrementeaza indexul (sterge logic ultimul caracter).
                uart_send_string(" \b"); // Trimite spatiu si backspace pentru a sterge vizual in terminal.
            }
        }
        else if (uart_rx_state == AMPLITUDE_INPUT_MODE) // Daca suntem in modul de introducere a amplitudinii.
        {
            if (received_char == '\r' || received_char == '\n') // Tasta Enter.
            {
                uart_send_string("\r\n");
                if (amplitude_input_buffer_idx > 0) // Daca s-a introdus ceva.
                {
                    amplitude_input_buffer[amplitude_input_buffer_idx] = '\0'; // Termina sirul.
                    int requested_amplitude = atoi(amplitude_input_buffer); // Converteste la numar.

                    // Valideaza amplitudinea (0 - 100 %).
                    if (requested_amplitude >= 0 && requested_amplitude <= 100)
                    {
                        current_amplitude_percentage = (unsigned int)requested_amplitude;
                        // Actualizeaza factorul de scalare precalculat pentru ISR.
                        current_amplitude_scaling_factor = ((long)current_amplitude_percentage * 256) / 100;
                        uart_send_string("Amplitudine setata la: ");
                        uart_send_uint(current_amplitude_percentage);
                        uart_send_string(" %\r\n> ");
                    }
                    else // Amplitudine invalida.
                    {
                        uart_send_string("Amplitudine invalida. Introduceti o valoare intre 0-100 %.\r\n> ");
                    }
                } else { // Nu s-a introdus nicio amplitudine.
                     uart_send_string("Nu s-a introdus nicio amplitudine.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE; // Revine la modul normal.
                amplitude_input_buffer_idx = 0; // Reseteaza buffer-ul.
                memset(amplitude_input_buffer, 0, AMPLITUDE_INPUT_BUFFER_SIZE);
            }
            // Daca s-a primit o cifra.
            else if (received_char >= '0' && received_char <= '9' && amplitude_input_buffer_idx < (AMPLITUDE_INPUT_BUFFER_SIZE - 1))
            {
                amplitude_input_buffer[amplitude_input_buffer_idx++] = received_char;
            }
            // Daca s-a primit Backspace.
            else if ((received_char == '\b' || received_char == 127) && amplitude_input_buffer_idx > 0)
            {
                amplitude_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else if (uart_rx_state == OFFSET_INPUT_MODE) // Daca suntem in modul de introducere a offset-ului.
        {
            if (received_char == '\r' || received_char == '\n') // Tasta Enter.
            {
                uart_send_string("\r\n");
                if (offset_input_buffer_idx > 0) // Daca s-a introdus ceva.
                {
                    offset_input_buffer[offset_input_buffer_idx] = '\0'; // Termina sirul.
                    // Foloseste atol pentru a permite numere mai mari daca ar fi necesar, desi int este suficient pentru 0-3300.
                    long requested_offset_mv = atol(offset_input_buffer);

                    // Valideaza offset-ul (0 - 3300 mV, presupunand referinta DAC de 3.3V).
                    if (requested_offset_mv >= 0 && requested_offset_mv <= 3300)
                    {
                        current_offset_mv = (unsigned int)requested_offset_mv;
                        // Actualizeaza valoarea offset-ului precalculata in unitati DAC pentru ISR.
                        precalculated_offset_dac_units = ((long)current_offset_mv * 4095L) / 3300L;
                        uart_send_string("Offset setat la: ");
                        uart_send_uint(current_offset_mv);
                        uart_send_string(" mV\r\n> ");
                    }
                    else // Offset invalid.
                    {
                        uart_send_string("Offset invalid. Introduceti o valoare intre 0-3300 mV.\r\n> ");
                    }
                } else { // Nu s-a introdus niciun offset.
                     uart_send_string("Nu s-a introdus nicio valoare pentru offset.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE; // Revine la modul normal.
                offset_input_buffer_idx = 0; // Reseteaza buffer-ul.
                memset(offset_input_buffer, 0, OFFSET_INPUT_BUFFER_SIZE);
            }
            // Daca s-a primit o cifra.
            else if (received_char >= '0' && received_char <= '9' && offset_input_buffer_idx < (OFFSET_INPUT_BUFFER_SIZE - 1))
            {
                offset_input_buffer[offset_input_buffer_idx++] = received_char;
            }
            // Daca s-a primit Backspace.
            else if ((received_char == '\b' || received_char == 127) && offset_input_buffer_idx > 0)
            {
                offset_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else if (uart_rx_state == PHASE_INPUT_MODE) // Daca suntem in modul de introducere a fazei.
        {
            if (received_char == '\r' || received_char == '\n') // Tasta Enter.
            {
                uart_send_string("\r\n");
                if (phase_input_buffer_idx > 0) // Daca s-a introdus ceva.
                {
                    phase_input_buffer[phase_input_buffer_idx] = '\0'; // Termina sirul.
                    int requested_phase = atoi(phase_input_buffer); // Converteste la numar.

                    // Valideaza faza (0 - 299 esantioane, deoarece sunt 300 de esantioane indexate 0-299).
                    if (requested_phase >= 0 && requested_phase < 300)
                    {
                        current_phase_offset_samples = (unsigned int)requested_phase;
                        uart_send_string("Faza setata la: ");
                        uart_send_uint(current_phase_offset_samples);
                        uart_send_string(" esantioane\r\n> ");
                    }
                    else // Faza invalida.
                    {
                        uart_send_string("Faza invalida. Introduceti o valoare intre 0-299 esantioane.\r\n> ");
                    }
                } else { // Nu s-a introdus nicio faza.
                     uart_send_string("Nu s-a introdus nicio valoare pentru faza.\r\n> ");
                }
                uart_rx_state = NORMAL_COMMAND_MODE; // Revine la modul normal.
                phase_input_buffer_idx = 0; // Reseteaza buffer-ul.
                memset(phase_input_buffer, 0, PHASE_INPUT_BUFFER_SIZE);
            }
            // Daca s-a primit o cifra.
            else if (received_char >= '0' && received_char <= '9' && phase_input_buffer_idx < (PHASE_INPUT_BUFFER_SIZE - 1))
            {
                phase_input_buffer[phase_input_buffer_idx++] = received_char;
            }
            // Daca s-a primit Backspace.
            else if ((received_char == '\b' || received_char == 127) && phase_input_buffer_idx > 0)
            {
                phase_input_buffer_idx--;
                uart_send_string(" \b");
            }
        }
        else // NORMAL_COMMAND_MODE: Proceseaza comenzi de un singur caracter.
        {
            if (received_char == 'F' || received_char == 'f') // Comanda pentru setarea frecventei.
            {
                uart_rx_state = FREQUENCY_INPUT_MODE; // Treci la modul de introducere a frecventei.
                freq_input_buffer_idx = 0; // Reseteaza buffer-ul de frecventa.
                memset(freq_input_buffer, 0, FREQ_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nIntroduceti Frecventa (1-10000Hz), apoi Enter: ");
            }
            else if (received_char == 'A' || received_char == 'a') // Comanda pentru setarea amplitudinii.
            {
                uart_rx_state = AMPLITUDE_INPUT_MODE; // Treci la modul de introducere a amplitudinii.
                amplitude_input_buffer_idx = 0;
                memset(amplitude_input_buffer, 0, AMPLITUDE_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nIntroduceti Amplitudinea (0-100%), apoi Enter: ");
            }
            else if (received_char == 'O' || received_char == 'o') // Comanda pentru setarea offset-ului.
            {
                uart_rx_state = OFFSET_INPUT_MODE; // Treci la modul de introducere a offset-ului.
                offset_input_buffer_idx = 0;
                memset(offset_input_buffer, 0, OFFSET_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nIntroduceti Offset-ul (0-3300mV), apoi Enter: ");
            }
            else if (received_char == 'H' || received_char == 'h') // Comanda pentru setarea fazei.
            {
                uart_rx_state = PHASE_INPUT_MODE; // Treci la modul de introducere a fazei.
                phase_input_buffer_idx = 0;
                memset(phase_input_buffer, 0, PHASE_INPUT_BUFFER_SIZE);
                uart_send_string("\r\nIntroduceti Faza (0-299 esantioane), apoi Enter: ");
            }
            else if (received_char == '#') // Comanda pentru trimiterea datelor ADC.
            {
                // Formateaza valorile ADC din `ADC_Result` in sirurile de caractere `ADC_char`.
                for(j=0;j<3;j++) // Pentru fiecare din cele 3 canale/rezultate ADC.
                {
                   ADC_char[j][2] = j+48; // Seteaza numarul canalului (Ax).
                   // Extrage si converteste cifrele valorii ADC.
                   ADC_char[j][4] = ((ADC_Result[j]/1000)%10)+48; // Cifra miilor.
                   ADC_char[j][6] = ((ADC_Result[j]/100)%10)+48;  // Cifra sutelor.
                   ADC_char[j][5] = ((ADC_Result[j]/10)%10)+48;   // Cifra zecilor.
                   ADC_char[j][7] = (ADC_Result[j]%10)+48;    // Cifra unitatilor.
                }
                // Trimite sirurile formatate prin UART.
                for(j=0;j<3;j++) {
                    for(i=0;i<12;i++) { // Presupunand ca fiecare linie ADC_char are 12 caractere (incl. CRLF, null).
                        while(!(UCA1IFG&UCTXIFG));
                        UCA1TXBUF = ADC_char[j][i];
                    }
                }
                uart_send_string("\r\n> "); // Trimite prompt.
            }
            // Comenzi pentru atribuirea formelor de unda la slotul de atribuire curent.
            else if(received_char == 's') { // Sinus.
                waveform_slots[current_assignment_slot_index] = sinus;
                uart_send_string("\r\nSinus atribuit Slotului "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 't') { // Triunghi.
                waveform_slots[current_assignment_slot_index] = triunghi;
                uart_send_string("\r\nTriunghi atribuit Slotului "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'r') { // Rampa.
                waveform_slots[current_assignment_slot_index] = rampa;
                uart_send_string("\r\nRampa atribuita Slotului "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'p') { // Dreptunghiulara (patrat).
                waveform_slots[current_assignment_slot_index] = patrat;
                uart_send_string("\r\nDreptunghiulara atribuita Slotului "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            else if(received_char == 'c') { // Tren de impulsuri.
                waveform_slots[current_assignment_slot_index] = tren_impulsuri;
                uart_send_string("\r\nTren de impulsuri atribuit Slotului "); uart_send_uint(current_assignment_slot_index); uart_send_string(".\r\n> ");
            }
            // Comenzi pentru selectarea slotului de atribuire.
            else if(received_char == 'X' || received_char == 'x') { // Slot 0.
                current_assignment_slot_index = 0;
                uart_send_string("\r\nSlot 0 pentru atribuire.\r\n> ");
            }
            else if(received_char == 'Y' || received_char == 'y') { // Slot 1.
                current_assignment_slot_index = 1;
                uart_send_string("\r\nSlot 1 pentru atribuire.\r\n> ");
            }
            else if(received_char == 'Z' || received_char == 'z') { // Slot 2.
                current_assignment_slot_index = 2;
                uart_send_string("\r\nSlot 2 pentru atribuire.\r\n> ");
            }
            // Comenzi pentru selectarea slotului de iesire DAC.
            else if(received_char == 'L' || received_char == 'l') { // Slot 0.
                current_output_slot_index = 0;
                uart_send_string("\r\nSlot 0 pentru iesire DAC.\r\n> ");
            }
            else if(received_char == 'M' || received_char == 'm') { // Slot 1.
                current_output_slot_index = 1;
                uart_send_string("\r\nSlot 1 pentru iesire DAC.\r\n> ");
            }
            else if(received_char == 'N' || received_char == 'n') { // Slot 2.
                current_output_slot_index = 2;
                uart_send_string("\r\nSlot 2 pentru iesire DAC.\r\n> ");
            }
            else if (received_char == '\r' || received_char == '\n') // Daca se primeste doar Enter.
            {
                uart_send_string("\r\n> "); // Trimite un nou prompt.
            }
            else // Comanda necunoscuta.
            {
                uart_send_string("\r\nComanda necunoscuta.\r\n> ");
            }
        }
      break; // Sfarsitul cazului USCI_UART_UCRXIFG.
    case USCI_UART_UCTXIFG: break;    // Flag de transmisie (buffer gol) - nu necesita actiune aici.
    case USCI_UART_UCSTTIFG: break;   // Flag de start bit - nu necesita actiune aici.
    case USCI_UART_UCTXCPTIFG: break; // Flag de transmisie completa - nu necesita actiune aici.
    default: break;
  }
}

// Rutina de tratare a intreruperilor pentru Timer B2, CCR0 (TIMER2_B0_VECTOR).
// Aceasta este rutina critica pentru generarea semnalului DAC.
// Se declanseaza la fiecare `timer_b0_ccr0_increment` cicluri SMCLK.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_B0_VECTOR
__interrupt void Timer2_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_B0_VECTOR))) Timer2_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // Programeaza urmatoarea intrerupere adaugand incrementul la valoarea curenta a TB2CCR0.
    // Acest lucru mentine o temporizare precisa chiar daca exista mici variatii in timpul de executie al ISR-ului.
    TB2CCR0 += timer_b0_ccr0_increment;

    // Avanseaza la urmatorul esantion din tabelul de forma de unda.
    current_waveform_index++;
    if(current_waveform_index >= 300) // Daca s-a ajuns la sfarsitul tabloului (300 de esantioane).
        current_waveform_index = 0;   // Revine la inceputul tabloului.

    // Aplica defazajul (phase offset) la indexul curent al formei de unda.
    unsigned int temp_phased_index = current_waveform_index + current_phase_offset_samples;
    if (temp_phased_index >= 300) { // Asigura ca indexul cu defazaj ramane in limitele tabloului (0-299).
        temp_phased_index -= 300;   // Wrap-around.
    }
    unsigned int phased_waveform_index = temp_phased_index; // Indexul final, cu defazaj aplicat.

    // Preia esantionul brut (raw sample) din tabelul de forma de unda selectat pentru iesire,
    // folosind indexul cu defazaj.
    unsigned int raw_sample = waveform_slots[current_output_slot_index][phased_waveform_index];
    // Punctul de mijloc teoretic al datelor brute din tabel (pentru un semnal bipolar pe 12 biti, 0-4095,
    // 2048 ar reprezenta 0V analogic daca semnalul este centrat).
    long dac_raw_midpoint = 2048;

    // Proces de calcul al valorii finale pentru DAC in 5 pasi:
    // 1. Face esantionul relativ la punctul sau de mijloc original.
    //    Acest lucru transforma esantionul intr-o valoare AC virtuala, centrata in jurul lui 0.
    long sample_relative_to_raw_mid = (long)raw_sample - dac_raw_midpoint;

    // 2. Scaleaza amplitudinea componentei AC.
    //    Foloseste factorul de scalare precalculat `current_amplitude_scaling_factor`.
    //    `current_amplitude_scaling_factor` este `(procent_amplitudine * 256) / 100`.
    //    Multiplicarea cu acest factor si apoi deplasarea la dreapta cu 8 biti (`>> 8`)
    //    este o metoda optimizata echivalenta cu `* procent_amplitudine / 100`.
    long scaled_relative_sample = (sample_relative_to_raw_mid * current_amplitude_scaling_factor) >> 8;

    // 3. Utilizeaza valoarea de offset DC precalculata in unitati DAC.
    long offset_dac_units = precalculated_offset_dac_units;

    // 4. Adauga componenta AC scalata la noul offset DC.
    //    Aceasta deplaseaza semnalul AC scalat la nivelul de offset dorit.
    long final_value_long = offset_dac_units + scaled_relative_sample;

    // 5. Limiteaza (clamp) valoarea finala la intervalul valid al DAC-ului (0-4095 pentru 12 biti).
    if (final_value_long < 0) final_value_long = 0;
    if (final_value_long > 4095) final_value_long = 4095;

    // Scrie valoarea finala calculata in registrul de date al DAC-ului din SAC1.
    // Aceasta genereaza tensiunea analogica corespunzatoare la iesirea DAC-ului.
    SAC1DAT = (unsigned int)final_value_long;
}

// Rutina de tratare a intreruperilor pentru Timer B2, CCR1-CCR6 si flag-ul de overflow (TIMER2_B1_VECTOR).
// in acest cod, este folosita doar pentru CCR1.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER2_B1_VECTOR
__interrupt void Timer2_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER2_B1_VECTOR))) Timer2_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    // TB2IV (Timer B2 Interrupt Vector) contine codul sursei de intrerupere.
    switch(TB2IV)
    {
        case TBIV__NONE:    // Nicio intrerupere.
            break;
        case TBIV__TBCCR1:  // intrerupere de la TB2CCR1.
            TB2CCR1 += 10;  // Incrementeaza TB2CCR1. Scopul exact nu este clar definit
                            // in contextul principal al generatorului de forme de unda.
                            // Ar putea fi pentru o functionalitate secundara de temporizare.
            break;
        case TBIV__TBIFG:   // intrerupere de overflow a timer-ului (TB2R ajunge la 0xFFFF).
            break;          // Nu este tratata explicit.
        default:
            break;
    }
}

// Functie pentru calibrarea fina a oscilatorului DCO (Digitally Controlled Oscillator).
// Aceasta este o functie standard furnizata de Texas Instruments pentru a ajusta DCO-ul
// astfel incat frecventa sa sa fie cat mai apropiata de cea tintita de FLL (Frequency Locked Loop).
void Software_Trim()
{
    // Variabile locale pentru procesul de ajustare.
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3; // Valoare initiala pentru DCOFTRIM.
    unsigned char endLoop = 0;    // Flag pentru terminarea buclei.

    do
    {
        // Seteaza DCOTAP la o valoare de pornire (256).
        CSCTL0 = 0x100;
        do
        {
            // sterge flag-ul de eroare DCO (DCOFFG) pana cand acesta ramane sters,
            // indicand ca DCO-ul s-a stabilizat.
            CSCTL7 &= ~DCOFFG;
        }while (CSCTL7 & DCOFFG);

        // Asteapta un numar de cicluri pentru ca FLL sa se blocheze.
        // intarzierea depinde de frecventa MCLK.
        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ); // MCLK_FREQ_MHZ este 24.

        // Asteapta pana cand FLL-ul nu mai este deblocat (FLLUNLOCK0/1 sunt 0)
        // sI nu exista eroare DCO (DCOFFG este 0).
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        // Citeste valorile actuale ale registrelor de control al ceasului.
        csCtl0Read = CSCTL0; // Contine DCOTAP.
        csCtl1Read = CSCTL1; // Contine DCOFTRIM.

        oldDcoTap = newDcoTap; // Salveaza valoarea anterioara a DCOTAP.
        newDcoTap = csCtl0Read & 0x01ff; // Extrage noua valoare DCOTAP (9 biti).
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4; // Extrage valoarea curenta DCOFTRIM (3 biti).

        // Logica de ajustare a DCOFTRIM bazata pe valoarea DCOTAP.
        // Scopul este de a aduce DCOTAP cat mai aproape de 256.
        if(newDcoTap < 256) // Daca DCOTAP este sub tinta.
        {
            newDcoDelta = 256 - newDcoTap; // Calculeaza diferenta.
            // Verifica daca s-a trecut peste tinta (oscilatie).
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256))
                endLoop = 1; // Termina bucla daca s-a oscilat.
            else
            {
                dcoFreqTrim--; // Decrementeaza DCOFTRIM pentru a creste frecventa DCO.
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4); // Aplica noul DCOFTRIM.
            }
        }
        else // Daca DCOTAP este peste tinta.
        {
            newDcoDelta = newDcoTap - 256; // Calculeaza diferenta.
            // Verifica daca s-a trecut peste tinta (oscilatie).
            if(oldDcoTap < 256)
                endLoop = 1; // Termina bucla daca s-a oscilat.
            else
            {
                dcoFreqTrim++; // Incrementeaza DCOFTRIM pentru a scadea frecventa DCO.
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4); // Aplica noul DCOFTRIM.
            }
        }

        // Salveaza setarile care au produs cea mai mica diferenta (bestDcoDelta).
        if(newDcoDelta < bestDcoDelta)
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0); // Continua pana cand se detecteaza o oscilatie sau se atinge un optim.

    // Restaureaza cele mai bune setari gasite pentru CSCTL0 si CSCTL1.
    CSCTL0 = csCtl0Copy;
    CSCTL1 = csCtl1Copy;
    // Asteapta ca FLL sa se blocheze din nou cu noile setari.
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
} // Sfarsitul functiei Software_Trim
