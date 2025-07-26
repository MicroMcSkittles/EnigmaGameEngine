# this script converts text files to strings for cpp
import sys
from pathlib import Path

def processTextFile(filename, output):
    text_file_name = Path(filename).stem
    text_file_name += (Path(filename).suffix)[1:]
    print(f"const char* {text_file_name} = \"", end="", file=output)

    file = open(filename, "r")

    data = ""
    text = file.read()
    for char in text:
        if char == "\n":
            data += r"\n"  
        else:
            data += f"{char}"
    print(f"{data}", end="", file=output)

    print("\";\n", file=output)

    file.close()
    return

file_count = len(sys.argv) - 1

if file_count == 1:
    print("Invalid arguments. Expected format:\TXTtoCPPstr.py output txt_files...")
    sys.exit()

output_directory = sys.argv[1]
output = open(output_directory, "w")

image_files = sys.argv[2:]
for filename in image_files:
    processTextFile(filename, output)

output.close()