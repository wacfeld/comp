section .data
a dd 5
b db 'a'

section .bss
x resd 1

section .text
global _start
_start:
  mov al, byte [b]

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

