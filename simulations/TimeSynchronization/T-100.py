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
DL = "resultsCSV/DL1-100s.csv"
UL = "resultsCSV/UL1-100s.csv"
UEUE = "resultsCSV/UEUE1-100s.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]
c = ["green", "orange", "skyblue"]

# Lista para armazenar todos os dados e estatísticas
all_data = []
stats_list = []

for idx, arq in enumerate(arqs):
    df = pd.read_csv(arq)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [abs(float(i)) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [abs(float(i)) for i in x.strip('[]').split()])
    #df['vecvalue'] = remove_outliers(df['vecvalue'])
    
    # Explodir os valores da coluna 'vecvalue' para criar uma linha por valor
    exploded_df = df.explode('vecvalue')
    exploded_df['case'] = labels[idx]  # Adicionando o label para identificar cada conjunto de dados
    exploded_df['vecvalue'] = exploded_df['vecvalue'].astype(float)  # Garantir que 'vecvalue' seja numérico

    # Cálculo das estatísticas
    stats = exploded_df['vecvalue'].describe()
    variance = exploded_df['vecvalue'].var()
    stats_list.append({
        "Case": labels[idx],
        "Mean": stats['mean'],
        "Std Dev": stats['std'],
        "Min": stats['min'],
        "Max": stats['max'],
        "Variance": variance
    })

    all_data.append(exploded_df)

# Criar DataFrame para as estatísticas
stats_df = pd.DataFrame(stats_list)

# Imprimir as estatísticas
print("\nEstatísticas Resumidas:")
print(stats_df)

# Concatenar todos os dados em um único DataFrame
combined_df = pd.concat(all_data)

# Criando o boxplot com todos os dados lado a lado
plt.figure(figsize=(12, 6))
sns.boxplot(x='case', y='vecvalue', data=combined_df, palette=c)

# Ajustes nos rótulos e títulos
plt.xlabel('Cases', fontsize=24)
plt.ylabel('Time difference (s)', fontsize=24)
plt.xticks(fontsize=18)
plt.yticks(fontsize=18)
#plt.yscale('log')  # Escala log para o eixo y

# Ajustar o layout para garantir que tudo esteja visível
plt.tight_layout()

# Salvar o gráfico
plt.savefig("TimeSync1_Comparison100.pdf")
plt.show()
