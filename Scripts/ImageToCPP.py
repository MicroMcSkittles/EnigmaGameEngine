# This script reads in a list of image files and stores the data to a cpp array
import sys
from pathlib import Path

def processImage(filename, output):
    image_name = Path(filename).stem
    print(f"uint8_t {image_name}[] = ", end="", file=output)
    print("{", file=output)

    image = open(filename, "rb")

    data = ""
    bytes = image.read()
    for byte in bytes:
        byte_hex = hex(byte)
        data += f"{byte_hex},"
    data = data[:-1]
    print(f"\t{data}", file=output)

    print("};\n", file=output)

    image.close()
    return

file_count = len(sys.argv) - 1

if file_count == 1:
    print("Invalid arguments. Expected format:\nImageToCPP.py output image_files...")
    sys.exit()

output_directory = sys.argv[1]
output = open(output_directory, "w")
print("#include <stdint.h>\n", file=output)

image_files = sys.argv[2:]
for filename in image_files:
    processImage(filename, output)

output.close()