#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 192
#define BLACK 0
#define WHITE 15
#define RED   12
#define BLUE  9

#define HELLO_STR_ADDR 1000
#define SMILEY_BMP_ADDR 2000

start:
    MOV R0, SCREEN_WIDTH
    MOV R1, SCREEN_HEIGHT
    INT 0x18      

    MOV R0, BLACK
    INT 0x15      

    INT 0x16      

    STRMOV HELLO_STR_ADDR, "Hello World!"

    MOV R0, 10      
    MOV R1, 10      
    MOV R2, HELLO_STR_ADDR 
    MOV R3, WHITE   
    INT 0x41        

    MOV R0, 10
    MOV R1, 30
    STRMOV HELLO_STR_ADDR + 20, "Testing 123..." 
    MOV R2, HELLO_STR_ADDR + 20
    MOV R3, RED
    INT 0x41

    MOV R5, SMILEY_BMP_ADDR 
    MOV R6, 0       
    MOV R7, 64      
    MOV R8, R5      
    memset_loop:
        STORE R6, R8    
        INC R8
        DEC R7
        CMP R7, R6      
        JNE memset_loop 

    MOV R10, 14     
    MOV R0, R5      
    LEA R1, R0, 18  
    STORE R10, R1
    LEA R1, R0, 21  
    STORE R10, R1
    LEA R1, R0, 41  
    STORE R10, R1
    LEA R1, R0, 42
    STORE R10, R1
    LEA R1, R0, 43
    STORE R10, R1
    LEA R1, R0, 44
    STORE R10, R1
    LEA R1, R0, 45
    STORE R10, R1
    LEA R1, R0, 46
    STORE R10, R1
    LEA R1, R0, 33  
    STORE R10, R1
    LEA R1, R0, 38  
    STORE R10, R1

    MOV R0, 100     
    MOV R1, 50      
    MOV R2, SMILEY_BMP_ADDR 
    MOV R3, 8       
    MOV R4, 8       
    INT 0x42        

    MOV R0, 120
    MOV R1, 60
    INT 0x42

end_loop:
    JMP end_loop    

HLT             
