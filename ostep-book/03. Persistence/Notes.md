# Persistence

## I/O
```bash
		Intel’s Z270 Chipset

----------		       ------------- 
|   CPU  |		       |   Memory  |         
----------		       -------------
	|						 |
	|						 |
	|						 |
<-----------------------------------------> Memory Bus (proprietary)
					|
					|
					|
<-----------------------------------------> General I/O Bus (e.g. PCI)
					|			|
					|			|
					|			|
					|		------------
					|		| Graphics |
					|		------------
					|
					|
					|
<-----------------------------------------> Peripheral I/O Bus
		|		 |		  |					(e.g., SCSI, SATA, USB)
	-------	  -------	------
	|	  |   |		|   |	 |
	-------	  -------	------
```
> [NOTE!] Este nevoie de aceasta ierarhie din cauza costurilor. Cu cat este mai aproape de CPU, cu atat devine mai scump de proiectat (pentru viteze ridicate) + cu cat e mai aproape, cu atat poti conecta mai putine device-uri la acel Bus. Sistemele moderne folosesc chipset-uri specializate si conexiuni **point-to-point** pentru a spori performanta.

Procesorul se conecteaza la chip-ul proprietar (DMI - Direct Media Interface la intel), iar diferitele device-uri ce doresc sa se conecteze se conecteaza la acel chip.

```bash
-------------					-------------					-------------
|			| PCIe				|			| Memory			|			|
|			| Graphics			|			| Interconnect		|			|
| Graphics  |<---------------->	|	CPU		|<---------------->	|	Memory	|
|			|					|			|					|			|
|			|					|			|					|			|
-------------					-------------					-------------
									|
									| DMI
									|
								-------------
			  <---------------->|			|<---------------->Disk
			  <---------------->|			|<---------------->Disk
			      Network		|			|
              <---------------->| I/O Chip	|
             					|			|
								|			|
								-------------
								|		  |	
								|		  |
								|		  |
								Keyboard Mouse
```

```bash
			Structura unui dispozitiv (fictiv)
	-------------------------------------
	| Registers: Status Command Data	|
	|									| Interface
	|									|
	-------------------------------------
	| Micro-controller (CPU)			|
	| Memory (DRAM or SRAM or both)		| Internals
	| Other Hardware-specific Chips		|
	-------------------------------------
```

 * `coalescing` - mecanism in care astepti putin inainte de a face ceva (e.g. o intrerupere) pentru ca eventuale alte procese care vor sa faca o intrerupere sa se suprapuna, astfel fiind trimisa o singura intrerupere

### DMA (Direct Memory Access)
In loc ca CPU-ul sa copieze datele din memorie in chipset-ul specific care mai apoi pune date pe disc, exista un device special (DMA) caruia CPU-ul ii spune de unde sa copieze datele din memorie, cat sa copieze si carui device sa trimita datele, astfel reducand timpul petrecut de CPU facand acest lucru. DMA-ul ridica o intrerupere atunci cand a terminat de mutat datele.

## HDD Structure
 * este impartit in **block-uri de 512 bytes**
 * producatorii de HDD-uri garanteaza **atomicitatea** a 512 bytes (1 sector)
 * sistemul de operare lucreaza cu blocuri de 4KB, 8 sectoare a cate 512 bytes (8 * 512 = 4096), ceea ce inseamna ca poate aparea efectul de `Torn Write`, e.g.: daca vrem sa scriem 8 sectoare, scriem primele e.g. 3 sectoare si apare o taiere de curent, atunci restul de 5 sectoare raman nescrise (cu toate acestea datorita atomicitatii unui sector de 512 bytes, stim sigur ca primele 3 au fost scrise in totalitate, fara alteratii)
 * `procesul de seek hardware` - se refera la procesul in care bratul ajunge la track-ul corect (pe cercul cu sectoarele unde se afla informatia respectiva)

## Redundant Arrays of Inexpensive Disks (RAIDs)
Un RAID este ca un fel de pc alcatuit din mai multe hdd-uri. Beneficiile unui astfel de approach la stocarea datelor sunt:
 * performanta - folosind mai multe disk-uri in paralel creste viteza I/O-ului;
 * capacitatea crescuta de stocare
 * `reliability-ul` - impartind datele intre mai multe disk-uri (cu un mecanism de `redundancy`), RAID-urile pot tolera pierderi de disk-uri, acestea ruland in continuare ca si cum nimic nu s-ar fi intamplat
 * `transparency` - nu trebuie modificat nimic din configuratia actuala, RAID-ul este vazut doar ca o zona mare de stocare pentru user

 ### RAID Level 0: Striping
 * acest level este cel mai rapid, el neavand parte de redundancy;
 * sa zicem ca avem 4 disk-uri:
	 * putem impartii informatia pe care vrem sa o punem in block-uri (e.g. block-uri de 4KB)
	 * informatia va fi impartita astfel:
	 	```bash
		Disk 0 Disk 1 Disk 2 Disk 3
		  0		 1		2  		3
		  4		 5		6		7
		  8		 9		10		11
		````
	 * acest layout ne ofera maximul de paralelism pe care il putem obtine (pot exista si alte configuratii, insa ideea de baza ramane sa lucram cu mai multe disk-uri odata, nu doar cu unul singur)
	 * dimensiunea block-urilor conteaza:
	 	 * dimensiune mica (e.g. 4KB): pentru orice fisier mai mare de 4KB se folosesc toate disk-urile, deci viteza de punere a informatiei este sporita;
		 * dimensiune mai mare (e.g. 8KB): pentru orice fisier mai mic decat aceasta dimensiune, un singur disk se poate ocupa de respectiva cerere, restul ramanand disponibile pentru alte cereri (util in baze de date unde se fac multe cereri scurte)
		 * de obicei se folosesc block-uri de 64KB

### RAID Level 1: Mirroring
 * acest level se ocupa de copierea block-urilor, fiecare copie fiind pe un disk diferit
 * configuratia pentru 2 copii pentru un block ar arata asa:
	```bash
			Configuratie RAID-10 (or RAID 1+0, stripe of mirrors)

		Disk 0 Disk 1 Disk 2 Disk 3
		  0		 0		1  		1
		  2		 2		3		3
		  4		 4		5		5
	````
 * citirea unei informatii (e.g. block-ul 5) se poate face fie de pe Disk 2, fie de pe Disk 3, in timp ce scrierea trebuie facuta pe ambele disk-uri

### RAID Level 4: Saving Space With Parity
 * structura sa arata asa:
	```bash
		Disk 0 Disk 1 Disk 2 Disk 3 Disk 4
		  0		 1		2  		3	  P0
		  4		 5		6		7	  P1
		  8		 9		10		11	  P2
	````
 * Mentinem mereu pe fiecare rand un nr. par de biti de 1 cu ajutorul operatorului XOR:
	 * daca pe randul 1 avem (0, 0, 1, 1), atunci P0 = 0 XOR 0 XOR 0 XOR 1 XOR 1 = 0
	 * cu ajutorul acestui operator putem face si **data recovery**
	 * acest model se extinde pentru toti bitii (nu stocam cate un bit pe fiecare disk, ci block-uri de 4KB, insa operatia ramane aceeasi)

### RAID Level 5: Rotating Parity
 * incearca sa rezolve problema **small write-urilor** de la RAID 4
 * structura arata asa:
	```bash
		Disk 0 Disk 1 Disk 2 Disk 3 Disk 4
		  0		 1		2  		3	  P0
		  5		 6		7		P1	  4
		  10	11		P2		8	  9
		  15	P3		12		13	 14
		  P4	16		17		18	 19
	````
