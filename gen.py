import struct

instructions = [0xD105, 0xD203, 0x0123, 0xF300, 0xE000]

with open("test.bin", "wb") as f:
    for instr in instructions:
        f.write(struct.pack(">H", instr))  # >H = big-endian unsigned short

print("wrote test.bin")