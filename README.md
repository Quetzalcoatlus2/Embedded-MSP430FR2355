# Embedded-MSP430FR2355

# Joc timpi de reacție

Testarea reflexelor este realizata printr-un joc competitiv jucat in doi. La intervale aleatorii de timp, un led se va aprinde pe placa. Jucatorul care isi apasa butonul mai repede dupa aprinderea
led-ului castiga runda si primeste un punct. Dupa ce o runda se termina, alta porneste dupa un alt interval aleatoriu de timp. Jocul continua pana cand unul dintre jucatori ajunge la 5 puncte,
moment in care jucatorul respectiv castiga jocul. Dupa terminarea unui joc poate incepe alt joc cu scorurile resetate daca sunt apasate simultan ambele butoane.

Se afiseaza in UART:
- Mesaj de preintampinare al rundei cu numarul rundei ce urmeaza sa inceapa
- Timpii de reactie ai jucatorilor
- Cu cat timp a fost jucatorul rapid mai rapid decat jucatorul lent (diferenta dintre timpii de reactie)
- Scorul fiecarui jucator
- Mesaj de terminare al jocului cu evidentierea castigatorului si inceperea altui joc


# Generator de semnale

Generatorul de semnale permite configurarea semnalelor prin UART.

Pot fi selectate forme de undă: sinusoidal, triunghi, rampă, dreptunghiular și tren de impulsuri.

Frecvența este stabilă în intervalul 1-344 Hz. 

Amplitudinea este configurată ca procent (100% - 3.3V). 

Componenta continuă este configurată în milivolți (1-3300 mV).

Faza este configurabilă ca număr de eșantioane (1-300 pentru că sunt folosite 300 de eșantioane pentru generarea unei perioade de semnal).

Sunt configurabile 3 sloturi de formă de undă pentru salvarea formelor de undă.
