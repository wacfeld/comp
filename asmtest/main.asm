section .data

section .bss
section .text
global _start
_start:
  mov ax, 0xffff
  mov al, 5
  jmp exit

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

