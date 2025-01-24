import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import numpy as np

# Função para processar dados do arquivo
def process_file(filepath, i):
    df = pd.read_csv(filepath)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [1000000*float(i) for i in x.strip('[]').split()])

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
    
    # Interpolando y1 em x2 para alinhamento
    y1_at_x2 = interp_y1(x2)

    # Calculando a diferença entre as curvas
    diff = y1_at_x2 - y2

    return diff

# Arquivos e labels
DL = "resultsCSV/DL5-100s.csv"
UL = "resultsCSV/UL5-100s.csv"
UEUE = "resultsCSV/UEUE5-100s.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]
colors = ["green", "orange", "skyblue"]

# Lista para armazenar todos os dados
all_data = []
stats_list = []

for idx, arq in enumerate(arqs):
    diff = process_file(arq, idx)
    
    # Criando um DataFrame para os valores de diferença
    diff_df = pd.DataFrame({"vecvalue": diff})
    diff_df["case"] = labels[idx]  # Adicionando o label para identificação
    
    # Calculando estatísticas descritivas
    stats = diff_df["vecvalue"].describe()
    variance = diff_df["vecvalue"].var()
    stats_list.append({
        "Case": labels[idx],
        "Mean": stats["mean"],
        "Std Dev": stats["std"],
        "Min": stats["min"],
        "25%": stats["25%"],
        "Median (50%)": stats["50%"],
        "75%": stats["75%"],
        "Max": stats["max"],
        "Variance": variance
    })
    
    all_data.append(diff_df)

# Criar DataFrame para as estatísticas
stats_df = pd.DataFrame(stats_list)

# Imprimir as estatísticas
print("\nEstatísticas Descritivas:")
print(stats_df)

# Concatenar todos os dados em um único DataFrame
combined_df = pd.concat(all_data, ignore_index=True)

# Criando o gráfico de violino
plt.figure(figsize=(12, 6))
sns.violinplot(x='case', y='vecvalue', data=combined_df, palette=colors, cut=0)

# Ajustes nos rótulos e títulos
plt.xlabel('Cases', fontsize=24)
plt.ylabel('Time Difference (s)', fontsize=24)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)

# Ajustar o layout para garantir que tudo esteja visível
plt.tight_layout()

# Salvar o gráfico
plt.savefig("TimeSync_Differences_ViolinPlot.pdf")
plt.show()
