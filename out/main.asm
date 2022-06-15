section .data

section .bss
ident_x resb 4

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
mov dword [esp], ident_x
add esp, 20
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

