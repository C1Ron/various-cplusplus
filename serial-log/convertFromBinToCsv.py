import struct
import sys
import csv

def convert_binary_to_csv(input_file, output_file):
    with open(input_file, 'rb') as bin_file, open(output_file, 'w', newline='') as csv_file:
        csv_writer = csv.writer(csv_file)
        
        # Read and write the header
        header = bin_file.readline().decode('ascii').strip()
        csv_writer.writerow(header.split(','))

        # Count the number of columns (subtract 1 for the timestamp)
        num_columns = len(header.split(',')) - 1

        # Read and convert log entries
        while True:
            timestamp_bytes = bin_file.read(8)
            if not timestamp_bytes:
                break  # End of file
            
            timestamp = struct.unpack('<Q', timestamp_bytes)[0]
            
            row = [timestamp]
            for _ in range(num_columns):
                value_bytes = bin_file.read(4)
                if not value_bytes:
                    print("Unexpected end of file")
                    return
                value = struct.unpack('<i', value_bytes)[0]
                row.append(value)
            
            csv_writer.writerow(row)

    print(f"Conversion complete. CSV file saved as {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python binary_to_csv.py <input_binary_file> <output_csv_file>")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    convert_binary_to_csv(input_file, output_file)