section .data
x dd 5

section .bss
section .text
global _start
_start:
  mov eax, x
  mov dword eax, [eax]
  jmp exit

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

