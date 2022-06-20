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
mov dword [esp], 00000000000000000000000000000001b
neg dword [esp]
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 1
sub esp, 4
lea eax, [ebp-9]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
neg dword [esp]
mov EAX, dword [esp]
add esp, 4
sub esp, 1
mov byte [esp], AL
mov BL, byte [esp]
add esp, 1
mov eax, dword [esp]
add esp, 4
mov byte [eax], BL
sub esp, 1
mov byte [esp], BL
add esp, 1
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 1
mov AL, byte [ebp-9]
mov byte [esp], AL
mov AL, byte [esp]
add esp, 1
movzx EAX, AL
sub esp, 4
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jae .lab0
mov dword [esp], 0
jmp .lab1
.lab0: mov dword [esp], 1
.lab1:
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
dbg: mov esp,ebp
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

