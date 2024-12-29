import socket
import time
import random
import struct

def generate_random_position():
    return random.uniform(-100, 100), random.uniform(-100, 100), random.uniform(-100, 100)

def generate_random_data():
    integer = random.randint(0, 100)
    floating = random.uniform(-100, 100)
    string = "Hello, Unreal Engine! this is a test"
    boolean = random.choice([True, False])
    byte_array = b'\x01\x02\x03\x04'
    return integer, floating, string, boolean, byte_array

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('127.0.0.1', 5555)

while True:
    integer, floating, string, boolean, byte_array = generate_random_data()
    
    # 编码字符串
    string_encoded = string.encode('utf-8')
    string_length = len(string_encoded)
    
    # 打包数据，确保格式与 Unreal Engine 中的解析相匹配
    packed_data = struct.pack('!ifI{}s?4s'.format(string_length), 
                               integer, 
                               floating, 
                               string_length, 
                               string_encoded, 
                               boolean, 
                               byte_array)
    
    # 发送数据到 Unreal Engine
    sock.sendto(packed_data, server_address)
    print(f"Sent data: {integer}, {floating}, {string}, {boolean}, {byte_array}")
    time.sleep(5)
