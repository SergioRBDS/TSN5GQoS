import pandas as pd
from scipy.interpolate import interp1d
import numpy as np

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
    df['vectime'] = df['vectime'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [float(i) for i in x.strip('[]').split()])

    curve1 = df.iloc[0]
    curve2 = df.iloc[1]

    x1 = np.array(curve1['vectime'])
    y1 = np.array(curve1['vecvalue'])
    x2 = np.array(curve2['vectime'])
    y2 = np.array(curve2['vecvalue'])

    interp_y1 = interp1d(x1, y1, kind='linear', fill_value="extrapolate")
    y1_at_x2 = interp_y1(x2)
    module = df.iloc[1]['module']

    df2 = pd.DataFrame([[module,x2,y2-y1_at_x2]],columns=["module", "vectime", "vecvalue"])

    return df2


# Arquivos e labels
DL = "resultsCSV/DL4-FULL.csv"
UL = "resultsCSV/UL4-FULL.csv"
UEUE = "resultsCSV/UEUE4-FULL.csv"

arqs = [DL, UL, UEUE]
labels = ["DL", "UL", "UEUE"]

# Lista para armazenar as diferenças
differences = []

# Processar cada arquivo e calcular a diferença
for idx, arq in enumerate(arqs):
    df = pd.read_csv(arq)
    df = df[["module", "vectime", "vecvalue"]]
    df['module'] = df['module'].apply(lambda x: x[8:-5])
    df['vectime'] = df['vectime'].apply(lambda x: [float(i) for i in x.strip('[]').split()])
    df['vecvalue'] = df['vecvalue'].apply(lambda x: [1000000 * float(i) for i in x.strip('[]').split()])

    curve1 = df.iloc[0]
    curve2 = df.iloc[1]

    x1 = np.array(curve1['vectime'])
    y1 = np.array(curve1['vecvalue'])
    x2 = np.array(curve2['vectime'])
    y2 = np.array(curve2['vecvalue'])

    interp_y1 = interp1d(x1, y1, kind='linear', fill_value="extrapolate")
    interp_y2 = interp1d(x2, y2, kind='linear', fill_value="extrapolate")
    y1_at_x2 = interp_y1(x2)
    y2_at_x1 = interp_y1(x1)
    module = df.iloc[1]['module']

    diff = []
    if idx==1:
        diff = y1-y2_at_x1
        module = df.iloc[0]['module']
    else:
        diff = y2-y1_at_x2
        module = df.iloc[1]['module']
    df2 = pd.DataFrame([[module,x2,diff]],columns=["module", "vectime", "vecvalue"])
    df2['vecvalue'] = remove_outliers(df2['vecvalue'])
    df2['vecvalue'] = df2['vecvalue'].apply(lambda x: [abs(i) for i in x])    
    exploded_df = df2.explode('vecvalue')
    exploded_df['case'] = labels[idx]  # Adicionando o label para identificar cada conjunto de dados
    exploded_df['vecvalue'] = exploded_df['vecvalue'].astype(float)  # Garantir que 'vecvalue' seja numérico

    # Cálculo das estatísticas
    stats = exploded_df['vecvalue'].describe()
    variance = exploded_df['vecvalue'].var()
    print(f"\nStatistics for {labels[idx]}:")
    print(f"  Mean: {stats['mean']:.16f}")
    print(f"  Std Dev: {stats['std']:.16f}")
    print(f"  Min: {stats['min']:.16f}")
    print(f"  Max: {stats['max']:.16f}")
    print(f"  Variance: {variance:.16f}")
    exploded_df = exploded_df.dropna()
    exploded_df = exploded_df.drop(columns=['vectime'])
    differences.append(exploded_df)

# Concatenar todos os dados em um único DataFrame
combined_df = pd.concat(differences)

