section .data
ident_identifier dd 00000000000000000000000000000101b
glob0 db 91, 0, 
glob1 db 44, 32, 0, 
glob2 db 93, 10, 0, 0, 
glob3 db 105, 100, 101, 110, 116, 105, 102, 105, 101, 114, 32, 40, 103, 108, 111, 98, 97, 108, 41, 58, 0, 
glob4 db 105, 100, 101, 110, 116, 105, 102, 105, 101, 114, 32, 40, 108, 111, 99, 97, 108, 41, 58, 0, 
glob5 db 105, 100, 101, 110, 116, 105, 102, 105, 101, 114, 32, 40, 103, 108, 111, 98, 97, 108, 41, 58, 0, 
glob6 db 98, 117, 98, 98, 108, 101, 32, 115, 111, 114, 116, 58, 10, 0, 0, 
glob7 db 102, 105, 98, 111, 110, 97, 99, 99, 105, 32, 115, 101, 113, 117, 101, 110, 99, 101, 58, 0, 
glob8 db 102, 97, 99, 116, 111, 114, 105, 97, 108, 32, 40, 114, 101, 99, 117, 114, 115, 105, 111, 110, 41, 0, 
glob9 db 112, 114, 101, 115, 115, 32, 97, 110, 121, 32, 107, 101, 121, 32, 116, 111, 32, 99, 111, 110, 116, 105, 110, 117, 101, 32, 40, 103, 101, 116, 99, 104, 97, 114, 40, 41, 41, 32, 0, 
glob10 db 121, 111, 117, 32, 105, 110, 112, 117, 116, 116, 101, 100, 32, 116, 104, 101, 32, 99, 104, 97, 114, 97, 99, 116, 101, 114, 32, 119, 105, 116, 104, 32, 118, 97, 108, 117, 101, 32, 0, 
glob11 db 100, 101, 109, 111, 110, 115, 116, 114, 97, 116, 105, 111, 110, 32, 111, 102, 32, 103, 111, 116, 111, 58, 0, 
glob12 db 101, 120, 105, 116, 105, 110, 103, 32, 110, 101, 115, 116, 101, 100, 32, 108, 111, 111, 112, 0, 
glob13 db 104, 105, 0, 
glob14 db 112, 114, 105, 110, 116, 32, 101, 118, 101, 114, 121, 32, 110, 117, 109, 98, 101, 114, 32, 110, 111, 116, 32, 100, 105, 118, 105, 115, 105, 98, 108, 101, 32, 98, 121, 32, 54, 32, 40, 99, 111, 110, 116, 105, 110, 117, 101, 41, 58, 0, 
glob15 db 99, 111, 117, 110, 116, 32, 102, 114, 111, 109, 32, 110, 42, 49, 48, 32, 116, 111, 32, 40, 110, 43, 49, 41, 42, 49, 48, 44, 32, 98, 117, 116, 32, 115, 116, 111, 112, 32, 119, 104, 101, 110, 32, 97, 32, 109, 117, 108, 116, 105, 112, 108, 101, 32, 111, 102, 32, 55, 32, 105, 115, 32, 114, 101, 97, 99, 104, 101, 100, 32, 40, 98, 114, 101, 97, 107, 41, 58, 0, 
glob16 db 109, 117, 108, 116, 105, 100, 105, 109, 101, 110, 115, 105, 111, 110, 97, 108, 32, 97, 114, 114, 97, 121, 115, 58, 0, 
errmsg db "error", 10
errlen equ $ - errmsg

section .bss

section .text

ident_putstr:
push ebp
mov ebp,esp
.lab0:
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 1
mov AL, byte [eax]
mov byte [esp], AL
mov AL, byte [esp]
add esp, 1
test AL, AL
jz .lab1
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 1
mov AL, byte [eax]
mov byte [esp], AL
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp+8]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
add dword [EAX], 1
add esp, 4
jmp .lab0
.lab1:
mov esp,ebp
pop ebp
ret


ident_puts:
push ebp
mov ebp,esp
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
mov esp,ebp
pop ebp
ret


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
jl .lab2
mov dword [esp], 0
jmp .lab3
.lab2: mov dword [esp], 1
.lab3:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab4
sub esp, 1
mov byte [esp], 00101101b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
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
jmp .lab5
.lab4:
.lab5:
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
.lab6:
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
.lab7:
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jnz .lab6
.lab8:
.lab9:
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab10
jmp .lab9
.lab10:
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
.lab11:
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
jge .lab14
mov dword [esp], 0
jmp .lab15
.lab14: mov dword [esp], 1
.lab15:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab13
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
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
.lab12:
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
jmp .lab11
.lab13:
mov esp,ebp
pop ebp
ret


ident_putintarr:
push ebp
mov ebp,esp
sub esp, 4
sub esp, 4
mov dword [esp], glob0
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
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
.lab16:
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp+12]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab19
mov dword [esp], 0
jmp .lab20
.lab19: mov dword [esp], 1
.lab20:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab18
sub esp, 4
mov EAX, dword [ebp+8]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-4]
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
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp+12]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
sub dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jne .lab21
mov dword [esp], 0
jmp .lab22
.lab21: mov dword [esp], 1
.lab22:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab23
sub esp, 4
mov dword [esp], glob1
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
jmp .lab24
.lab23:
.lab24:
.lab17:
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
jmp .lab16
.lab18:
sub esp, 4
mov dword [esp], glob2
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
mov esp,ebp
pop ebp
ret


ident_main:
push ebp
mov ebp,esp
sub esp, 488
sub esp, 4
mov dword [esp], glob3
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ident_identifier]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-4]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
mov dword [esp], glob4
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
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
sub esp, 4
mov EAX, dword [ebp-4]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob5
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ident_identifier]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-8]
mov dword [esp], eax
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
sub esp, 4
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
lea eax, [ebp-48]
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
mov dword [esp], glob6
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov dword [esp], ident_putintarr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 8
sub esp, 4
lea eax, [ebp-52]
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
.lab25:
sub esp, 4
mov EAX, dword [ebp-52]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab28
mov dword [esp], 0
jmp .lab29
.lab28: mov dword [esp], 1
.lab29:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab27
sub esp, 4
lea eax, [ebp-56]
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
.lab30:
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001001b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab33
mov dword [esp], 0
jmp .lab34
.lab33: mov dword [esp], 1
.lab34:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab32
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
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
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
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
jg .lab35
mov dword [esp], 0
jmp .lab36
.lab35: mov dword [esp], 1
.lab36:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab37
sub esp, 4
lea eax, [ebp-60]
mov dword [esp], eax
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
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
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
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
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
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
mov EAX, dword [ebp-60]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
jmp .lab38
.lab37:
.lab38:
.lab31:
sub esp, 4
lea eax, [ebp-56]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab30
.lab32:
.lab26:
sub esp, 4
lea eax, [ebp-52]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab25
.lab27:
sub esp, 4
mov EAX, dword [ebp-8]
mov dword [esp], EAX
sub esp, 4
lea eax, [ebp-48]
mov dword [esp], eax
sub esp, 4
mov dword [esp], ident_putintarr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 8
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob7
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], 00000000000000000000000000001111b
sub esp, 4
mov dword [esp], ident_fib
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob8
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
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
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-64]
mov dword [esp], eax
sub esp, 4
mov dword [esp], glob9
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
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-64]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-68]
mov dword [esp], eax
sub esp, 4
mov dword [esp], ident_getchar
mov EAX, dword [esp]
add esp, 4
call EAX
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
mov dword [esp], glob10
sub esp, 4
mov dword [esp], ident_putstr
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-68]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob11
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-72]
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
lea eax, [ebp-52]
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
.lab39:
sub esp, 4
mov EAX, dword [ebp-52]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab42
mov dword [esp], 0
jmp .lab43
.lab42: mov dword [esp], 1
.lab43:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab41
sub esp, 4
lea eax, [ebp-56]
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
.lab44:
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000101b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab47
mov dword [esp], 0
jmp .lab48
.lab47: mov dword [esp], 1
.lab48:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab46
sub esp, 4
lea eax, [ebp-72]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
sub esp, 4
mov EAX, dword [ebp-72]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov EAX, dword [ebp-72]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001101b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab49
mov dword [esp], 0
jmp .lab50
.lab49: mov dword [esp], 1
.lab50:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab51
sub esp, 4
mov dword [esp], glob12
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
jmp goto_endloop
jmp .lab52
.lab51:
.lab52:
.lab45:
sub esp, 4
lea eax, [ebp-56]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab44
.lab46:
.lab40:
sub esp, 4
lea eax, [ebp-52]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab39
.lab41:
goto_endloop:
sub esp, 4
mov dword [esp], glob13
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob14
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-76]
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
.lab53:
sub esp, 4
mov EAX, dword [ebp-76]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000011110b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab56
mov dword [esp], 0
jmp .lab57
.lab56: mov dword [esp], 1
.lab57:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab55
sub esp, 4
mov EAX, dword [ebp-76]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000110b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
cdq
idiv ebx
mul ebx
sub dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab58
mov dword [esp], 0
jmp .lab59
.lab58: mov dword [esp], 1
.lab59:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab60
jmp .lab54
jmp .lab61
.lab60:
.lab61:
sub esp, 4
mov EAX, dword [ebp-76]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00100000b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
.lab54:
sub esp, 4
lea eax, [ebp-76]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab53
.lab55:
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob15
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-76]
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
.lab62:
sub esp, 4
mov EAX, dword [ebp-76]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab65
mov dword [esp], 0
jmp .lab66
.lab65: mov dword [esp], 1
.lab66:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab64
sub esp, 4
lea eax, [ebp-80]
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
.lab67:
sub esp, 4
mov EAX, dword [ebp-80]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab70
mov dword [esp], 0
jmp .lab71
.lab70: mov dword [esp], 1
.lab71:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab69
sub esp, 4
lea eax, [ebp-84]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-76]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
mul EBX
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-80]
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
mov EAX, dword [ebp-84]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000111b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
cdq
idiv ebx
mul ebx
sub dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000000000b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
je .lab72
mov dword [esp], 0
jmp .lab73
.lab72: mov dword [esp], 1
.lab73:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab74
jmp .lab69
jmp .lab75
.lab74:
.lab75:
sub esp, 4
mov EAX, dword [ebp-84]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putint
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 1
mov byte [esp], 00100000b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
.lab68:
sub esp, 4
lea eax, [ebp-80]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab67
.lab69:
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
.lab63:
sub esp, 4
lea eax, [ebp-76]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab62
.lab64:
sub esp, 1
mov byte [esp], 00001010b
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
mov dword [esp], glob16
sub esp, 4
mov dword [esp], ident_puts
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
sub esp, 4
lea eax, [ebp-52]
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
.lab76:
sub esp, 4
mov EAX, dword [ebp-52]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab79
mov dword [esp], 0
jmp .lab80
.lab79: mov dword [esp], 1
.lab80:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab78
sub esp, 4
lea eax, [ebp-56]
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
.lab81:
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
sub esp, 4
mov dword [esp], 00000000000000000000000000001010b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab84
mov dword [esp], 0
jmp .lab85
.lab84: mov dword [esp], 1
.lab85:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab83
sub esp, 4
lea eax, [ebp-484]
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-52]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 40
mul EBX
add dword [esp], EAX
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov dword [esp], eax
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
mov EAX, dword [esp]
add esp, 4
mov EBX, 4
mul EBX
add dword [esp], EAX
sub esp, 4
mov EAX, dword [ebp-56]
mov dword [esp], EAX
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
.lab82:
sub esp, 4
lea eax, [ebp-56]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab81
.lab83:
.lab77:
sub esp, 4
lea eax, [ebp-52]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
inc dword [EAX]
add esp, 4
jmp .lab76
.lab78:
sub esp, 4
lea eax, [ebp-488]
mov dword [esp], eax
sub esp, 4
lea eax, [ebp-484]
mov dword [esp], eax
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov dword [esp], eax
mov EBX, dword [esp]
add esp, 4
mov eax, dword [esp]
add esp, 4
mov dword [eax], EBX
sub esp, 4
mov dword [esp], EBX
add esp, 4
.lab86:
sub esp, 4
mov EAX, dword [ebp-488]
mov dword [esp], EAX
sub esp, 4
lea eax, [ebp-484]
mov dword [esp], eax
mov eax, dword [esp]
test eax, eax
je near error
add esp, 4
sub esp, 4
mov dword [esp], eax
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
sub esp, 4
mov dword [esp], 00000000000000000000000001100100b
mov EBX, dword [esp]
add esp, 4
mov EAX, dword [esp]
add esp, 4
sub esp, 4
cmp EAX, EBX
jl .lab88
mov dword [esp], 0
jmp .lab89
.lab88: mov dword [esp], 1
.lab89:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab87
sub esp, 4
mov EAX, dword [ebp-488]
mov dword [esp], EAX
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
sub esp, 4
lea eax, [ebp-488]
mov dword [esp], eax
mov EAX, dword [esp]
add esp, 4
sub esp, 4
mov EBX, dword [EAX]
mov dword [esp], EBX
add dword [EAX], 4
add esp, 4
jmp .lab86
.lab87:
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
.lab90:
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
jl .lab93
mov dword [esp], 0
jmp .lab94
.lab93: mov dword [esp], 1
.lab94:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab92
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
mov AL, byte [esp]
add esp, 1
movsx EAX, AL
sub esp, 4
mov dword [esp], EAX
sub esp, 4
mov dword [esp], ident_putchar
mov EAX, dword [esp]
add esp, 4
call EAX
add esp, 4
.lab91:
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
jmp .lab90
.lab92:
mov esp,ebp
pop ebp
ret


ident_factorial:
push ebp
mov ebp,esp
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
je .lab95
mov dword [esp], 0
jmp .lab96
.lab95: mov dword [esp], 1
.lab96:
mov EAX, dword [esp]
add esp, 4
test EAX, EAX
jz .lab97
sub esp, 4
mov dword [esp], 00000000000000000000000000000001b
mov EAX, dword [esp]
add esp, 4
mov esp,ebp
pop ebp
ret
jmp .lab98
.lab97:
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
.lab98:
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

