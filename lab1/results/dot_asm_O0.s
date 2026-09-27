	.file	"dot_asm.cpp"
	.intel_syntax noprefix
	.text
	.globl	"_Z17dot_product_basicPKfS0_i"
	.type	"_Z17dot_product_basicPKfS0_i", @function
"_Z17dot_product_basicPKfS0_i":
.LFB0:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -24[rbp], rdi
	mov	QWORD PTR -32[rbp], rsi
	mov	DWORD PTR -36[rbp], edx
	pxor	xmm0, xmm0
	movss	DWORD PTR -8[rbp], xmm0
	mov	DWORD PTR -4[rbp], 0
	jmp	.L2
.L3:
	mov	eax, DWORD PTR -4[rbp]
	cdqe
	lea	rdx, 0[0+rax*4]
	mov	rax, QWORD PTR -24[rbp]
	add	rax, rdx
	movss	xmm1, DWORD PTR [rax]
	mov	eax, DWORD PTR -4[rbp]
	cdqe
	lea	rdx, 0[0+rax*4]
	mov	rax, QWORD PTR -32[rbp]
	add	rax, rdx
	movss	xmm0, DWORD PTR [rax]
	mulss	xmm0, xmm1
	movss	xmm1, DWORD PTR -8[rbp]
	addss	xmm0, xmm1
	movss	DWORD PTR -8[rbp], xmm0
	add	DWORD PTR -4[rbp], 1
.L2:
	mov	eax, DWORD PTR -4[rbp]
	cmp	eax, DWORD PTR -36[rbp]
	jl	.L3
	movss	xmm0, DWORD PTR -8[rbp]
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	"_Z17dot_product_basicPKfS0_i", .-"_Z17dot_product_basicPKfS0_i"
	.globl	"_Z20dot_product_restrictPKfS0_i"
	.type	"_Z20dot_product_restrictPKfS0_i", @function
"_Z20dot_product_restrictPKfS0_i":
.LFB1:
	.cfi_startproc
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	mov	QWORD PTR -24[rbp], rdi
	mov	QWORD PTR -32[rbp], rsi
	mov	DWORD PTR -36[rbp], edx
	pxor	xmm0, xmm0
	movss	DWORD PTR -8[rbp], xmm0
	mov	DWORD PTR -4[rbp], 0
	jmp	.L6
.L7:
	mov	eax, DWORD PTR -4[rbp]
	cdqe
	lea	rdx, 0[0+rax*4]
	mov	rax, QWORD PTR -24[rbp]
	add	rax, rdx
	movss	xmm1, DWORD PTR [rax]
	mov	eax, DWORD PTR -4[rbp]
	cdqe
	lea	rdx, 0[0+rax*4]
	mov	rax, QWORD PTR -32[rbp]
	add	rax, rdx
	movss	xmm0, DWORD PTR [rax]
	mulss	xmm0, xmm1
	movss	xmm1, DWORD PTR -8[rbp]
	addss	xmm0, xmm1
	movss	DWORD PTR -8[rbp], xmm0
	add	DWORD PTR -4[rbp], 1
.L6:
	mov	eax, DWORD PTR -4[rbp]
	cmp	eax, DWORD PTR -36[rbp]
	jl	.L7
	movss	xmm0, DWORD PTR -8[rbp]
	pop	rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	"_Z20dot_product_restrictPKfS0_i", .-"_Z20dot_product_restrictPKfS0_i"
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
