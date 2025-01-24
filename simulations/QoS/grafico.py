import matplotlib.pyplot as plt
from collections import Counter
import numpy as np


GBR = [1, 2, 3, 4, 65, 66, 67, 71, 72, 73, 74, 76]
NonGBR = [5, 6, 7, 8, 9, 69, 70, 79, 80]
DCGBR = [82, 83, 84, 85, 86, 87, 88, 89, 90]

# Função para contar o número de APPs alocados a cada QFI dentro de uma classe
def count_apps_for_qfi(qfi_data, qfi_list):
    count_dict = {}
    for qfi in qfi_list:
        if qfi in qfi_data:
            count_dict[qfi] = len(qfi_data[qfi]["APPs"])
        else:
            count_dict[qfi] = 0
    return count_dict

def parse_qfi_data(file_path):
    qfi_data = {}

    with open(file_path, "r") as file:
        for line in file:
            if line.startswith("INFO: QFI:"):  # Verifica se a linha começa com "INFO: QFI:"
                parts = line.strip().split(", ")  # Divide a linha por vírgulas
                qfi = int(parts[0].split("QFI: ")[1])  # Extrai o QFI
                pcp = int(parts[1].split("PCP: ")[1])  # Extrai o PCP
                apps_part = parts[2].split("APPs: ")[1].strip()  # Obtém a parte após "APPs: " e remove espaços extras
                apps = [int(app.strip()) for app in apps_part.split("|") if app.strip().isdigit()]  # Converte os aplicativos para inteiros
                qfi_data[qfi] = {"PCP": pcp, "APPs": apps}  # Salva os dados no dicionário

    return qfi_data


# Função para criar histogramas
def plot_histogram(freq, title):
    apps = list(freq.keys())
    counts = list(freq.values())
    plt.figure(figsize=(10, 6))
    plt.bar(apps, counts, color='skyblue')
    plt.xlabel('APPs')
    plt.ylabel('Frequência')
    plt.title(title)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.show()


# Exemplo de uso:
file_path = "results.txt"  # Substitua pelo caminho do seu arquivo
qfi_data = parse_qfi_data(file_path)

# Exibindo os dados para verificar
for qfi, apps in qfi_data.items():
    print(f"QFI: {qfi}, APPs: {apps}")

# Contar o número de APPs para cada QFI em cada classe
gbr_counts = count_apps_for_qfi(qfi_data, GBR)
nongbr_counts = count_apps_for_qfi(qfi_data, NonGBR)
dcgbr_counts = count_apps_for_qfi(qfi_data, DCGBR)

# Função para plotar histograma com espaçamento igual
def plot_histogram(qfi_counts, title, color,name):
    # Obter a lista de QFIs e as contagens
    qfis = list(qfi_counts.keys())
    counts = list(qfi_counts.values())

    # Criar os bins com espaçamento igual, independentemente dos valores dos QFIs
    plt.figure(figsize=(10, 6))
    plt.bar(np.arange(len(qfis)), counts, tick_label=qfis, color=color)  # Usando np.arange para espaçamento igual
    plt.xlabel("QFI", fontsize=26)
    plt.ylabel("Número de Aplicações", fontsize=24)
    plt.xticks(fontsize=24)
    plt.yticks(fontsize=26)
    plt.xticks(rotation=0)
    plt.tight_layout()
    plt.savefig(name+".pdf")
    plt.show()

# Plotando os histogramas

# Histograma para GBR
plot_histogram(gbr_counts, "Número de Aplicações por QFI (GBR)", "blue","GBR")

# Histograma para NonGBR
plot_histogram(nongbr_counts, "Número de Aplicações por QFI (NonGBR)", "green","Non-GBR")

# Histograma para DCGBR
plot_histogram(dcgbr_counts, "Número de Aplicações por QFI (DCGBR)", "red","DC-GBR")