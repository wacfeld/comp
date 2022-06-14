section .data
ident_x dd 00000000000000000000000000000000b

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
mov dword [ebp-4], 5
sub esp, 4
mov dword [ebp-8], 10
sub esp, 4
mov dword [ebp-13], 15
mov esp,ebp
pop ebp

ret
global _start
_start:
  call ident_main
  call exit

exit:
  mov eax, 1
  mov ebx, 0
  int 80h

