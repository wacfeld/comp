section .data

section .bss

section .text

global _start
_start:
  push ebp
  mov ebp, esp
  mov dword [ebp-100b], 4
  mov dword eax, [ebp-4]
.exit:
  mov eax, 1
  mov ebx, 0
  int 80h
