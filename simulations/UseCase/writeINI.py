import pandas as pd
import xml.etree.ElementTree as ET
import os
from collections import defaultdict

def add_num_apps(nodes, content,source,destination):
    
    app_count = ""
    for node, count in nodes.items():
        app_count += f"*.{node}.numApps = {count}\n"
        if node in source:
            app_count += f"*.{node}.hasOutgoingStreams = true\n"
        if node in destination:
            app_count += f"*.{node}.hasIncomingStreams = true\n"
    
    return app_count + content

def format_source_content(source, app_index, app, source_count):
    """Formata as configurações para uma aplicação 'source'."""
    
    content = ""
    content += f"*.{source}.app[{app_index}].typename = \"UdpSourceApp\"\n"
    content += f"*.{source}.app[{app_index}].io.destAddress = \"{app['destination']}\"\n"
    content += f"*.{source}.app[{app_index}].io.destPort = {app['destPort']}\n"
    content += f"*.{source}.app[{app_index}].source.packetLength = {app['packetLength']}\n"

    if 'intervalDistribution' in app and pd.notna(app['intervalDistribution']):
        interval_distribution_value = app['intervalDistribution'].split('(')[1].rstrip(')')
        content += f"*.{source}.app[{app_index}].source.productionInterval = normal({app['productionInterval']}, {interval_distribution_value})\n"
    else:
        content += f"*.{source}.app[{app_index}].source.productionInterval = {app['productionInterval']}\n"

    if 'initialProductionOffset' in app and pd.notna(app['initialProductionOffset']):
        content += f"*.{source}.app[{app_index}].source.initialProductionOffset = {app['initialProductionOffset']}\n"

    return content

def format_destination_content(destination, dest_app_index, app, destination_count):
    """Formata as configurações para uma aplicação 'destination'."""
    content = ""

    content += f"*.{destination}.app[{dest_app_index}].typename = \"UdpSinkApp\"\n"
    content += f"*.{destination}.app[{dest_app_index}].io.localPort = {app['destPort']}\n"

    if 'displayName' in app and pd.notna(app['displayName']):
        content += f"*.{destination}.app[{dest_app_index}].display-name = {app['displayName']}\n"

    return content

def format_mappings(stream_identifier_mapping, stream_encoder_mapping):
    """Formata mapeamentos de streamIdentifier e streamCoder."""
    content = ""
    for source in stream_identifier_mapping:
        content += f"*.{source}.bridging.streamIdentifier.identifier.mapping = [{', '.join(stream_identifier_mapping[source])}]\n"
        content += f"*.{source}.bridging.streamCoder.encoder.mapping = [{', '.join(stream_encoder_mapping[source])}]\n"
    return content

def write_omnetpp_ini(template_path, output_path, df, source_count, destination_count):
    """Escreve o conteúdo formatado para o arquivo omnetpp.ini com base no template."""
    if os.path.exists(template_path):
        with open(template_path, 'r') as template_file:
            content = template_file.read()

        app_content = ""
        stream_identifier_mapping = defaultdict(list)
        stream_encoder_mapping = defaultdict(list)
        source_app_indices = defaultdict(int)
        destination_app_indices = defaultdict(int)
        node_app_indices = defaultdict(int)

        # Processando fontes
        for _, app in df.iterrows():
            source = app['source']
            app_index = node_app_indices[source]
            app_content += format_source_content(source, app_index, app, source_count) # ta errado

            stream_identifier_mapping[source].append(f"{{stream: \"{source}{app_index}\", packetFilter: expr(udp.destPort == {app['destPort']})}}")
            stream_encoder_mapping[source].append(f"{{stream: \"{source}{app_index}\", pcp: {app['pcp']}}}")

            node_app_indices[source] += 1
            source_app_indices[source] += 1

        # Processando destinos
        for _, app in df.iterrows():
            destination = app['destination']
            dest_app_index = node_app_indices[destination]
            app_content += format_destination_content(destination, dest_app_index, app, destination_count)

            node_app_indices[destination] += 1
            destination_app_indices[destination] += 1

        print(node_app_indices)
        app_content = add_num_apps(node_app_indices, app_content,source_app_indices,destination_app_indices)


        # Adicionando mapeamentos
        app_content += format_mappings(stream_identifier_mapping, stream_encoder_mapping)
        content = content.replace("### app.xml", app_content)

        with open(output_path, 'w') as output_file:
            output_file.write(content)
        print(f"Arquivo {output_path} criado com sucesso.")
    else:
        print(f"Arquivo de template {template_path} não encontrado.")