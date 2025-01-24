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

# Criar o gráfico
plt.figure(figsize=(12, 8))

# Plotar dados do arquivo UL
for _, row in df_ul.iterrows():
    plt.plot(row['vectime'], row['vecvalue'], label=f"UL - {row['module'][5:]}")
# Plotar dados do arquivo DL
for _, row in df_dl.iterrows():
    plt.plot(row['vectime'], row['vecvalue'], label=f"DL - {row['module'][5:]}")
# Plotar dados do arquivo UE-UE
for _, row in df_ueue.iterrows():
    plt.plot(row['vectime'], row['vecvalue'], label=f"UE-UE - {row['module'][5:]}")

# Adicionar rótulos e título
plt.xlabel('Simulation time (s)',fontsize=24)
plt.ylabel('Time difference to GM (µs)',fontsize=24)
#plt.yscale('log')
plt.xlim(left=0)
plt.tick_params(axis='both', which='major', labelsize=18)
# Adicionar legenda
plt.legend(fontsize=24)

# Exibir e salvar o gráfico
plt.grid(True)
plt.savefig("TimeSync1_Comparison.pdf")
plt.show()