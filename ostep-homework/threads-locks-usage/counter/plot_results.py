import pandas as pd
import matplotlib.pyplot as plt

# 1. Citim datele din fișierul CSV
try:
    df = pd.read_csv('results.csv')
except FileNotFoundError:
    print("Eroare: Nu am putut gasi fisierul 'results.csv'. Asigura-te ca ai rulat scriptul bash inainte.")
    exit(1)

# 2. Extragem coloanele de interes
threads = df['NumThreads']
expected_val = df['Expected']

serial_time = df['SerialTime']
concurent_time = df['ConcurentTime']
sloppy_time = df['SloppyTime']

serial_val = df['SerialValue']
concurent_val = df['ConcurentValue']
sloppy_val = df['SloppyValue']

# 3. Creăm o figură cu 2 subgrafice (1 rând, 2 coloane)
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

# ==========================================
# GRAFICUL 1: TIMP (Performanță)
# ==========================================
ax1.plot(threads, serial_time, label='Serial (Fără Lock)', color='blue', linestyle='--', marker='o')
ax1.plot(threads, concurent_time, label='Concurent (1 Lock Global)', color='red', marker='x')
ax1.plot(threads, sloppy_time, label='Sloppy (Lock per CPU)', color='purple', marker='s')

ax1.set_title('Timp de Execuție: Serial vs. Concurent vs. Sloppy')
ax1.set_xlabel('Număr de Thread-uri')
ax1.set_ylabel('Timp (secunde)')
ax1.set_xticks(threads)
ax1.grid(True, linestyle=':', alpha=0.7)
ax1.legend()

# ==========================================
# GRAFICUL 2: VALOARE (Corectitudine)
# ==========================================
# Valoarea așteptată (linia groasă de referință)
ax2.plot(threads, expected_val, label='Valoare Așteptată (Exactă)', color='green', linewidth=6, alpha=0.3)

# Valorile obținute
ax2.plot(threads, serial_val, label='Valoare Serială', color='blue', linestyle='--', marker='o')
ax2.plot(threads, concurent_val, label='Valoare Concurentă', color='red', linestyle=':', marker='x')
ax2.plot(threads, sloppy_val, label='Valoare Sloppy', color='purple', linestyle='-.', marker='s')

ax2.set_title('Corectitudinea Datelor: Expected vs Actual')
ax2.set_xlabel('Număr de Thread-uri')
ax2.set_ylabel('Valoare Counter')
ax2.set_xticks(threads)
ax2.grid(True, linestyle=':', alpha=0.7)
ax2.legend()

# 4. Ajustăm aspectul general și afișăm
plt.tight_layout()
plt.show()