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

# Arquivos e labels
QoS = "resultsCSV/withQoS.csv"
noQoS = "resultsCSV/withoutQoS.csv"

arqs = [QoS, noQoS]
labels = ["with QoS mapping", "without QoS mapping"]
c = ["green", "orange"]

# Lista para armazenar todos os dados e estatísticas
all_data = []
stats_list = []

# Preparando o gráfico
plt.figure(figsize=(10, 6))

# Lista para armazenar os dados para o gráfico
data_for_plot = []

# Loop para processar os arquivos
for idx, arq in enumerate(arqs):
    df = pd.read_csv(arq)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    print(df['module'])
    df['vectime'] = df['vectime'].apply(lambda x: [abs(float(i)) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    #df['vecvalue'] = remove_outliers(df['vecvalue'])

    # Calcular a média para cada módulo
    df['mean_value'] = df['vecvalue'].apply(lambda x: sum(x) / len(x))

    # Calcular média por módulo
    module_means = df.groupby('module')['mean_value'].mean().reset_index()

    # Adicionar os dados de cada arquivo
    module_means['QoS'] = labels[idx]  # Adiciona a coluna com a label do QoS
    data_for_plot.append(module_means)

# Concatenar todos os dados para plotar
final_data = pd.concat(data_for_plot)

# Plotando as barras lado a lado
sns.barplot(x='module', y='mean_value', hue='QoS', data=final_data, palette=['green', 'orange'])

# Ajustando título e rótulos
plt.title('Média dos Valores por Módulo - Comparação entre QoS e Sem QoS')
plt.xlabel('Módulo')
plt.ylabel('Valor Médio')

# Exibindo o gráfico
plt.xticks(rotation=45)
# Customizando os rótulos


plt.tight_layout()
plt.show()
