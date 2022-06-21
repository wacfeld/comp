hey
hey
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
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000011b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
neg dword [esp]
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
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab3
.lab2:
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000010b
neg dword [esp]
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab4
mov dword [esp], 0
jmp .lab5
.lab4: mov dword [esp], 1
.lab5:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab6
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000010b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab7
.lab6:
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000011b
neg dword [esp]
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab8
mov dword [esp], 0
jmp .lab9
.lab8: mov dword [esp], 1
.lab9:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab10
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000011b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab11
.lab10:
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000100b
neg dword [esp]
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab12
mov dword [esp], 0
jmp .lab13
.lab12: mov dword [esp], 1
.lab13:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab14
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000100b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab15
.lab14:
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000010111111011b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
.lab15:
.lab11:
.lab7:
.lab3:
glob0:
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

