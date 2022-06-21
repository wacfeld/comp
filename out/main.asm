section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_main:
push ebp
mov ebp,esp
sub esp, 4
jmp goto_thing
goto_thing:
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
jmp goto_thing
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

