import matplotlib.pyplot as plt
import re
import sys

# Dictionar pentru a stoca rezultatele mapate: numar_trials -> {numar_pagini: timp}
data = {}

try:
    with open('output.txt', 'r') as file:
        current_pages = None
        current_trials = None

        for line in file:
            # Cautam linia generata de scriptul Bash
            match_params = re.search(r'Running with (\d+) pages and (\d+) trials', line)
            if match_params:
                current_pages = int(match_params.group(1))
                current_trials = int(match_params.group(2))
                continue
            
            # Cautam linia generata de programul C (actualizata pentru engleza)
            match_time = re.search(r'Average time per access: ([\d\.]+) ns', line)
            if match_time and current_pages is not None and current_trials is not None:
                time_ns = float(match_time.group(1))
                
                if current_trials not in data:
                    data[current_trials] = {}
                data[current_trials][current_pages] = time_ns

except FileNotFoundError:
    print("Eroare: Fisierul 'output.txt' nu a fost gasit.")
    sys.exit(1)

if not data:
    print("Eroare: Nu am gasit date valide in output.txt. Verifica formatul textului!")
    sys.exit(1)

# Alegem cel mai mare numar de trials pentru grafic, 
# pentru ca acelea sunt datele cu cea mai mare acuratete
max_trials = max(data.keys())

# Extragem axele X si Y si le sortam crescator dupa numarul de pagini
pages = sorted(data[max_trials].keys())
times = [data[max_trials][p] for p in pages]

# --- Generarea Graficului ---
plt.figure(figsize=(10, 6))

# Desenam linia cu puncte
plt.plot(pages, times, marker='o', linestyle='-', color='#e74c3c', linewidth=2)

# Setarile axelor (Axa X logaritmica baza 2, conform cerintei din carte)
plt.xscale('log', base=2)
plt.xlabel('Numărul de Pagini (Scară Logaritmică)', fontsize=12)
plt.ylabel('Timp mediu per accesare (ns)', fontsize=12)
plt.title(f'Performanța TLB (Costul accesării memoriei la {max_trials} trials)', fontsize=14, fontweight='bold')

# Grila de fundal pentru a citi usor valorile
plt.grid(True, which="both", linestyle="--", alpha=0.6)

# Salvam imaginea si o afisam pe ecran
plt.savefig('tlb_graph.png', dpi=300, bbox_inches='tight')
print("✅ Graficul a fost generat si salvat ca 'tlb_graph.png'")

plt.show()