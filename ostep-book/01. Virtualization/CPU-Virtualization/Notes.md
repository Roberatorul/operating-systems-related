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

## Scheduling algorithms (for a single CPU)

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

## Sincronizarea mai multor procesoare
Termeni intalniti pentru mecanismul de caching:
 * `temporal locality` - e.g.: pentru o variabila accesata recent se presupune ca aceasta urmeaza sa fie accesata din nou intr-o perioada scurta de timp (de exemplu i-ul dintr-o bucla for), asa ca aceasta este salvata in cache
 * `spatial locality` - e.g.: pentru un element dintr-un vector se presupune ca vom avea nevoie si de celelalte elemente din apropierea sa, asa ca se incarca mai multe elemente din vector in cache, nu doar elementul pe care l-am accesat
 * `cache coherence` - se refera la modul in care se rezolva problema cache-ul diferit a unor procesoare; daca un thread1 de pe CPU1 modifica o valoare v pe care o are in memorie (variabila pe care o are salvata si thread2 de pe CPU2 in cache), datorita magistralei imbinate pe care acestea comunica, cand se aduce o modificare variabilei v, cache-ul lui CPU2 poate fi anuntat sa isi modifice valoarea, fie sa o scoata din cache (explicatie minimala) 
```bash
=================				=================
|	  CPU1		|				|	  CPU2		|
=================				=================
|	  Cache		|				|	  Cache		|
=================				=================
		|								|
		|								|
		|_______________________________|
						|
						|
						|
			   =====================
			   |	  Memory	   |
			   =====================
```
 * `cache afinity` - se refera la faptul ca atunci cand facem un context switch pentru diferite thread-uril ale diferitor procesoare pe care le avem, TLB-ul va fi curatat, fapt ce reduce performanta programului

## Scheduling algorithms (for multiple CPUs)

### Single-Queue Scheduling (SQMS - single-queue multiprocessor scheduling)
 * pune toate thread-urile intr-o coada de asteptare de executie (la fel ca la **FIFO**)
 * are probleme de **cache afinity** deoarece thread-urile pot rula de diferite procesoare; se poate optimiza acest lucru prin rularea de thread-uri inrudite pe acelasi procesor, reducand TLB flush-ul, insa aceasta implementare poate deveni complicata

### Multi-Queue Scheduling (MQMS - multi-queue multiprocessor scheduling)
 * asemanator cu metoda de mai sus, doar ca apar mai multe cozi; thread-urile care ruleaza sunt impartite pentru fiecare coada in parte, astfel eliminand problema **cache afinity-ului**
 * algoritmul de scheduling pentru fiecare coada poate fi un **Round Robin** sau alta euristica aleasa de cel care implementeaza
 * problema care apare:
	 * o coada poate sa isi termine de procesat sarcinile, astfel intrand in idle, in timp ce cealalta ramane incarcata
 * solutie:
	 * `work stealing` - la un moment dat cand o coada ramane cu putine sarcini de executat, aceasta se uita la celelalte cozi si mai ia din sarcinile ramase
> [NOTE!] Mecanismul de **work stealing** poate introduce un overhead + complexitate prin "uitarea" la celelalte cozi din jur. Implementarea unei MQMS ramane la latitudinea celui care o implementeaza.

###  Linux Multiprocessor Schedulers
 * nu exista un algoritm standard: printre cei mai renumiti sunt:
	 * `the O(1) scheduler`
	 * `CFS`
	 * `BFS` - nu vine de la cautarea pe grafuri bfs
> [NOTE!] Pentru mai multe informatii vezi `Chapter 10.6` pentru cateva explicatii + referinte si `Chapter 9.2` pentru **CFS**.

## Notes from the book
 * In early (or simple) operating systems, the loading process is done eagerly, i.e., all at once before running the program; modern OSes perform the process lazily, i.e., by loading pieces of code or data only as they are
needed during program execution. To truly understand how lazy loading of pieces of code and data works, you’ll have to  understand more about he machinery of paging and swapping, topics we’ll cover in the future when we discuss the
virtualization of memory. For now, just remember that before running anything, the OS clearly must do some work to get
the important program bits from disk into memory.
