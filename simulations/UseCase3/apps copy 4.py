import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

def read_histogram_file(filename):
    histograms = []
    with open(filename, 'r') as file:
        current_histogram = {'binedges': [], 'binvalues': []}
        for line in file:
            line = line.strip()
            if line.startswith('binedges='):
                current_histogram['binedges'] = list(map(float, line.split('=')[1].strip('[]').split(',')))
            elif line.startswith('binvalues='):
                current_histogram['binvalues'] = list(map(float, line.split('=')[1].strip('[]').split(',')))
            elif not line:  # Linha vazia significa separação de histogramas
                if current_histogram['binedges'] and current_histogram['binvalues']:
                    histograms.append(current_histogram)
                    current_histogram = {'binedges': [], 'binvalues': []}
        # Adicionar o último histograma, caso não tenha linha vazia no final
        if current_histogram['binedges'] and current_histogram['binvalues']:
            histograms.append(current_histogram)
    return histograms

def process_histogram(binedges, binvalues):
    # Calcular os valores médios dos bins
    bin_centers = (np.array(binedges[:-1]) + np.array(binedges[1:])) / 2
    return bin_centers, binvalues

def plot_histograms(histograms):
    plt.figure(figsize=(10, 6))
    for i, histogram in enumerate(histograms):
        binedges = histogram['binedges']
        binvalues = histogram['binvalues']
        bin_centers, values = process_histogram(binedges, binvalues)
        plt.plot(bin_centers, values, label=f'Histograma {i + 1}')
    plt.xlabel('Valores')
    plt.ylabel('Frequência')
    plt.title('Histogramas Processados')
    plt.legend()
    plt.grid(True)
    plt.show()


# Função para processar dados do arquivo
def process_file(filepath):
    df = pd.read_csv(filepath)
    print(df.columns)
    print(df[["module","mean","stddev","min","max"]])
    df = df[["module", "binedges", "binvalues"]]
    df['module'] = df['module'].apply(lambda x: x[13:-5])
    df['binedges'] = df['binedges'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['binvalues'] = df['binvalues'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    
    return df


# Função para calcular o valor médio de cada bin e multiplicar pelo valor de binvalue
def calculate_latencies(df):
    latencies = []
    for _, row in df.iterrows():
        binedges = row['binedges']
        binvalues = row['binvalues']
        
        # Calcular o valor médio entre os binedges de cada bin
        bin_mids = [(binedges[i] + binedges[i + 1]) / 2 for i in range(len(binedges) - 1)]
        
        # Multiplicar o valor médio do bin pelo binvalue
        latencies.extend([bin_mids[i] * binvalues[i] for i in range(len(binvalues))])
        
    return latencies

filename1 = 'resultsCSV/Hlatency0.csv'  # Substitua pelo caminho do seu arquivo
filename2 = 'resultsCSV/HlatencyQFI.csv'  # Substitua pelo caminho do seu arquivo


df1 = process_file(filename1)
df2 = process_file(filename2)

# Definir o número de histogramas a serem plotados
num_histograms = 8  # Total de histogramas
nrows, ncols = 4, 2  # 9 subgráficos (3x3), já que temos 8 histogramas (vamos usar o último como vazio ou para outro propósito)

# Criar a figura e os subgráficos
fig, axes = plt.subplots(nrows, ncols, figsize=(8, 16))

# Achatar o array de eixos para facilitar o acesso
axes = axes.flatten()
names = ["Camera","ConveyorS1","ConveyorS2","ConveyorS3","RobotS1","RobotS2","SensorS1","SensorS3"]
# Plotar histogramas lado a lado para cada par correspondente
for i in range(num_histograms):  # Para cada histograma
    # Obter dados para o histograma i de ambos os arquivos
    name = df1.iloc[i]['module']
    binedges1 = df1.iloc[i]['binedges']
    binvalues1 = df1.iloc[i]['binvalues']
    binedges2 = df2.iloc[i]['binedges']
    binvalues2 = df2.iloc[i]['binvalues']
    
    # A largura das barras é a diferença entre as bordas consecutivas
    widths1 = [binedges1[i+1] - binedges1[i] for i in range(len(binedges1) - 1)]
    widths2 = [binedges2[i+1] - binedges2[i] for i in range(len(binedges2) - 1)]
    
    # Plotar ambos os histogramas sobrepostos com transparência para comparação
    axes[i].bar(binedges1[:-1], binvalues1, width=widths1, align='edge', alpha=0.5, label=f'Without mapping - {names[i]}', color='blue')
    axes[i].bar(binedges2[:-1], binvalues2, width=widths2, align='edge', alpha=0.5, label=f'With mapping - {names[i]}', color='red')

    # Configurações para cada gráfico
    axes[i].set_title("")
    axes[i].set_xlabel('')
    axes[i].set_ylabel('')
    axes[i].legend(loc='upper right',fontsize=12)
    axes[i].grid(True)

# Remover o último subgráfico (9º gráfico), já que temos 8 histogramas

# Ajustar o layout para que os gráficos fiquem organizados
plt.tight_layout()
plt.savefig("Latency-Uplink.pdf")
# Exibir o gráfico
plt.show()