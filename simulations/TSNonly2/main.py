import pandas as pd
import xml.etree.ElementTree as ET
import os
from collections import defaultdict
from readXML import *
from writeINI import *




# Caminhos para os arquivos
xml_file_path = os.path.join('config', 'app.xml')
template_file_path = 'omnetppTemplate.ini'
output_file_path = 'omnetpp.ini'

# Executar o script
mandatory_params = ['source', 'destination', 'packetLength', 'productionInterval', 'pcp', 'destPort']
xml_root = read_xml(xml_file_path)
if xml_root is not None:
    df, source_count, destination_count = parse_apps_to_dataframe(xml_root, mandatory_params)
    if not df.empty:
        write_omnetpp_ini(template_file_path, output_file_path, df, source_count, destination_count)
