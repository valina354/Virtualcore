#define MY_VALUE 12345
#define ADD_VALUE 100
MOV R0, MY_VALUE 
MOV R1, ADD_VALUE
INT 0x01
ADD R0, R1
INT 0x01
HLT
