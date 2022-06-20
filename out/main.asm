section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss
ident_globalvariable resb 4

section .text

ident_f:
push ebp
mov ebp,esp
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 1
mov AL, byte [ebp+12]
mov byte [esp], AL
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
add dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
glob0:
mov esp,ebp
pop ebp
ret
glob1:
mov esp,ebp
pop ebp
ret


ident_main:
push ebp
mov ebp,esp
sub esp, 4
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 1
mov byte [esp], 01100001b
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
sub esp, 4
mov dword [esp], ident_f
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 5
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
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000011111111b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
inc dword [EAX]
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
add esp, 4
glob2:
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

