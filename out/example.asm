section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000110b
sub esp, 4
mov dword [esp], ident_factorial
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], EAX
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


ident_factorial:
push ebp
mov ebp,esp
sub esp, 4
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab0
mov dword [esp], 0
jmp .lab1
.lab0: mov dword [esp], 1
.lab1:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab2
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
mov esp,ebp
pop ebp
ret
jmp .lab3
.lab2:
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
sub dword [esp], EAX
sub esp, 4
mov dword [esp], ident_factorial
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
mul EBX
sub esp, 4
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov esp,ebp
pop ebp
ret
.lab3:
mov esp,ebp
pop ebp
ret

extern ident_putchar
extern ident_getchar
extern ident_exit
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

