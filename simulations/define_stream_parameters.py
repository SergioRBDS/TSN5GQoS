import xml.etree.ElementTree as ET

def load_xml(file_path):
    """Carrega o arquivo XML e retorna a raiz."""
    tree = ET.parse(file_path)
    return tree.getroot()

def extract_traffic_types(element):
    types = []
    for type in element.findall('TrafficType'):
        traffic_data = {attr: type.get(attr, 'N/A') for attr in type.keys()}
        types.append(traffic_data)
    return types
# Função para extrair informações dos TrafficType

def process_flows(file_path,types):
    root = load_xml(file_path)
        
    flows = []
    for type in types:
        flows.append(extract_traffic_types(root.find(type)))
    return flows
