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
sub esp, 120
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
lea eax, [ebp-132]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
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
lea eax, [ebp-132]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000001100b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-12]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub EAX, EBX
cdq
mov EBX, 4
idiv EBX
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

