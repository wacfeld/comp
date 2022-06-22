section .data
glob0 db 104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 0, 
errmsg db "error", 10
errlen equ $ - errmsg

section .bss
ident_s resb 4

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 1
sub esp, 4
mov dword [esp], ident_s
sub esp, 4
mov dword [esp], glob0
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-1]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ident_s]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EAX, dword [esp]
add esp, 4
mov EBX, 1
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 1
mov AL, byte [eax]
mov byte [esp], AL
mov BL, byte [esp]
add esp, 1
mov eax, dword [esp]
add esp, 4
mov byte [eax], BL
sub esp, 1
mov byte [esp], BL
add esp, 1
glob1:
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

