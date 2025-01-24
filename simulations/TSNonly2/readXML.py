import pandas as pd
import xml.etree.ElementTree as ET
import os
from collections import defaultdict

def read_xml(file_path):
    """Lê o arquivo XML e retorna o elemento raiz."""
    if os.path.exists(file_path):
        tree = ET.parse(file_path)
        root = tree.getroot()
        return root
    else:
        print(f"Arquivo {file_path} não encontrado.")
        return None

def extract_app_data(app):
    """Extrai dados de um app e retorna como um dicionário."""
    return {param.get('name'): param.text for param in app.findall('param')}


def validate_app_data(app_data, mandatory_params):
    """Verifica se o app_data possui todos os parâmetros obrigatórios preenchidos."""
    missing_or_null_params = [param for param in mandatory_params if param not in app_data or not app_data[param]]
    if missing_or_null_params:
        print(f"Erro: Aplicação com parâmetros obrigatórios ausentes ou nulos: {', '.join(missing_or_null_params)}")
        return False
    return True

def parse_apps_to_dataframe(root, mandatory_params):
    """Converte os aplicativos XML em um DataFrame, validando parâmetros obrigatórios."""
    if root.tag != 'config':
        print("O arquivo XML não contém a raiz 'config'.")
        return pd.DataFrame(), pd.DataFrame()

    apps = []
    for cell in root.findall('cell'):
        for app in cell.findall('app'):
            app_data = extract_app_data(app)
            if validate_app_data(app_data, mandatory_params):
                apps.append(app_data)
    
        df = pd.DataFrame(apps)
        source_count = df['source'].value_counts().to_dict()
        destination_count = df['destination'].value_counts().to_dict()
    print(df)
    print(df.columns)

    return df, source_count, destination_count



