section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
mov dword [ebp-4], 5
sub esp, 1
mov byte [ebp-5], 6
sub esp, 4
sub esp, 4
lea eax, [ebp-9]
mov dword [esp], eax
sub esp, 1
mov AL, byte [ebp-5]
mov byte [esp], AL
mov AL, byte [esp]
add esp, 1
movzx EAX, AL
mov EBX, 4
mul EBX
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
add dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
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

