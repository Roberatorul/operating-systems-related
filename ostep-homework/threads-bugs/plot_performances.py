import csv
import matplotlib.pyplot as plt
from collections import defaultdict

# Structură pentru a stoca datele: { 'nume_program': {'threads': [], 'times': []} }
data = defaultdict(lambda: {'threads': [], 'times': []})

# 1. Citirea datelor
try:
    with open('results.csv', 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            prog = row['Program']
            threads = int(row['Threads'])
            time = float(row['Time'])
            
            data[prog]['threads'].append(threads)
            data[prog]['times'].append(time)
except FileNotFoundError:
    print("Eroare: Fisierul results.csv nu a fost gasit. Ruleaza scriptul bash intai!")
    exit(1)

# 2. Configurarea graficului
plt.figure(figsize=(10, 6))

# Trecem prin fiecare program si il adaugam pe grafic
markers = ['o', 's', '^', 'D', 'v'] # Forme diferite pentru puncte
marker_idx = 0

for prog, values in data.items():
    plt.plot(values['threads'], values['times'], 
             marker=markers[marker_idx % len(markers)], 
             linewidth=2, 
             label=prog)
    marker_idx += 1

# 3. Estetica graficului
plt.title('Performanța Metodelor de Prevenire a Deadlock-urilor', fontsize=14, fontweight='bold')
plt.xlabel('Număr de Thread-uri', fontsize=12)
plt.ylabel('Timp de Execuție (secunde) - Mai mic = Mai bine', fontsize=12)

# Setăm axa X să arate exact valorile noastre (1, 2, 4, 8)
plt.xticks([1, 2, 4, 8])

plt.grid(True, linestyle='--', alpha=0.7)
plt.legend(title='Strategie', title_fontsize='12', fontsize='10')
plt.tight_layout()

# 4. Salvarea si afisarea
plt.savefig('grafic_performante.png', dpi=300)
print("Graficul a fost salvat cu succes ca 'grafic_performante.png'.")
plt.show()
