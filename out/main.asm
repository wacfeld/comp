section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 20
sub esp, 4
sub esp, 4
lea eax, [ebp-20]
mov dword [esp], eax
mov eax, [esp]
test eax, eax
je near error
add esp, 4
mov EAX, dword [eax]
mov dword[esp], EAX
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

