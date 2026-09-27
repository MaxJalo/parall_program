	.file	"dot_asm.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	"_Z17dot_product_basicPKfS0_i"
	.type	"_Z17dot_product_basicPKfS0_i", @function
"_Z17dot_product_basicPKfS0_i":
.LFB0:
	.cfi_startproc
	mov	r8d, edx
	test	edx, edx
	jle	.L5
	lea	eax, -1[rdx]
	cmp	eax, 2
	jbe	.L6
	mov	ecx, edx
	xor	eax, eax
	pxor	xmm0, xmm0
	shr	ecx, 2
	mov	edx, ecx
	sal	rdx, 4
	.p2align 6
	.p2align 4
	.p2align 3
.L4:
	movups	xmm1, XMMWORD PTR [rdi+rax]
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
	cmp	rax, rdx
	jne	.L4
	lea	eax, 0[0+rcx*4]
	cmp	r8d, eax
	je	.L1
.L3:
	mov	edx, eax
	lea	ecx, 1[rax]
	movss	xmm1, DWORD PTR [rdi+rdx*4]
	mulss	xmm1, DWORD PTR [rsi+rdx*4]
	addss	xmm0, xmm1
	cmp	r8d, ecx
	jle	.L1
	movss	xmm1, DWORD PTR 4[rdi+rdx*4]
	mulss	xmm1, DWORD PTR 4[rsi+rdx*4]
	add	eax, 2
	addss	xmm0, xmm1
	cmp	r8d, eax
	jle	.L1
	movss	xmm1, DWORD PTR 8[rsi+rdx*4]
	mulss	xmm1, DWORD PTR 8[rdi+rdx*4]
	addss	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L5:
	pxor	xmm0, xmm0
.L1:
	ret
.L6:
	xor	eax, eax
	pxor	xmm0, xmm0
	jmp	.L3
	.cfi_endproc
.LFE0:
	.size	"_Z17dot_product_basicPKfS0_i", .-"_Z17dot_product_basicPKfS0_i"
	.p2align 4
	.globl	"_Z20dot_product_restrictPKfS0_i"
	.type	"_Z20dot_product_restrictPKfS0_i", @function
"_Z20dot_product_restrictPKfS0_i":
.LFB1:
	.cfi_startproc
	mov	r8d, edx
	test	edx, edx
	jle	.L13
	lea	eax, -1[rdx]
	cmp	eax, 2
	jbe	.L14
	mov	ecx, edx
	xor	eax, eax
	pxor	xmm0, xmm0
	shr	ecx, 2
	mov	edx, ecx
	sal	rdx, 4
	.p2align 6
	.p2align 4
	.p2align 3
.L12:
	movups	xmm1, XMMWORD PTR [rdi+rax]
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
	cmp	rax, rdx
	jne	.L12
	lea	eax, 0[0+rcx*4]
	cmp	r8d, eax
	je	.L9
.L11:
	mov	edx, eax
	lea	ecx, 1[rax]
	movss	xmm1, DWORD PTR [rdi+rdx*4]
	mulss	xmm1, DWORD PTR [rsi+rdx*4]
	addss	xmm0, xmm1
	cmp	r8d, ecx
	jle	.L9
	movss	xmm1, DWORD PTR 4[rdi+rdx*4]
	mulss	xmm1, DWORD PTR 4[rsi+rdx*4]
	add	eax, 2
	addss	xmm0, xmm1
	cmp	r8d, eax
	jle	.L9
	movss	xmm1, DWORD PTR 8[rsi+rdx*4]
	mulss	xmm1, DWORD PTR 8[rdi+rdx*4]
	addss	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L13:
	pxor	xmm0, xmm0
.L9:
	ret
.L14:
	xor	eax, eax
	pxor	xmm0, xmm0
	jmp	.L11
	.cfi_endproc
.LFE1:
	.size	"_Z20dot_product_restrictPKfS0_i", .-"_Z20dot_product_restrictPKfS0_i"
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
