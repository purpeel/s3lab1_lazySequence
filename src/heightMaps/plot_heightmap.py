import numpy as np
import matplotlib.pyplot as plt
import sys
import os

def plot_heightmap(csv_file):
    # Load CSV data
    data = np.loadtxt(csv_file, delimiter=',')
    
    # Create figure
    plt.figure(figsize=(10, 10))
    plt.imshow(data, cmap='Blues', interpolation='nearest')
    plt.colorbar(label='Height')
    plt.title('Height Map')
    
    # Save PNG in same directory as CSV
    png_file = csv_file.replace('.csv', '.png')
    plt.savefig(png_file, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Height map visualization saved to {png_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python plot_heightmap.py <csv_file>")
        sys.exit(1)
    
    csv_file = sys.argv[1]
    if not os.path.exists(csv_file):
        print(f"Error: File {csv_file} not found")
        sys.exit(1)
    
    plot_heightmap(csv_file)