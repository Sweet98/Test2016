#!/usr/bin/env python
"""
AppEncrypt is a command line tool for encrtpy our bin file
"""
import sys
import os
import struct
import optparse
#import time

def EncryptFile(InputFile, OutputFile):
    bytesInFile = InputFile.read()
    if bytesInFile != "":
        EncrBytes = ""
        for byte in bytesInFile:
            eb = struct.unpack('b', byte)
            eb = struct.pack('b', ~eb[0])
            EncrBytes = EncrBytes + eb
        OutputFile.write(EncrBytes)

def main():
    parser = optparse.OptionParser(usage="AppEncrypt -i [INPUT_FILE] -o [OUTPUT_FILE]", \
            version="%prog V1.0", \
            description="AppEncrypt is a command line tool for encrtpy our bin file")
    parser.add_option('-i', '--input', dest="input_file", help="file name for input")
    parser.add_option('-o', '--output', dest="output_file", help="file name of encrypted file for output")
    (options, args) = parser.parse_args()

    try:
        with open(options.input_file, "rb") as OriginBin, open(options.output_file, "wb") as EncryptedBin:
            #start_time = time.time()
            EncryptFile(OriginBin, EncryptedBin)
            #print("--- %s seconds ---" % (time.time() - start_time))
    except Exception as e:
        print(e)

if __name__ == '__main__':
    main()
