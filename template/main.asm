section .data
a dd 5

section .bss
x resd 1

section .text
global _start
_start:
  mov eax, [x-4]

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

