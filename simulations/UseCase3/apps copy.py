import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

# Função para carregar e processar o arquivo
def process_file(file_path, base_module):
    df = pd.read_csv(file_path)

    # Processamento dos dados
    df['module'] = df['module'].apply(lambda x: x[13:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [abs(float(i)) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])

    # Verificar se o módulo base existe no DataFrame
    base_df = df[df['module'] == base_module]
    if base_df.empty:
        print(f"Erro: módulo base '{base_module}' não encontrado.")
        return None

    # Interpolar as outras curvas para que os pontos de vectime coincidam com os da curva base
    interpolated_data = []

    for idx, row in df.iterrows():
        if row['module'] != base_module:
            # Realizar a interpolação linear
            f_interp = interp1d(row['vectime'], row['vecvalue'], kind='linear', fill_value='extrapolate')

            # Interpolando os valores para os pontos de vectime da curva base
            try:
                interpolated_values = f_interp(base_df['vectime'].iloc[0])  # interpolando para os mesmos pontos de tempo da base
                # Calcular a diferença em relação à curva base
                difference = np.array(interpolated_values) - np.array(base_df['vecvalue'].iloc[0])

                # Armazenar os dados interpolados e a diferença
                for diff in difference:
                    interpolated_data.append({
                        'module': row['module'],  # Incluindo o módulo
                        'difference': diff,
                        'file': file_path  # Associar o arquivo
                    })
            except Exception as e:
                print(f"Erro na interpolação para o módulo {row['module']}: {e}")

    # Criar um DataFrame com as diferenças
    return pd.DataFrame(interpolated_data)

# Defina o módulo base
base_module = 'SwitchDN'

# Carregar e processar os dois arquivos
file1 = "resultsCSV/RandomNoQoS.csv"
file2 = "resultsCSV/RandomQFI.csv"  # substitua pelo caminho do segundo arquivo

diff_df1 = process_file(file1, base_module)
diff_df2 = process_file(file2, base_module)

# Verificar se os dois DataFrames foram carregados corretamente
if diff_df1 is None or diff_df2 is None:
    print("Erro ao processar os arquivos.")
else:
    # Concatenar os dois DataFrames para plotar em um único boxplot
    combined_df = pd.concat([diff_df1, diff_df2]).reset_index(drop=True)

    # Plotando o boxplot com a coluna 'module' para separar as curvas
    plt.figure(figsize=(12, 6))
    sns.boxplot(x='file', y='difference', hue='module', data=combined_df)
    
    # Ajustando título e rótulos
    plt.title('Boxplot das Diferenças entre as Curvas Interpoladas por Arquivo e Módulo')
    plt.xlabel('Arquivo')
    plt.ylabel('Diferença em relação à Curva Base')
    plt.tight_layout()
    plt.show()
