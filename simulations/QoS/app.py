import random
import xml.etree.ElementTree as ET

# Função para gerar valores de PER para pcp == 0
def gerar_per():
    return f"{random.choice([10**i for i in range(-8, 0)])}"

# Função para gerar valores de latência para pcp entre 1 e 4
def gerar_latency():
    return f"{random.randint(10, 1000)}ms"

# Função para gerar valores de deadline para pcp entre 5 e 7
def gerar_deadline():
    return f"{random.randint(10, 1000)}ms"

# Função para gerar valores de packetLength
def gerar_packet_length():
    return f"{random.choice([64, 128, 256, 512, 1024, 1400])}B"

# Função para gerar valores de productionInterval
def gerar_production_interval():
    return f"{random.randint(10, 1000)}ms"

# Função para gerar o app
def gerar_app():
    pcp = random.randint(0, 7)
    app = ET.Element("app")

    # Criar os parâmetros dentro do app
    params = {
        "source": f"PLC",
        "destination": f"Device",
        "packetLength": gerar_packet_length(),
        "productionInterval": gerar_production_interval(),
        "vlan": str(random.randint(1, 100)),
        "pcp": str(pcp),
        "destPort": str(random.randint(2000, 3000)),
        "displayName": f"App{random.randint(1, 10)}"
    }

    # Adiciona os parâmetros dentro do app
    for param_name, param_value in params.items():
        param = ET.SubElement(app, "param", name=param_name)
        param.text = param_value

    # Adiciona parâmetros específicos para pcp
    if pcp == 0:
        if random.random() < 0.8:  # 80% chance de ter PER
            param = ET.SubElement(app, "param", name="PER")
            param.text = gerar_per()
    elif pcp in [1, 2, 3, 4]:
        param = ET.SubElement(app, "param", name="latency")
        param.text = gerar_latency()
    else:
        param = ET.SubElement(app, "param", name="deadline")
        param.text = gerar_deadline()

    return app

# Função para adicionar indentação no XML
def indent(elem, level=0):
    i = "\n" + level * "  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        for e in elem:
            indent(e, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i

# Função principal para gerar o XML
def gerar_xml():
    config = ET.Element("config")
    cell = ET.SubElement(config, "cell")
    
    # Gerar 5 aplicações
    for _ in range(1000):  # você pode ajustar o número de aplicações conforme necessário
        cell.append(gerar_app())
    
    # Criar a árvore XML
    tree = ET.ElementTree(config)
    
    # Adicionar indentação ao XML
    indent(config)
    
    # Salvar o arquivo XML com indentação
    tree.write("config/app.xml", encoding="utf-8", xml_declaration=True)

# Executando a função
gerar_xml()
