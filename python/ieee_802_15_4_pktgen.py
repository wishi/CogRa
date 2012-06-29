# from cogra import crc16
from crc16 import CRC16

def to_binary(str):
  
    # result
    result = []
  
    # preamble
    pre = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
  
    # sfd  
    sfd = [1, 0, 1, 0, 0, 1, 1, 1]
    # size
    size = []
    
    # header
    header = []
    
    # payload
    payload = []
    
    # size: per 1 char in string 1 byte
    frame_len = bin(len(str)+6)[2:]
    
    # add every bit from the to-binary conversion
    # result to the size array
    for i in frame_len:
        size.insert(0, int(i))
    
    
    # fill the size-array to match 8 bit
    while len(size)<=8:
        size.insert(0, int(0))
 
    # fill a byte for every char            
    for char in str:
        binstring = bin(ord(char))[2:]
        for i in binstring:
            payload.append(int(i))
            
        # fill the char to be 8 bit in binary
        while len(payload)<=8:
            payload.insert(0, int(0))
        
    crc = CRC16()
    crc.update(str)
    
    
    # add sfd at the beginning
    result = pre + sfd + size + payload 

    return result


    
    
