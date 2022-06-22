
section .data
section .text

global ident_putchar
global ident_getchar

; prints a char to stdout
ident_putchar:
  mov edx, 1 ; len 1
  lea ecx, [esp+4] ; passed on stack, above eip (no ebp pushed, no stack frame)
  mov ebx, 1 ; stdout
  mov eax, 4 ; write syscall
  int 0x80

  ret ; bye
  

; reads a char from stdin
ident_getchar:
  mov edx, 1 ; len 1
  lea ecx, [esp-1] ; push onto stack
  mov ebx, 0 ; stdin
  mov eax, 3 ; read syscall
  int 0x80

  xor eax, eax ; zero eax
  mov al, byte [esp-1] ; put into eax

  ret ; bye
