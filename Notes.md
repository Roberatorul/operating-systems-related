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


## Notes from the book
 * In early (or simple) operating systems, the loading process is done eagerly, i.e., all at once before running the program; modern OSes perform the process lazily, i.e., by loading pieces of code or data only as they are
needed during program execution. To truly understand how lazy loading of pieces of code and data works, you’ll have to  understand more about he machinery of paging and swapping, topics we’ll cover in the future when we discuss the
virtualization of memory. For now, just remember that before running anything, the OS clearly must do some work to get
the important program bits from disk into memory.