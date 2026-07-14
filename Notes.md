# Notite

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

## Notes from the book
 * In early (or simple) operating systems, the loading process is done eagerly, i.e., all at once before running the program; modern OSes perform the process lazily, i.e., by loading pieces of code or data only as they are
needed during program execution. To truly understand how lazy loading of pieces of code and data works, you’ll have to  understand more about he machinery of paging and swapping, topics we’ll cover in the future when we discuss the
virtualization of memory. For now, just remember that before running anything, the OS clearly must do some work to get
the important program bits from disk into memory.