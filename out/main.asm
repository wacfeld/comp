section .data
ident_x dd 00000000000000000000000000000000b

section .bss

section .code
mov dword [ebp-4], 00000000000000000000000000000101b
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

