# Description
Acest director contine diferite implementari pentru un counter paralel. Metodele abordate sunt:
 * `seriala` - foloseste un singur thread
 * `concurenta` - foloseste o variabila globala shared de catre toate thread-urile si fiecare thread face lock pe ea cand vrea sa updateze counter-ul (acest lucru afecteaza semnificativ performantele)
 * `sloppy` - algoritmul din carte (**Capitolul 29**) care foloseste un mecanism de impartire pe toate nucleele fizice ale procesorului, spawnand cate 2 thread-uri pentru fiecare nucleu (desi performanta se impunatateste tot este inceata pentru ca 2 thread-uri modifica o variabila, deci este nevoie de lock)
 * `thread local storage (tls)` - fiecare thread are un counter privat, astfel mai este nevoie decat de un lock pentru variabila globala care este updatata (pentru a avea o acuratete de 100% folosim la final o functie de flush pentru a elibera ce a mai ramas in variabila locala)
> [Note!] Varianta sloppy + tls se folosesc de acel threshold pentru a goli la un anumit timp variabilele locale in variabila globala. Aceasta strategie are cateva lucruri de spus: daca sa zicem folosim o aplicatie interactiva si vrem sa avem niste rezultate partiale la momentul in care facem o interogare, atunci aplicam aceasta metoda (nu am vrea ca la un moment in care cineva vrea sa vada valoare aproximativa a counterului aceasta sa fie 0 pentru ca inca nu am terminat de procesat), altfel, daca rezultatul care ne intereseaza este cel final, fara alte interogari pe parcurs, putem renunta la acel threshold

# How to run
```bash
# ruleaza o serie de teste in care prorgamului 
# principal i se aloca un nr de thread-uri (intre 1 si 16)
# si se salveaza resultatele in results.csv
./measure_performance
python3 plot_results.py # script generat
# se citesc datele din fisierul .csv si se fac 2 figuri
# una pentru timpul de rulare al fiecarui program si
# una pentru acuratetea rezultatelor (in mare pentru varianta sloppy)
```

# Grafice
Primul grafic arata performantele de timp pe care fiecare metoda le area, iar cel de al 2-lea arata acuratetea fiecarei metode (daca a ajuns la rezultatul asteptata).
> [NOTE!] Singura metoda care nu ruleaza pe mai multe thread-rui este cea seriala, astfel in grafice cand este crescut nr de thread-uri, pentru acea implementare nu inseamna nimic deoarece ea ruleaza pe un singur thread, axa Ox de thread-uri fiind pentru celelalte metode