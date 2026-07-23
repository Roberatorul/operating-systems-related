import pandas as pd
import matplotlib.pyplot as plt

# 1. Citim datele din fișierul CSV
try:
    df = pd.read_csv('results.csv')
except FileNotFoundError:
    print("Eroare: Nu am putut gasi fisierul 'results.csv'. Asigura-te ca ai rulat scriptul bash inainte.")
    exit(1)

# 2. Extragem coloanele
threads = df['NumThreads']
expected_val = df['Expected']

# Timpii
serial_time = df['SerialTime']
concurent_time = df['ConcurentTime']
sloppy_time = df['SloppyTime']
tls_time = df['TLSTime']

# Valorile finale (pentru corectitudine)
serial_val = df['SerialValue']
concurent_val = df['ConcurentValue']
sloppy_val = df['SloppyValue']
tls_val = df['TLSValue']

# 3. Creăm figurile
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 7))

# ==========================================
# GRAFICUL 1: TIMP (Performanță)
# ==========================================
ax1.plot(threads, serial_time, label='Serial (Fără Lock)', color='blue', linestyle='--', marker='o')
ax1.plot(threads, concurent_time, label='Concurent (1 Lock Global)', color='red', marker='x')
ax1.plot(threads, sloppy_time, label='Sloppy (Lock per CPU)', color='purple', marker='s')
ax1.plot(threads, tls_time, label='TLS (Privat/Fără Lock)', color='darkorange', linewidth=2, marker='*')

ax1.set_title('Timp de Execuție: Evoluția Optimizărilor')
ax1.set_xlabel('Număr de Thread-uri')
ax1.set_ylabel('Timp (secunde)')
ax1.set_xticks(threads)
ax1.grid(True, linestyle=':', alpha=0.7)
ax1.legend()

# ==========================================
# GRAFICUL 2: VALOARE (Corectitudine)
# ==========================================
ax2.plot(threads, expected_val, label='Valoare Așteptată', color='green', linewidth=6, alpha=0.3)

ax2.plot(threads, serial_val, label='Valoare Serială', color='blue', linestyle='--', marker='o')
ax2.plot(threads, concurent_val, label='Valoare Concurentă', color='red', linestyle=':', marker='x')
ax2.plot(threads, sloppy_val, label='Valoare Sloppy', color='purple', linestyle='-.', marker='s')
ax2.plot(threads, tls_val, label='Valoare TLS (cu Flush)', color='darkorange', linestyle=':', marker='*')

ax2.set_title('Corectitudinea Datelor: Expected vs Actual')
ax2.set_xlabel('Număr de Thread-uri')
ax2.set_ylabel('Valoare Counter')
ax2.set_xticks(threads)
ax2.grid(True, linestyle=':', alpha=0.7)
ax2.legend()

# 4. Afișăm
plt.tight_layout()
plt.show()