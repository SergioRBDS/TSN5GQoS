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

# Função para processar dados do arquivo
def process_file(filepath):
    df = pd.read_csv(filepath)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    exploded_df = df.explode('vecvalue')  # Explodir os valores em linhas separadas
    #exploded_df['case'] = label  # Adicionar uma coluna para indicar o caso
    exploded_df['vecvalue'] = exploded_df['vecvalue'].astype(float)  # Garantir que os valores sejam numéricos
    
    # Remover outliers
    exploded_df = exploded_df.dropna(subset=['vecvalue'])  # Remover linhas onde os valores ficaram nulos após o filtro
    return df

# Arquivos e labels
DL = "resultsCSV/DL2-100s.csv"
UL = "resultsCSV/UL2-100s.csv"
UEUE = "resultsCSV/UEUE2-100s.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]
c = ["green", "orange", "skyblue"]

# Processar os dados para cada arquivo e adicionar um label de 'case'
all_data = []
for idx, arq in enumerate(arqs):
    df = pd.read_csv(arq)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [abs(float(i)) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = remove_outliers(df['vecvalue'])
    
    # Explodir os valores da coluna 'vecvalue' para criar uma linha por valor
    exploded_df = df.explode('vecvalue')
    exploded_df['case'] = labels[idx]  # Adicionando o label para identificar cada conjunto de dados
    exploded_df['vecvalue'] = exploded_df['vecvalue'].astype(float)  # Garantir que 'vecvalue' seja numérico

    all_data.append(exploded_df)

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
plt.gca().yaxis.get_offset_text().set_fontsize(18)
#plt.yscale('log')  # Escala log para o eixo y

# Ajustar o layout para garantir que tudo esteja visível
plt.tight_layout()

# Salvar o gráfico
plt.savefig("TimeSync2_Comparison100.pdf")
plt.show()