section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
mov dword [ebp-4], 1
sub esp, 1
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
mov eax, [esp]
test eax, eax
je near error
add esp, 4
sub esp, 1
mov AL, byte [eax]
mov byte[esp], AL
add esp, 1
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

