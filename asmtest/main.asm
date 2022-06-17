section .data

section .bss
section .text
global _start
_start:
  mov al, -5
  movsx eax, al
  jmp exit

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

