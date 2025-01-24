import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# Dados da tabela
data = {
    'Experiment': [1, 1, 1, 2, 2, 2, 3, 3, 3],
    'Case': ['DL', 'UL', 'UE-UE', 'DL', 'UL', 'UE-UE', 'DL', 'UL', 'UE-UE'],
    'Std. Dev. (s)': [
        6.753065e-13, 8.292095e-13, 8.777971e-13, 
        9.203216e-13, 9.008004e-13, 9.587384e-13, 
        1.177446e-12, 1.095524e-12, 1.115267e-12
    ]
}

# Criando o DataFrame
df = pd.DataFrame(data)
print(df)
# Plotando o gráfico de barras
plt.figure(figsize=(10, 6))
sns.barplot(x='Case', y='Std. Dev. (s)', hue='Experiment', data=df)

# Ajustando o título e os rótulos
plt.title('Standard Deviation (s) by Experiment and Case', fontsize=16)
plt.xlabel('Synchronization case', fontsize=14)
plt.ylabel('Std. Dev. (s)', fontsize=14)

# Ajustando o tamanho das fontes
plt.xticks(fontsize=12)
plt.yticks(fontsize=12)

# Exibindo o gráfico
plt.tight_layout()
plt.show()
