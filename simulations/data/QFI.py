import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import copy  # Importando o módulo copy
import math
import pandas as pd  # Para criar a tabela

def read_histogram_from_xml(file_path):
    # Lê o arquivo XML
    tree = ET.parse(file_path)
    root = tree.getroot()
    
    histogram = []  # Variável para armazenar o histograma
    
    for bin_elem in root.findall('bin'):
        low = bin_elem.attrib['low']  # Obtém o valor do atributo 'low'
        count = int(bin_elem.text.strip())  # Obtém o valor numérico dentro da tag <bin>
        histogram.append({'low': low, 'count': count})  # Adiciona como um dicionário
    
    return histogram

def plot_histogram(histogram, label="Original"):
    # Plotando histograma
    delays = [float(bin_data['low'].replace('ms', '')) for bin_data in histogram]
    counts = [bin_data['count'] for bin_data in histogram]
    
    plt.plot(delays, counts, label=label)


def analyze_histogram(histogram):
    # Converter valores para float e ignorar o último bin
    valid_bins = [
        {'low': float(bin_data['low'].replace('ms', '')), 'count': bin_data['count']}
        for bin_data in histogram[:-1]  # Ignora o último bin
    ]

    # Separar bins com e sem contagem zero
    bins_with_values = [bin_data for bin_data in valid_bins if bin_data['count'] > 0]

    # Verificar se há dados para análise
    if not bins_with_values:
        return "Nenhum dado válido para análise."

    # Métricas de análise
    min_delay = min(bin_data['low'] for bin_data in bins_with_values)
    max_delay = max(bin_data['low'] for bin_data in bins_with_values)
    total_values = sum(bin_data['count'] for bin_data in bins_with_values)
    
    # Atraso médio (média ponderada)
    weighted_sum = sum(bin_data['low'] * bin_data['count'] for bin_data in bins_with_values)
    mean_delay = weighted_sum / total_values if total_values > 0 else 0

    # Moda (maior contagem)
    mode_bin = max(bins_with_values, key=lambda bin_data: bin_data['count'], default=None)

    # Mediana
    cumulative_count = 0
    median = None
    for bin_data in bins_with_values:
        cumulative_count += bin_data['count']
        if cumulative_count >= total_values / 2:
            median = bin_data['low']
            break

    # Desvio padrão e variância
    squared_diff_sum = sum((bin_data['low'] - mean_delay) ** 2 * bin_data['count'] for bin_data in bins_with_values)
    variance = squared_diff_sum / total_values if total_values > 0 else 0
    std_dev = math.sqrt(variance)  # Desvio padrão

    # Coeficiente de variação
    cv = std_dev / mean_delay if mean_delay != 0 else float('inf')

    # Retorno da análise em formato de dicionário
    analysis = {
        'min_delay': min_delay,
        'max_delay': max_delay,
        'mean_delay': mean_delay,
        'total_values': total_values,
        'mode_delay': mode_bin['low'] if mode_bin else None,
        'mode_count': mode_bin['count'] if mode_bin else None,
        'median': median,
        'variance': variance,
        'std_dev': std_dev,
        'cv': cv
    }

    return analysis




def redistribute_bins(histogram, pdb):
    # Filtrando os bins que ultrapassam o PDB
    valid_bins = [bin_data for bin_data in histogram if float(bin_data['low'].replace('ms', '')) <= pdb and bin_data['low'] != '-inf ms']
    removed_bins = [bin_data for bin_data in histogram if float(bin_data['low'].replace('ms', '')) > pdb]
    
    # Contagem total dos bins removidos
    total_removed_count = sum(bin_data['count'] for bin_data in removed_bins)
    
    # Ordenando os bins válidos em ordem crescente de delay (do maior para o menor)
    valid_bins.sort(key=lambda x: float(x['low'].replace('ms', '')), reverse=True)
    
    # Redistribuindo as contagens removidas para os bins válidos
    while total_removed_count > 0:
        for bin_data in valid_bins:
            # Atribui um número inteiro de contagens ao bin
            bin_data['count'] += 1
            total_removed_count -= 1
            if total_removed_count <= 0:
                break
    
    # Ordena os bins válidos novamente por delay em ordem crescente
    valid_bins.sort(key=lambda x: float(x['low'].replace('ms', '')))
    
    # Retorna o histograma com o bin '-inf' não modificado e as alterações feitas nos outros bins
    # Inclui o bin '-inf' de volta ao histograma
    final_histogram = [bin_data for bin_data in histogram if bin_data['low'] == '-inf ms'] + valid_bins
    return final_histogram


def redistribute_bins2(histogram, pdb):
    # Filtrando os bins que ultrapassam o PDB
    valid_bins = [bin_data for bin_data in histogram if float(bin_data['low'].replace('ms', '')) <= pdb and bin_data['low'] != '-inf ms']
    removed_bins = [bin_data for bin_data in histogram if float(bin_data['low'].replace('ms', '')) > pdb]
    
    # Contagem total dos bins removidos
    total_removed_count = sum(bin_data['count'] for bin_data in removed_bins)
    
    # Se não há contagens removidas, não há nada para redistribuir
    if total_removed_count == 0:
        return histogram

    # Calcular o total de contagens dos bins válidos
    total_valid_count = sum(bin_data['count'] for bin_data in valid_bins)

    # Redistribuindo as contagens removidas para os bins válidos de forma proporcional
    for bin_data in valid_bins:
        # Calcular a proporção de contagens para este bin
        proportion = (bin_data['count']+1) / total_valid_count if total_valid_count > 0 else 0
        
        # Redistribuir a contagem proporcional
        bin_data['count'] += int(proportion * total_removed_count)
    
    # Ordena os bins válidos novamente por delay em ordem crescente
    valid_bins.sort(key=lambda x: float(x['low'].replace('ms', '')))
    
    # Retorna o histograma com o bin '-inf' não modificado e as alterações feitas nos outros bins
    final_histogram = [bin_data for bin_data in histogram if bin_data['low'] == '-inf ms'] + valid_bins
    final_histogram.append({'low': f'{pdb:.2f}ms', 'count': 0})
    return final_histogram

def print_analysis_table(analysis_results):
    # Criando um DataFrame do Pandas para organizar os dados como uma tabela
    df = pd.DataFrame(analysis_results)

    # Exibindo a tabela de forma organizada
    print(df.to_string(index=True))

# Caminho do arquivo XML
arq = ["s1-UL","s3-UL","s4-UL","s6-UL","s7-UL","s8-UL","s9-UL","s9-UL","s20-UL","s22-UL","s24-UL","s26-UL"]
all_analysis = []  # Lista para armazenar as análises

for a in arq:
    input_xml = '../deterministic6g_data/PD-Wireless-5G-1/'+a+'_wall.xml'
    pdb = 5  # PDB limite de 4ms

    # Lendo o histograma
    histogram = read_histogram_from_xml(input_xml)

    # Criando uma cópia do histograma para análise do original
    original_histogram = copy.deepcopy(histogram)

    # Analisando o histograma original
    analysis = analyze_histogram(original_histogram)
    print(f"Análise do histograma para {a}:")
    
    # Adicionando o nome do arquivo como parte da análise
    analysis['file'] = a
    
    # Adicionando os resultados de cada análise à lista
    all_analysis.append(analysis)

# Convertendo a lista de resultados em um DataFrame e exibindo como tabela
analysis_df = pd.DataFrame(all_analysis)

# Exibindo os resultados da análise de forma tabular
print_analysis_table(analysis_df)

# Ajustando o histograma conforme o PDB
#adjusted_histogram = redistribute_bins2(copy.deepcopy(original_histogram), pdb)

# Analisando o histograma ajustado
#analysis = analyze_histogram(adjusted_histogram)
#print("Resultados da análise do histograma ajustado:")
#for key, value in analysis.items():
#    print(f"{key}: {value}")


#print("Histograma armazenado na variável:")
#for bin_data in adjusted_histogram:
#    print(bin_data)

# Plotando o histograma original e ajustado
#plt.figure(figsize=(10, 6))
#plot_histogram(original_histogram, label="Histograma Original")
#plot_histogram(adjusted_histogram, label="Histograma Ajustado (PDB)")
#plt.xlabel('Delay (ms)')
#plt.ylabel('Contagem')
#plt.title('Comparação entre Histograma Original e Ajustado (PDB)')
#plt.legend()
#plt.grid(True)
#plt.show()
