#define N 15

; Register Usage:
; R0: Holds F(n-1) - the previous Fibonacci number (also used for CMP with 0)
; R1: Holds F(n) - the current Fibonacci number
; R2: Loop counter (decrements from N)
; R3: Temporary register for calculating F(n+1)

start:
    MOV R0, 0       
    MOV R1, 1       
    MOV R2, N       

    CMP R2, R0      
    JLE end_program 

fib_loop:
    INT 0x01        

    MOV R3, R0      
    ADD R3, R1      
    JO overflow_error 

    MOV R0, R1      
    MOV R1, R3      

    LOOP R2, fib_loop 

    JMP end_program 

overflow_error:
    HLT             

end_program:
    HLT             
