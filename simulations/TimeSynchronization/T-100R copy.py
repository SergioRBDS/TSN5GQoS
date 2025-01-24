import pandas as pd
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy as np

# Função para processar dados do arquivo
def process_file(filepath):
    df = pd.read_csv(filepath)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])

    curve1 = df.iloc[0]
    curve2 = df.iloc[1]

    x1 = np.array(curve1['vectime'])
    y1 = np.array(curve1['vecvalue'])
    x2 = np.array(curve2['vectime'])
    y2 = np.array(curve2['vecvalue'])

    interp_y1 = interp1d(x1, y1, kind='linear', fill_value="extrapolate")
    interp_y2 = interp1d(x2, y2, kind='linear', fill_value="extrapolate")
    
    # Interpolando y1 em x2 e y2 em x1 para alinhá-los
    y1_at_x2 = interp_y1(x2)
    y2_at_x1 = interp_y2(x1)

    return curve1['module'], x1, y1, curve2['module'], x2, y2, y1_at_x2, y2_at_x1

# Arquivos e labels
DL = "resultsCSV/DL4-FULL.csv"
UL = "resultsCSV/UL4-FULL.csv"
UEUE = "resultsCSV/UEUE4-FULL.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]
colors = ["green", "orange", "skyblue"]

# Criando os subgráficos verticais
fig, axes = plt.subplots(len(arqs), 1, figsize=(6, 15), sharey=False)

# Processar cada arquivo e plotar as duas curvas
for idx, arq in enumerate(arqs):
    module1, x1, y1, module2, x2, y2, y1_at_x2, y2_at_x1 = process_file(arq)
    
    # Plotando as duas curvas no subgráfico, usando o nome do módulo para os rótulos
    if idx != 1:        
        axes[idx].plot(x1, y1, label=f"GM clock", color="black", linewidth=2)
        axes[idx].plot(x2, y2, label=f"{module1[5:]}", color="skyblue", linestyle='dotted', linewidth=2)
    else:
        axes[idx].plot(x2, y2, label=f"GM clock", color="black", linewidth=2)
        axes[idx].plot(x1, y1, label=f"{module1[5:]}", color="skyblue", linestyle='dotted', linewidth=2)
    
    # Títulos e rótulos
    axes[idx].set_title(f'', fontsize=14)
    axes[idx].set_xlim(0, 100)
    if idx ==2:
        axes[idx].set_xlabel('Time', fontsize=12)
    else:
        axes[idx].set_xlabel('', fontsize=12)
        axes[idx].set_xticklabels([])
    axes[idx].set_ylabel(f'{labels[idx]} Time difference (s)', fontsize=12)
    
    # Configurar a grade e o espaçamento do eixo x
    axes[idx].grid(True)  # Ativa a grade
    axes[idx].axhline(y=0, color='black', linestyle='-', linewidth=2)  # Linha horizontal em y=0
    axes[idx].legend()

# Ajustando o layout
plt.tight_layout()

# Salvando e mostrando o gráfico
plt.savefig("SyncRandomDrift.pdf")
plt.show()
