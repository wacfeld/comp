section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
mov dword [ebp-4], 7
sub esp, 4
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
mov eax, dword [esp]
add esp, 4
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
mov EBX, dword [esp]
mov dword [eax], EBX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
mov eax, dword [esp]
add esp, 4
sub esp, 4
mov dword [esp], 00000000000000000000000000001000b
mov EBX, dword [esp]
mov dword [eax], EBX
add esp, 4
mov esp,ebp
pop ebp
ret

global _start
_start:
call ident_main
call exit

error:
mov edx, errlen
mov ecx, errmsg
mov ebx, 1
mov eax, 4
int 0x80
exit:
mov eax, 1
mov ebx, 0
int 80h

