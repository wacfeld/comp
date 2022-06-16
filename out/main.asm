section .data

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 20
sub esp, 4
mov dword [ebp-24], 7
sub esp, 4
lea eax, [ebp-20]
mov dword [esp], eax
add esp, 4
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

