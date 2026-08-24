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