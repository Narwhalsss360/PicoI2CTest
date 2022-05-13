from machine import Pin, I2C

wire = I2C(0, scl=Pin(9), sda=Pin(8), freq=100000)

def to_raw(addr, data):
    end = bytearray(addr.to_bytes(1, 'little'))
    db = data.to_bytes(4, 'little')
    for b in db:
        end.append(b)
    return end

def fromRead(read_bytes):
    return int.from_bytes(read_bytes, 'little')