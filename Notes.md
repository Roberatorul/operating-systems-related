# CPU virtualization

## Memory
`volatile memory` - RAM-ul: este o memorie rapida insa atunci cand se opreste curentul se pierd datele  
`non volatile memory` - HDD SSD: retin date si dupa ce se opreste curentul; sunt cu mult mai incete decat RAM-ul

## LDE (Limited Direct Execution Protocol)
### Trap-table  
La boot-ul sistemului, OS-ul creeaza in memorie un `trap-table` (este un array de pointeri) unde salveaza adresele diferitor actiuni (din **kernel mode**) care sa fie accesate atunci cand un program face ceva in **user mode**. Spre exemplu daca un utilizator incearca sa faca o impartire la 0 (care spre exemplu este la adresa 0 din trap table), procesorul detecteaza acest lucru, trece in kernel mode si sare la acea adresa care este, spre exemplu, divided_by_zero_handler() si executa codul acelei functii.  
Acest mecanism impiedica utilizatorul sa sara unde ar dori el in kernel mode pt a executa cod. Acest lucru se aplica  pnetru multe lucruri, printre care si pentru `syscall-uri`. Cand apelezi un sycall, defapt faci o cerere de accesare a adresei 128 (spre exemplu) din trap table.

### Rularea unei actiuni privilegiate
Exista 2 faze:
 * `la boot`: **kernel-ul** initializeaza **trap table-ul**, iar CPU-ul retine adresa la care acesta este salvat
 * `la rularea unui proces`:
	 * kernelul aloca spatiu pentru un nod pe care il introduce in **lista proceselor**
	 * aloca memorie pentru proces
	 * cand vrea sa execute un proces privilegiat, acesta trece in kernel-mode si executa actiunea (descrisa mai pe larg in sectiunea `Trap-table` de mai sus, iar apoi face un `return-from-trap`)

### Switching between processes
**Disclaimer:** Aici discutia este despre un CPU cu un singur core, la CPU-urile multicore pot sta mai multe lucruri pe procesor (atatea cate core-uri avem), insa baza este aceeasi de jos:   
Sistemul de operare nu sta 100% pe procesor. Cand este intreprinsa o actiune, aceasta il scoate de pe procesor (**context switch**) si pune acea actiune pe procesor.  
Ca sa recapete controlul exista 2 metode:
 * atunci cand procesul face o actiune privilegiata, acesta reda procesorul OS-ului care apoi decide daca sa o realizeze sau sa fac schimb si sa puna alt proces pe CPU; o mare problema cu aceasta abordare sunt procesele care spre exemplu nu fac actiuni privilegiate sau loop-urile infinite (**metoda veche, nu mai este folosita din cate am inteles**)
 * `la boot` OS-ul seteaza un timer pe placa de baza (`timer interrupt`) la care aceasta sa trimita un semnal si sa puna OS-ul pe CPU (**metoda folosita actual**)
**Note:** Mecanismul in care se apeleaza timer-ul si trece inapoi OS-ul pe CPU se numeste `mode switch`, `context switch` realizand-se atunci cand OS-ul schimba starea unui proces (din ready in running, ready in blocked etc).

### Other problems
Concurenta este de asemenea o problema: ce se intampla atunci cand in timpul unui syscall apare un timmer interupt?  
Pentru astfel de probleme exista diferite mecanisme de sincronizare.

## Scheduler
### Notiuni generale
Exista 2 tipuri de analizare a unui scheduler:
 * performanta - se realizeaza prin calulul turnaround-ului
 * fairness - toate procesele sa ruleze la fel de mult pe procesor

Pentru a spori performanta procesele cu timp scurt de executie trebuie sa ruleze inaintea celor cu timp ridicat de executie.

Exista 2 tipuri de scheduler:
 * `non-preemptive scheduler` - lasa un proces sa ruleze pana la finalizarea sa
 * `preemptive scheduler` - face context switch-uri pentru a lasa un alt proces sa ruleze pe procesor

Tipuri de metrici:
 * `turnaround` 
	 * T_turnaround = T_completition - T_arrival;
	 * bun pentru performanta bruta a sistemului
 * `response time`
	 * T_response = T_firstrun - T_arrival (T_fristrun = momentul in care a rulat pe CPU; T_arrival = momentul in care a inceput procesul)ș
	 * bun pentru interactivitate (e.g. daca suntem in terminal si tastam ceva nu am vrea sa asteptam 10 secunde pentru ca alte procese au fost scheduled inaintea noastra)

## Scheduling algorithms

### FIFO
Foloseste principiul unei cozi: procesele ruleaza in ordinea in care au ajuns in coada. Nu tine 
de performante.

### Shortest Job First (SJF)
Turnaround-ul beneficiaza de pe urma faptului ca daca rulezi procese ce au un timp redus de rulare, turnaround-ul va beneficia major din cauza acestui lucru.  
Nu tine cont de faptul ca la inceput poate veni un proces cu timp mare, iar pe parcurs sa apara procese cu un timp redus, astfel turnaround-ul beneficiind daca acestea ar rula primele.

### Shortest Time-to-Completition First (STCF)
La fel ca si **SJF** insa tine cont ca pot sa ajunga procese cu timp de execuitie mai mic, astfel acesta oprind procesul initial din rulare si punandu-l pe celalalt cu timp mai scurt care a ajuns (context switch)

### Round Robin Algorithm (RR)
Acesta algoritm se bazeaza pe setarea unui time slice fiecarui proces. Cu cat time slice ul este mai mic, performantele sunt mai bune, insa este de luat in considerare si costul unui context-switch.  
Acest algoritm incearca sa rezolve problema response time-ului, insa nu este ideal pentru turnaround.

> [!NOTE] Pana in acest moment avem la baza faptul ca algoritmii discutati stiu cat ruleaza fiecare proces, acest lucru fiind imposibil in practica.

### Multi-Level Feedback Queue (MLFQ)
Este alcatuit din mai multe cozi (fiecare coada avand cate o prioritate). Procesele sunt puse in aceste cozi. Exista 2 mecanisme:
 * if priority(A) > priority(B) => A runs (B doesn't)
 * if priority(A) == priority(B) => A & B run in RR

Scheduler-ul "invata" pe parcurs cum ar trebui sa se ocupe de procese:
 * daca un proces este **I/O intensive**, acesta primeste o prioritate mare deoarece oricum va lasa procesorul atunci cand face operatii blocante
 * daca un proces este **CPU intensive**, atunci acest primeste o prioritate redusa

Aceasta metoda ofera unui nou proces prioritate maxima atunci cand sosete, iar cu timpul, cat acesta ruleaza pe procesor, prioritatea sa incepe sa fie scazuta, cazand pe cozile inferioare.  
Daca un proces realizeaza o operatie blocanta inainte de a i se duce `allotment-ul`, acesta ramane pe acelasi nivel, insa nu i se reseteaza allotment-ul. Acest ultim lucru previne notiunea de `gaming the scheduler` (daca dupa fiecare operatie I/O i s ar reseta, atunci cineva ar putea face operatii I/O pentru a isi pastra locul sus in coada).  
Pentru a oferi proceselor de pe nivelele inferioare o sansa la a rula pe CPU (daca pe sistem apar prea multe procese interactive ar putea bloca de tot procesele de pe nivelele inferioare), la o anumita perioada de timp S (in majoritatea cazurilor este determinata de administratorul de sistem sau pe baza tehnicilor de ML) toate procesele sunt mutatea pe cea mai de sus coada. Acest lucuru ofera o sansa si proceselor CPU intensive de a mai rula pe procesor. 
> [!NOTE] Parametrii acestei cozi (nr de cozi, time slice urile pentru fiecare, perioada S) sunt variabile. Pentru mai multe detalii si eventuale alte implementari vezi capitolul 8.5 din carte.

### Proportional Share
Acesta se bazeaza pe `fairness-ul` dintre procese, nu pe turnaround sau pe response time. La baza, fiecarui proces i se aloca un numar de tickete.  
Metode de scheduling:
 *  `lottery scheduling`
	 * e.g.: procesul A are ticketele de la 0 la 75, iar B de la 76 la 99. Se alege un numar random iar apoi procesul ce contine ticket-ul cu numarul respectiv ruleaza pe procesor
 * `stride scheduling`
	 * sa zicem ca avem procesele A (100 tickets), B (50 tickets) si C (250 tickets)
	 * alegem o valoare mare, e.g. 10_000
	 * obtinem **stride value-urile:** A(100), B(200), C(40)
	 * la inceput, fiecare proces are un **pass value** setat la 0, astfel orice proces poate incepe; sa zicem ca incepe A. Dupa aceea A->pass devine 100, dupa pe aceeasi logica ruleaza B si C cu B->pass = 200 si C->pass = 40. Apoi este ales procesul cu pass-ul cel mai mic, adica C, si acesta este rulat din nou, iar apoi procesul contiuna
	 * **!Problema:** Daca in joc apare un nou proces, pass-ul acestuia este 0, deci acesta ar rula mult pe procesor, tinand cont ca celelalte procese deja ruleaza de ceva timp
 * `Linux Completely Fair Scheduler`
	 * **Chapther 9.2 from the book**

# Memory Virtualization
> [NOTE!] Cand facem refereire la un sistem pe 32 de biti (sau 64) ne referim la cate adrese poti fi stocate. Acest lucru indica faptul ca fiecarei adrese ii corespunde o "casuta" din memoria RAM care are dimensiunea de 1 byte (8 biti). De accea pe un sistem de 32 de biti nu putem avea mai mult de 2^32(bytes) / 2^30(conversie la GB) = 2^2GB = 4GB RAM. Pe un sistem pe 64 avem 2^64 / 2^30 = 2^34GB RAM.

Tipuri de fragmentare:
 * `external fragmentation` - se refera la conceptul in care desi exista spatiu total in memorie pentru o alocare, alocarea nu poate fi facuta pentru ca acel spatiu liber nu este continuu
 * `internal fragmentation` - deoarece memoria este impartita in pagine de (obicei) 4KB, desi tu ceri sistemului de operare doar 6kb, acesta iti va oferi 2 pagini, adica un total de 8KB

Interesting chapters:
 * `Chapter 17. Free Space Management` - discusses memory allocators (free list)

## Paging
Pe un sistem de 64 de biti avem:
```bash
+-----------------------------------------------------+
virtual page number(52 bits)        | offset (12 bits)
+-----------------------------------------------------+

offset = la al catelea byte din pagina se afla adresa cautata
virtual page number = nr. paginii la care facem referire

In realitate s-ar putea folosi doar 48 sau 57 de biti din virtual page number, restul fiind folositi de sistemul de operare pentru altceva.

Cand se face translatarea, VPN este singurul translatat in memorie reala, offset-ul ramanand acelasi.
```

## Translation-lookaside buffer (TLB)
Este un mecanism de caching pentru translatarile de adrese virtuale in fizice.

```c
int vac[1001];
int sum = 0;
for (int i = 0; i < 1000; i++) {
    sum = sum + i;
	vec[i] +=i;
}
```
 * `temporal locality`
	 * variabilele sum si i pot beneficia de temporal locality pentru ca sunt accesate de foarte multe ori
 * `spatial locality`
	 * cand accesam vec[0] (sau alta valoare din vector), in TLB se mai salveaza si vecinii urmatori (1, 2, ...), TLB ul intuind ca am vrea sa le accesam si pe acestea in viitorul apropiat, astfel reducand numarul de translatari de memorie

In trecut, atunci cand aveam parte de unt `TLB miss`, hardware-ul era cel care se ocupa de rezolvarea acestuia, insa acum hardware-ul doar ridica o exceptie, OS-ul fiind cel care face un **domain switch** in kernel mode, se duce la **trap-ul** respectiv din **trap table** si rezolva problema.

TLB entry (una conceptuala):
```bash
+---------------------------------------+
	VPN		|	 PFN	|	 other bits
+---------------------------------------+
VPN = Virtual Page Number
PFN = Phisical Page Number
other bits = protection bits, address-space identifier, dirty bit etc
```

> [NOTE!] O problema care trebuie luata in considerare este accea in care TLB-ul este pentru tot sistemul, nu este per proces. Astfel, 2 procese pot avea aceeasi adresa virtuala, insa, logic, sa corespunda unor zone diferite de memorie (nu vorbim aici despre situatiile in care mai multe procese pot share ui o librarie). Atunci ori se da flush la TLB si se pun pe parcurs intrarile pentru procesul 2 afecteaza performanta, fie se foloseste un `address space identifier (ASID)` ca unul dintre camputirle pentru un TLB entry, astfel stiind si carui proces ii apartine acea pagina (de obicei se folosesc 8 biti pentru ASID, cu mult mai putin decat un PID de 32 de biti).

## Page table optimizations
> [NOTE!] Problema paginarii clasice este ca intr-un sistem exista un singur page table liniar pentru intregul spatiu de adrese al unui proces. Spatiul nealocat aflat intre zona de stack si heap necesita intrari in tabelul de pagini, chiar daca acestea sunt marcate ca invalide si nu sunt folosite.

**Problema**:
 * Pe un sistem pe 32 de biti (2^32 bytes) avem 4KB (2^12 bytes) dimensiunea unei pagini si (presupunem) 4 bytes(2^2 bytes) pentru un `PTE(Page Table Entry)`.
```bash
2^32[bytes] / 2^12[bytes] = 2^20[pages]
2^20[pages] * 2^2[bytes] = 2^22[bytes](dimensiunea ocupata daca toate paginile ar fi folosite)
2^22 / 2^20 = 4[MB](conversie pentru dimensiunea de mai sus) 
```
Optimizari:
 * pagini de dimensiune mai mare
	 * pentru o pagina de 16KB scad dimensiunea totala de la 4MB la 1MB, insa pagini mai mari inseamna ca avem mai mult **internal fragmentation**
 * paging and segments (`base and bounds`)
	 * fiecare pagina are o dimensiune variabila; pentru fiecare zona (code, heap, stack etc) se folosesc primii (sa zicem) 3 biti de la dreapta la stanga pentru a marca din ce zona face parte fiecare pagina;
	  * pe logica base and bound, base marcheaza inceputul zonei de memorie, iar bounds limita superioara;
	  * dezavantaje:
	  	 * cum paginile de memorie sunt de dimensiune variabila, poate deveni mai dificil gasirea spatiului in memorie;
		 * daca un program are un heap mare, insa doar cateva parti din el sunt folosite, nr. de PTE va fi crescut
 * `Multi-level Page Table`
	 * se bazeaza pe impartirea tabelului mare in mai multe tabele mici;
	 * se introduce o noua structura numita `Page directory`; aceasta detine, minimal, un bit de validitate care marcheaza daca cel putin una dintre zonele dintr-un tabel mai mic este valida + zona din memorie unde se afla acel tabel; elementele din Page directory se numesc `Page Directory Entries (PDE)`
	 * pentru mai multe detalii vezi `Chapter 20.3`
 * `Inverted Page Tables`
	 * tabelele nu mai sunt per proces, ci pentru intreg sistemul;
	 * in loc sa foloseasca **Virtual Page Number(VPN)** ca index pentru a gasi adresa fizica, IPT-ul este construit invers:
	 	 * tabelul are atatea intrari cat are adrese fizice;
		 * indexul intrarii in tabel reprezinta direct **Phisical Frame Number-ul(PFN)**
		 * O intrare contine: <PID_proces, VPN>
	 * limitarea este data doar de memoria RAM disponibila pe sistem;
	 * pentru cautari rapide se foloseste hash table ul ca structura de date

## Notes from the book
 * In early (or simple) operating systems, the loading process is done eagerly, i.e., all at once before running the program; modern OSes perform the process lazily, i.e., by loading pieces of code or data only as they are
needed during program execution. To truly understand how lazy loading of pieces of code and data works, you’ll have to  understand more about he machinery of paging and swapping, topics we’ll cover in the future when we discuss the
virtualization of memory. For now, just remember that before running anything, the OS clearly must do some work to get
the important program bits from disk into memory.

* As an aside, do note that many architectures (e.g., MIPS, SPARC, x86-64)
now support multiple page sizes. Usually, a small (4KB or 8KB) page
size is used. However, if a “smart” application requests it, a single large
page (e.g., of size 4MB) can be used for a specific portion of the address
space, enabling such applications to place a frequently-used (and large)
data structure in such a space while consuming only a single TLB entry. This type of large page usage is common in database management
systems and other high-end commercial applications. The main reason
for multiple page sizes is not to save page table space, however; it is to
reduce pressure on the TLB, enabling a program to access more of its address space without suffering from too many TLB misses. However, as
researchers have shown [N+02], using multiple page sizes makes the OS
virtual memory manager notably more complex, and thus large pages
are sometimes most easily used simply by exporting a new interface to
applications to request large pages directly