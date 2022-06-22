section .data
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_putint:
push ebp
mov ebp,esp
sub esp, 104
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
jl .lab0
mov dword [esp], 0
jmp .lab1
.lab0: mov dword [esp], 1
.lab1:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab2
sub esp, 1
mov byte [esp], 00101101b
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
sub esp, 4
lea eax, [ebp+8]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
neg dword [esp]
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
.lab3:
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
je .lab4
mov dword [esp], 0
jmp .lab5
.lab4: mov dword [esp], 1
.lab5:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab6
sub esp, 1
mov byte [esp], 00110000b
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
mov esp,ebp
pop ebp
ret
jmp .lab7
.lab6:
.lab7:
sub esp, 4
lea eax, [ebp-104]
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
.lab8:
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab9
sub esp, 4
lea eax, [ebp-100]
mov dword [esp], eax
sub esp, 4
lea eax, [ebp-104]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
mov EAX, dword [esp]
add esp, 4
mov EBX, 1
mul EBX
add dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
cdq
idiv ebx
mul ebx
sub dword [esp], EAX
sub esp, 1
mov byte [esp], 00110000b
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
lea eax, [ebp+8]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
cdq
idiv ebx
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
jmp .lab8
.lab9:
sub esp, 4
lea eax, [ebp-104]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
dec dword [EAX]
add esp, 4
.lab10:
sub esp, 4
mov EAX, dword [ebp-104]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jge .lab13
mov dword [esp], 0
jmp .lab14
.lab13: mov dword [esp], 1
.lab14:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab12
sub esp, 4
lea eax, [ebp-100]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-104]
mov dword [esp], EAX
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
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
.lab11:
sub esp, 4
lea eax, [ebp-104]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
dec dword [EAX]
add esp, 4
jmp .lab10
.lab12:
mov esp,ebp
pop ebp
ret


ident_main:
push ebp
mov ebp,esp
sub esp, 52
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000110b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001011b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000010b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000111b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000100b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000110b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
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
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000001001b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000111b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000001000b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
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
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000011b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
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
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000000111b
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000010b
neg dword [esp]
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-44]
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
.lab15:
sub esp, 4
mov EAX, dword [ebp-44]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab18
mov dword [esp], 0
jmp .lab19
.lab18: mov dword [esp], 1
.lab19:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab17
sub esp, 4
lea eax, [ebp-48]
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
.lab20:
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001001b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab23
mov dword [esp], 0
jmp .lab24
.lab23: mov dword [esp], 1
.lab24:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab22
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov EAX, dword [eax]
mov dword [esp], EAX
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
add dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov EAX, dword [eax]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jg .lab25
mov dword [esp], 0
jmp .lab26
.lab25: mov dword [esp], 1
.lab26:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab27
sub esp, 4
lea eax, [ebp-52]
mov dword [esp], eax
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov EAX, dword [eax]
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
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
add dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov EAX, dword [eax]
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
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-48]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
add dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-52]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab28
.lab27:
.lab28:
.lab21:
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab20
.lab22:
.lab16:
sub esp, 4
lea eax, [ebp-44]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab15
.lab17:
sub esp, 4
lea eax, [ebp-44]
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
.lab29:
sub esp, 4
mov EAX, dword [ebp-44]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab32
mov dword [esp], 0
jmp .lab33
.lab32: mov dword [esp], 1
.lab33:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab31
sub esp, 4
lea eax, [ebp-40]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-44]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov EAX, dword [eax]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
.lab30:
sub esp, 4
lea eax, [ebp-44]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab29
.lab31:
sub esp, 1
mov byte [esp], 00001010b
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
sub esp, 4
mov dword [esp], 00000000000000000000000000001111b
sub esp, 4
mov dword [esp], ident_fib
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
mov esp,ebp
pop ebp
ret


ident_fib:
push ebp
mov ebp,esp
sub esp, 16
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
sub esp, 4
lea eax, [ebp-12]
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
.lab34:
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
sub esp, 4
cmp EAX, EBX
jl .lab37
mov dword [esp], 0
jmp .lab38
.lab37: mov dword [esp], 1
.lab38:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab36
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-16]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-12]
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
lea eax, [ebp-12]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-12]
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
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-16]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 1
.lab35:
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
jmp .lab34
.lab36:
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
je .lab39
mov dword [esp], 0
jmp .lab40
.lab39: mov dword [esp], 1
.lab40:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab41
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
mov esp,ebp
pop ebp
ret
jmp .lab42
.lab41:
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
.lab42:
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

