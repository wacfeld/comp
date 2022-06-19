section .data

section .bss
section .text
global _start
_start:
mov eax, -1
mov ebx, 2
imul ebx

mov eax, -1
mov ebx, 2
mul ebx

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

