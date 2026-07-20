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
