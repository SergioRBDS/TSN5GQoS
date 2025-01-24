import random, sys, os, string, re, math, importlib
import numpy as np
import scipy.stats as st
import itertools as it
import matplotlib as mpl
import ast
from scipy.interpolate import interp1d
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import matplotlib.ticker as mticker

def remove_outliers(data):
    outliers_removed = []
    for values in data:
        q1 = pd.Series(values).quantile(0.25)
        q3 = pd.Series(values).quantile(0.75)
        iqr = q3 - q1
        lower_bound = q1 - 1.5 * iqr
        upper_bound = q3 + 1.5 * iqr
        outliers_removed.append([v for v in values if v >= lower_bound and v <= upper_bound])
    return outliers_removed

# Removendo outliers

DL = "resultsCSV/DL2-5s.csv"
UL = "resultsCSV/UL2-5s.csv"
UEUE = "resultsCSV/UEUE2-5s.csv"

# Função para processar dados do arquivo
def process_file(filepath):
    df = pd.read_csv(filepath)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [0] + [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [0] + [1000000 * abs(float(i)) for i in x.strip('[]').split()])
    # Remova outliers se necessário
    # df['vecvalue'] = remove_outliers(df['vecvalue'])
    return df

# Processar ambos os arquivos
df_dl = process_file(DL)
df_ul = process_file(UL)
df_ueue = process_file(UEUE)

# Criar o gráfico principal
fig, ax_main = plt.subplots(figsize=(12, 8))

# Plotar dados do arquivo UL
for _, row in df_ul.iterrows():
    ax_main.plot(row['vectime'], row['vecvalue'], label=f"UL - {row['module'][5:]}")
# Plotar dados do arquivo DL
for _, row in df_dl.iterrows():
    ax_main.plot(row['vectime'], row['vecvalue'], label=f"DL - {row['module'][5:]}")
# Plotar dados do arquivo UE-UE
for _, row in df_ueue.iterrows():
    ax_main.plot(row['vectime'], row['vecvalue'], label=f"UE-UE - {row['module'][5:]}")

# Configurações do gráfico principal
ax_main.set_xlabel('Simulation time (s)', fontsize=24)
ax_main.set_ylabel('Time difference to GM (µs)', fontsize=24)
ax_main.set_xlim(0,5)
ax_main.set_yscale("log")
#ax_main.yaxis.set_major_locator(mticker.MultipleLocator(2))  # Intervalos de 0.02 no eixo Y no gráfico inset
ax_main.xaxis.set_major_locator(mticker.MultipleLocator(0.5)) 
ax_main.tick_params(axis='both', which='major', labelsize=18)
ax_main.legend(fontsize=16)
ax_main.grid(True)

# Criar um gráfico menor dentro do principal (inset)
ax_inset = fig.add_axes([0.47, 0.25, 0.4, 0.4])  # [x, y, largura, altura]
for _, row in df_ul.iterrows():
    ax_inset.plot(row['vectime'], row['vecvalue'], label=f"UL")
for _, row in df_dl.iterrows():
    ax_inset.plot(row['vectime'], row['vecvalue'], label=f"DL")
for _, row in df_ueue.iterrows():
    ax_inset.plot(row['vectime'], row['vecvalue'], label=f"UE-UE")

# Configurações do gráfico inset
ax_inset.set_xlim(2,5)
ax_inset.set_ylim(-0.0000001,0.0000025)
#ax_inset.set_yscale("log")
ax_inset.yaxis.set_major_locator(mticker.MultipleLocator(0.000001))  # Intervalos de 0.02 no eixo Y no gráfico inset
ax_inset.xaxis.set_major_locator(mticker.MultipleLocator(0.5)) 
ax_inset.tick_params(axis='both', which='major', labelsize=14)
ax_inset.set_title("Zoomed from the 0.5s", fontsize=10)
ax_inset.grid(True)

# Exibir e salvar o gráfico
plt.savefig("TimeSync1.pdf")
plt.show()
