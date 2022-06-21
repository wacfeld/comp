section .data

msg db 97, 98, 10, 0,
len equ $-msg

section .bss
section .text
global _start
_start:

test eax, eax
jz .here

times 50 nop

.here:
mov edx,len
mov ecx,msg
mov ebx,1
mov eax,4
int 0x80

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

