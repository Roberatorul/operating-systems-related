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