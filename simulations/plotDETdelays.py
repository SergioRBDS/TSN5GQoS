import xml.etree.ElementTree as ET
import math
import matplotlib.pyplot as plt

def parse_histogram(xml_file):
    # Parse the XML file
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    # Extract bin data
    bins = []
    counts = []
    
    for bin_elem in root.findall('bin'):
        low_value = bin_elem.get('low')
        count = int(bin_elem.text)
        
        # Try to convert the low_value to a float (handle non-integer and special cases)
        try:
            low_value_ms = float(low_value[:-2])  # Remove 'ms' and convert to float
        except ValueError:
            print(f"Warning: Invalid value found for delay: {low_value}. Skipping this bin.")
            continue  # Skip this bin if the conversion fails
        
        # Skip 'inf' or 'NaN' values
        if math.isinf(low_value_ms) or math.isnan(low_value_ms):
            print(f"Warning: Invalid delay value ('{low_value_ms}') found. Skipping this bin.")
            continue
        
        bins.append(low_value_ms)
        counts.append(count)
    
    return bins, counts


def plot_and_save(bins, counts, filename, title, color):
    # Create a line plot
    plt.figure(figsize=(6, 4))
    plt.plot(bins, counts, color=color, linestyle='-', label=title)  # No markers
    plt.title(title)
    plt.xlabel('Delay (ms)')
    plt.ylabel('Count')
    plt.grid(True)
    
    # Save the plot as a PDF
    plt.savefig(filename, format='pdf', dpi=300)
    print(f"Plot saved as {filename}")
    plt.close()  # Close the figure to avoid overlap


def main():
    # Paths to the XML files
    xml_file1 = '../../deterministic6g_data/PD-Wireless-5G-1/s1-UL_wall.xml'  # Change this to your file path
    xml_file2 = '../../deterministic6g_data/PD-Wireless-5G-1/s10-DL_wall.xml'  # Change this to your file path
    xml_file3 = '../../deterministic6g_data/PD-Wireless-5G-1/testeD2D.xml'  # Change this to your file path
    
    # Parse and extract the histogram data
    bins1, counts1 = parse_histogram(xml_file1)
    bins2, counts2 = parse_histogram(xml_file2)
    bins3, counts3 = parse_histogram(xml_file3)
    
    # Generate and save separate plots for each file
    plot_and_save(bins1, counts1, 'file1_plot.pdf', 'File 1: Delay vs Count', 'blue')
    plot_and_save(bins2, counts2, 'file2_plot.pdf', 'File 2: Delay vs Count', 'red')
    plot_and_save(bins3, counts3, 'file3_plot.pdf', 'File 3: Delay vs Count', 'green')

if __name__ == '__main__':
    main()
