	.file	"dot_asm.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	_Z17dot_product_basicPKfS0_i
	.type	_Z17dot_product_basicPKfS0_i, @function
_Z17dot_product_basicPKfS0_i:
.LFB0:
	.cfi_startproc
	endbr64
	mov	rcx, rdi
	test	edx, edx
	jle	.L7
	lea	eax, -1[rdx]
	cmp	eax, 2
	jbe	.L8
	mov	edi, edx
	xor	eax, eax
	pxor	xmm0, xmm0
	shr	edi, 2
	sal	rdi, 4
	.p2align 4,,10
	.p2align 3
.L4:
	movups	xmm1, XMMWORD PTR [rcx+rax]
	movups	xmm3, XMMWORD PTR [rsi+rax]
	add	rax, 16
	mulps	xmm1, xmm3
	addss	xmm0, xmm1
	movaps	xmm2, xmm1
	shufps	xmm2, xmm1, 85
	addss	xmm0, xmm2
	movaps	xmm2, xmm1
	unpckhps	xmm2, xmm1
	shufps	xmm1, xmm1, 255
	addss	xmm0, xmm2
	addss	xmm0, xmm1
	cmp	rax, rdi
	jne	.L4
	mov	eax, edx
	and	eax, -4
	test	dl, 3
	je	.L11
.L3:
	movsx	r8, eax
	movss	xmm1, DWORD PTR [rcx+r8*4]
	mulss	xmm1, DWORD PTR [rsi+r8*4]
	lea	rdi, 0[0+r8*4]
	lea	r8d, 1[rax]
	addss	xmm0, xmm1
	cmp	edx, r8d
	jle	.L1
	movss	xmm1, DWORD PTR 4[rcx+rdi]
	mulss	xmm1, DWORD PTR 4[rsi+rdi]
	add	eax, 2
	addss	xmm0, xmm1
	cmp	edx, eax
	jle	.L1
	movss	xmm1, DWORD PTR 8[rsi+rdi]
	mulss	xmm1, DWORD PTR 8[rcx+rdi]
	addss	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L7:
	pxor	xmm0, xmm0
.L1:
	ret
	.p2align 4,,10
	.p2align 3
.L11:
	ret
.L8:
	xor	eax, eax
	pxor	xmm0, xmm0
	jmp	.L3
	.cfi_endproc
.LFE0:
	.size	_Z17dot_product_basicPKfS0_i, .-_Z17dot_product_basicPKfS0_i
	.p2align 4
	.globl	_Z20dot_product_restrictPKfS0_i
	.type	_Z20dot_product_restrictPKfS0_i, @function
_Z20dot_product_restrictPKfS0_i:
.LFB1:
	.cfi_startproc
	endbr64
	mov	rcx, rdi
	test	edx, edx
	jle	.L18
	lea	eax, -1[rdx]
	cmp	eax, 2
	jbe	.L19
	mov	edi, edx
	xor	eax, eax
	pxor	xmm0, xmm0
	shr	edi, 2
	sal	rdi, 4
	.p2align 4,,10
	.p2align 3
.L15:
	movups	xmm1, XMMWORD PTR [rcx+rax]
	movups	xmm3, XMMWORD PTR [rsi+rax]
	add	rax, 16
	mulps	xmm1, xmm3
	addss	xmm0, xmm1
	movaps	xmm2, xmm1
	shufps	xmm2, xmm1, 85
	addss	xmm0, xmm2
	movaps	xmm2, xmm1
	unpckhps	xmm2, xmm1
	shufps	xmm1, xmm1, 255
	addss	xmm0, xmm2
	addss	xmm0, xmm1
	cmp	rax, rdi
	jne	.L15
	mov	eax, edx
	and	eax, -4
	test	dl, 3
	je	.L21
.L14:
	movsx	r8, eax
	movss	xmm1, DWORD PTR [rcx+r8*4]
	mulss	xmm1, DWORD PTR [rsi+r8*4]
	lea	rdi, 0[0+r8*4]
	lea	r8d, 1[rax]
	addss	xmm0, xmm1
	cmp	edx, r8d
	jle	.L12
	movss	xmm1, DWORD PTR 4[rcx+rdi]
	mulss	xmm1, DWORD PTR 4[rsi+rdi]
	add	eax, 2
	addss	xmm0, xmm1
	cmp	edx, eax
	jle	.L12
	movss	xmm1, DWORD PTR 8[rsi+rdi]
	mulss	xmm1, DWORD PTR 8[rcx+rdi]
	addss	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L18:
	pxor	xmm0, xmm0
.L12:
	ret
	.p2align 4,,10
	.p2align 3
.L21:
	ret
.L19:
	xor	eax, eax
	pxor	xmm0, xmm0
	jmp	.L14
	.cfi_endproc
.LFE1:
	.size	_Z20dot_product_restrictPKfS0_i, .-_Z20dot_product_restrictPKfS0_i
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
