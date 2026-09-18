#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int IsPassOk(void);

int main(void)
{
	int PwStatus;

	puts("Enter password:");

	PwStatus = IsPassOk();

	if (PwStatus == 0)
    {
        printf("Bad password!\n");
        return -1;
	}
    else
    {
		printf("Access granted!\n");
	}

	return 0;
}

int IsPassOk(void)
{
	/* 	AAAABBBCCCC -> 12 bytes, or AAAABBBCCCCIIII if padding is enabled 
		on most 64-bit systems, there is 4 more bytes of padding (alignment) to make it 16 bytes */
	char Pass[12] = {0};
	/* DDDDEEEE - 8 bytes overwriting RBP (Base Pointer) in stack
		\xb5\x11\x40\x00\x00\x00\x00\x00 address of the call "printf("Access granted!"))" in little-endian
		address can be inspected via objdump -d prog | grep -A 30 "<main>:"
		something like this:
		40119d:	75 16                	jne    4011b5 <main+0x3f>
		4011b5 is the address in big-endian */

	/* gets() is unsafe and deprecated, and nobody should ever use it in production code */
	gets(Pass);

	/* RIP (Instruction Pointer) and RBP are at the top of the stack frame
		gets() writes from bottom upwards
		buffer overflow writes into RBP and RIP
		RBP gets corruped by DDDDEEEE part
		RIP is replaced by \xb5\x11\x40\x00\x00\x00\x00\x00

		00000000004011cb <IsPassOk>:
		  4011cb:	f3 0f 1e fa          	endbr64
		  4011cf:	55                   	push   %rbp ---> pushing the register's value to stack to save it for later usage
		  4011d0:	48 89 e5             	mov    %rsp,%rbp ---> setting rbp to rsp's value
		  4011d3:	48 83 ec 10          	sub    $0x10,%rsp ---> allocating 0x10 (16) bytes for local variables
		  4011d7:	48 c7 45 f4 00 00 00 	movq   $0x0,-0xc(%rbp) ---> setting memory at -0xc($rbp) with 0x0, -12 bytes frin rbp to -4 (8 bytes)
		  movq works with 8 bytes chunks of data, so only 8 bytes are zeroed out
		  4011de:	00 
		  4011df:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp) ---> setting memory -0x4($rbp) with 0x0, -4 bytes from rbp
		  movl works with 4 bytes chunks of data, the last 4 bytes are zeroed out
		  4011e6:	48 8d 45 f4          	lea    -0xc(%rbp),%rax gets() writes at -0xc(%rbp) (-12 bytes from rbo)
		  4011ea:	48 89 c7             	mov    %rax,%rdi
		  4011ed:	e8 8e fe ff ff       	call   401080 <gets@plt>
		  4011f2:	48 8d 45 f4          	lea    -0xc(%rbp),%rax
		  4011f6:	48 8d 15 35 0e 00 00 	lea    0xe35(%rip),%rdx        # 402032 <_IO_stdin_used+0x32>
		  4011fd:	48 89 d6             	mov    %rdx,%rsi
		  401200:	48 89 c7             	mov    %rax,%rdi
		  401203:	e8 68 fe ff ff       	call   401070 <strcmp@plt>
		  401208:	85 c0                	test   %eax,%eax
		  40120a:	0f 94 c0             	sete   %al
		  40120d:	0f b6 c0             	movzbl %al,%eax
		  401210:	c9                   	leave
		  401211:	c3                   	ret ---> returns to printf("Access Granted"); */

	return 0 == strcmp(Pass, "test");
}
