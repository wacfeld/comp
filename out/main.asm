section .data

section .bss

section .code
ident_f:
push ebp
mov ebp,esp
mov esp,ebp
pop ebp
ret
mov esp,ebp
pop ebp
ret
ident_main:
push ebp
mov ebp,esp
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

