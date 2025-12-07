import argparse
import serial
from pathlib import Path

def check_serial_out(ser: serial.Serial):
    # Loop, printing anything received from the device.
    # Stop after we see an FPS message 5 times in a row.
    fps_count = 0
    while True:
        line = ser.readline().decode('ascii')
        if line.startswith('FPS:'):
            fps_count += 1
        else:
            fps_count = 0

        print(line, end='')

        if fps_count >= 2:
            break

def upload_file(file: Path, ser: serial.Serial):
    name = file.name.split(".")[0]
    ser.write(f'upload {name}\n{file.read_text()}'.encode('ascii'))
    ser.write(0x04) # EOT character
    check_serial_out(ser)

parser = argparse.ArgumentParser(description='Upload a pattern to Aurora.')
parser.add_argument('port', help='Serial port to use')
parser.add_argument('filename', help='Pattern file to upload. If a path to a folder, uploads all lua files in that folder (not recursively).')

args = parser.parse_args()

with serial.Serial(args.port, 115200) as ser:

    file = Path(args.filename)
    if file.is_dir():
        for child in file.iterdir():
            if child.is_file() and child.name.endswith(".lua"):
                print(f"Uploading {child}")
                upload_file(child, ser)
    else:
        upload_file(file, ser)
        ser.write(f'select {file.name.split(".")[0]}\n'.encode('ascii'))
        check_serial_out(ser)
