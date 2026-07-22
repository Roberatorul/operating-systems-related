import pandas as pd
import matplotlib.pyplot as plt

try:
    df = pd.read_csv('results.csv')
except FileNotFoundError:
    print("Eroare: Fișierul results.csv nu a fost găsit!")
    exit(1)

thresholds = df['Threshold']
expected = df['Expected']
returned = df['Returned']

plt.figure(figsize=(10, 6))

plt.plot(thresholds, expected, label='Valoarea Așteptată (Exactă)', 
         color='green', linestyle='--', marker='o')

plt.plot(thresholds, returned, label='Valoarea Returnată (Aproximată)', 
         color='red', marker='x', markersize=8)

plt.xscale('log')
plt.title('Performanța Sloppy Counter: Acuratețe vs Threshold')
plt.xlabel('Threshold (Scară Logaritmică)')
plt.ylabel('Valoarea Counter-ului Global')
plt.grid(True, which="both", ls="--", alpha=0.5)
plt.legend()

plt.tight_layout()
plt.show()