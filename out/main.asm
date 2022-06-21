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
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-8]
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
.lab0:
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab3
mov dword [esp], 0
jmp .lab4
.lab3: mov dword [esp], 1
.lab4:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab2
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-8]
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
.lab1:
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab0
.lab2:
add esp, 8
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

