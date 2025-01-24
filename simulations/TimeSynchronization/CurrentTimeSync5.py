import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy as np

# Função para processar dados do arquivo
def process_file(filepath, i):
    df = pd.read_csv(filepath)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [1000000 * float(i) for i in x.strip('[]').split()])

    curve1 = df.iloc[0]
    curve2 = df.iloc[1]

    x1 = np.array(curve1['vectime'])
    y1 = np.array(curve1['vecvalue'])
    x2 = np.array(curve2['vectime'])
    if i == 0:
        y2 = np.array(curve2['vecvalue'])
    else:
        y2 = -np.array(curve2['vecvalue'])

    interp_y1 = interp1d(x1, y1, kind='linear', fill_value="extrapolate")
    interp_y2 = interp1d(x2, y2, kind='linear', fill_value="extrapolate")

    # Interpolando y1 em x2 e y2 em x1 para alinhá-los
    y1_at_x2 = interp_y1(x2)
    y2_at_x1 = interp_y2(x1)

    return curve1['module'], x1, y1, curve2['module'], x2, y2, y1_at_x2, y2_at_x1

# Arquivos e labels
DL = "resultsCSV/DL5-100s.csv"
UL = "resultsCSV/UL5-100s.csv"
UEUE = "resultsCSV/UEUE5-100s.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]

# Processar cada arquivo e salvar os gráficos separadamente
for idx, arq in enumerate(arqs):
    module1, x1, y1, module2, x2, y2, y1_at_x2, y2_at_x1 = process_file(arq, idx)

    # Criar figura para cada gráfico
    plt.figure(figsize=(6, 8))
    plt.plot(x1, y1, label=f"Translator", color="black", linewidth=4)
#    plt.plot(x2, y2, label=f"{module2[5:-1]}", color="skyblue", linestyle='dotted', linewidth=4)
    plt.plot(x2, y2, label=f"TSN switch", color="skyblue", linestyle='dotted', linewidth=4)

    # Títulos e rótulos
    #plt.title(f'{labels[idx]}', fontsize=30)
    plt.title(f'', fontsize=30)
    plt.xlim(0, 100)
    plt.xlabel('Simulation time (s)', fontsize=28)
    plt.ylabel('Synchronization error (μs)', fontsize=28)
    plt.tick_params(axis='x', labelsize=22)
    plt.tick_params(axis='y', labelsize=22)

    # Configurar a grade e linha horizontal
    plt.grid(True)
    plt.axhline(y=0, color='grey', linestyle='-', linewidth=2)
    #plt.legend(fontsize=22, loc='upper left')
    plt.legend(fontsize=26)

    # Salvar gráfico individualmente
    plt.tight_layout()
    plt.savefig(f"{labels[idx]}_Sync5.pdf")
    plt.close()

print("Gráficos individuais salvos com sucesso.")
