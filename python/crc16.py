#!/usr/bin/env python

"""
 Translation from a C code posted to a forum on the Internet.

 @translator Thomas Schmid
"""

from array import array


def reflect(crc, bitnum):
     # reflects the lower 'bitnum' bits of 'crc'
     j = 1
     crcout = 0
     
     for b in range(bitnum):
          i = 1 << (bitnum - 1 - b)
          if crc & i:
               crcout |= j
          j <<= 1
     return crcout


def crcbitbybit(p):
     # bit by bit algorithm with augmented zero bytes.
     crc = 0
     
     for i in range(len(p)):
          c = p[i]
          c = reflect(ord(c), 8)
          j = 0x80
          for b in range(16):
               bit = crc & 0x8000
               crc <<= 1
               crc &= 0xFFFF
               if c & j:
                    crc |= 1
               if bit:
                    crc ^= 0x1021
               j >>= 1
               if j == 0:
                    break
            
     for i in range(16):
          bit = crc & 0x8000
          crc <<= 1
          if bit:
               crc ^= 0x1021

     crc = reflect(crc, 16)
     return crc


class CRC16(object):
     """ Class interface, like the Python library's cryptographic
     hash functions (which CRC's are definitely not.)
     """
     
     def __init__(self, string=''):
          self.val = 0
          if string:
               self.update(string)
               
     def update(self, string):
          self.val = crcbitbybit(string)
                    
     def checksum(self):
          return chr(self.val >> 8) + chr(self.val & 0xff)


     def intchecksum(self):
          return self.val

     def hexchecksum(self):
          return '%04x' % self.val

     def copy(self):
          clone = CRC16()
          clone.val = self.val
          return clone



crc = CRC16()
#crc.update("123456789")
import struct
crc.update(struct.pack("20B", 0x1, 0x88, 0xe5, 0xff, 0xff, 0xff, 0xff, 0x10, 0x0, 0x10, 0x0, 0x1, 0x80, 0x80, 0xff, 0xff, 0x10, 0x0, 0x20, 0x0))

assert crc.checksum() == '\x02\x82'

