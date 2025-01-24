import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Função para processar os dados do arquivo CSV
def process_file(filepath):
    df = pd.read_csv(filepath)
    df = df[["module", "binedges", "binvalues"]]
    df['module'] = df['module'].apply(lambda x: x[13:-5])  # Ajustar o nome do módulo
    df['binedges'] = df['binedges'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['binvalues'] = df['binvalues'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    return df

# Função para combinar e plotar histogramas
def plot_combined_histogram(df, output_file):
    # Identificar os limites globais para os bins
    global_min = min(edge for binedges in df['binedges'] for edge in binedges)
    global_max = max(edge for binedges in df['binedges'] for edge in binedges)

    # Definir novos binedges padronizados
    num_bins = 100  # Número total de bins padronizados
    standardized_binedges = np.linspace(global_min, global_max, num_bins + 1)
    combined_binvalues = np.zeros(num_bins)

    # Recalcular valores de bins para todos os histogramas
    for _, row in df.iterrows():
        binedges = row['binedges']
        binvalues = row['binvalues']
        bin_centers = (np.array(binedges[:-1]) + np.array(binedges[1:])) / 2

        # Usar interpolação para mapear valores para os bins padronizados
        standardized_binvalues, _ = np.histogram(bin_centers, bins=standardized_binedges, weights=binvalues)
        combined_binvalues += standardized_binvalues

    # Calcular a largura dos bins padronizados
    widths = standardized_binedges[1:] - standardized_binedges[:-1]

    # Plotar o histograma combinado
    plt.figure(figsize=(10, 6))
    plt.bar(standardized_binedges[:-1], combined_binvalues, width=widths, align='edge', color='red', alpha=0.7, label='With mapping (combined)')
    plt.xlabel('Latency (ms)')
    plt.ylabel('Frequency')
    plt.title('Combined Histogram (With Mapping)')
    plt.legend()
    plt.grid(True)
    
    # Salvar e exibir o gráfico
    plt.savefig(output_file)
    plt.show()


# Arquivo CSV de entrada
filename2 = 'resultsCSV/HlatencyQFI.csv'  # Substitua pelo caminho do seu arquivo

# Processar os dados do arquivo
df2 = process_file(filename2)

# Gerar e salvar o gráfico combinado
plot_combined_histogram(df2, "Combined-Latency-With-Mapping.pdf")

