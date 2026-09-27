	.file	"dot_asm.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	"_Z17dot_product_basicPKfS0_i"
	.type	"_Z17dot_product_basicPKfS0_i", @function
"_Z17dot_product_basicPKfS0_i":
.LFB0:
	.cfi_startproc
	test	edx, edx
	mov	r8d, edx
	jle	.L6
	lea	eax, -1[rdx]
	cmp	eax, 6
	jbe	.L7
	mov	ecx, edx
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
	shr	ecx, 3
	mov	edx, ecx
	sal	rdx, 5
	.p2align 4,,10
	.p2align 3
.L4:
	vmovups	ymm1, YMMWORD PTR [rsi+rax]
	vmulps	ymm1, ymm1, YMMWORD PTR [rdi+rax]
	add	rax, 32
	cmp	rdx, rax
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm3, xmm1, xmm1, 85
	vshufps	xmm2, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm3
	vunpckhps	xmm3, xmm1, xmm1
	vextractf128	xmm1, ymm1, 0x1
	vaddss	xmm0, xmm0, xmm3
	vaddss	xmm0, xmm0, xmm2
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm1
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	jne	.L4
	lea	eax, 0[0+rcx*8]
	cmp	r8d, eax
	je	.L9
	mov	edx, eax
	vzeroupper
.L3:
	mov	ecx, r8d
	sub	ecx, eax
	lea	r9d, -1[rcx]
	cmp	r9d, 2
	jbe	.L5
	vmovups	xmm1, XMMWORD PTR [rsi+rax*4]
	vmulps	xmm1, xmm1, XMMWORD PTR [rdi+rax*4]
	mov	eax, ecx
	and	eax, -4
	and	ecx, 3
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	je	.L1
	add	edx, eax
.L5:
	lea	ecx, 1[rdx]
	mov	eax, edx
	cmp	r8d, ecx
	vmovss	xmm4, DWORD PTR [rdi+rax*4]
	vfmadd231ss	xmm0, xmm4, DWORD PTR [rsi+rax*4]
	jle	.L1
	add	edx, 2
	vmovss	xmm5, DWORD PTR 4[rsi+rax*4]
	vfmadd231ss	xmm0, xmm5, DWORD PTR 4[rdi+rax*4]
	cmp	r8d, edx
	jle	.L1
	vmovss	xmm6, DWORD PTR 8[rdi+rax*4]
	vfmadd231ss	xmm0, xmm6, DWORD PTR 8[rsi+rax*4]
	ret
	.p2align 4,,10
	.p2align 3
.L9:
	vzeroupper
.L1:
	ret
	.p2align 4,,10
	.p2align 3
.L6:
	vxorps	xmm0, xmm0, xmm0
	ret
.L7:
	xor	eax, eax
	xor	edx, edx
	vxorps	xmm0, xmm0, xmm0
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
	test	edx, edx
	mov	r8d, edx
	jle	.L16
	lea	eax, -1[rdx]
	cmp	eax, 6
	jbe	.L17
	mov	ecx, edx
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
	shr	ecx, 3
	mov	edx, ecx
	sal	rdx, 5
	.p2align 4,,10
	.p2align 3
.L14:
	vmovups	ymm1, YMMWORD PTR [rsi+rax]
	vmulps	ymm1, ymm1, YMMWORD PTR [rdi+rax]
	add	rax, 32
	cmp	rdx, rax
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm3, xmm1, xmm1, 85
	vshufps	xmm2, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm3
	vunpckhps	xmm3, xmm1, xmm1
	vextractf128	xmm1, ymm1, 0x1
	vaddss	xmm0, xmm0, xmm3
	vaddss	xmm0, xmm0, xmm2
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm1
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	jne	.L14
	lea	eax, 0[0+rcx*8]
	cmp	r8d, eax
	je	.L19
	mov	edx, eax
	vzeroupper
.L13:
	mov	ecx, r8d
	sub	ecx, eax
	lea	r9d, -1[rcx]
	cmp	r9d, 2
	jbe	.L15
	vmovups	xmm1, XMMWORD PTR [rsi+rax*4]
	vmulps	xmm1, xmm1, XMMWORD PTR [rdi+rax*4]
	mov	eax, ecx
	and	eax, -4
	and	ecx, 3
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	je	.L11
	add	edx, eax
.L15:
	lea	ecx, 1[rdx]
	mov	eax, edx
	cmp	r8d, ecx
	vmovss	xmm4, DWORD PTR [rdi+rax*4]
	vfmadd231ss	xmm0, xmm4, DWORD PTR [rsi+rax*4]
	jle	.L11
	add	edx, 2
	vmovss	xmm5, DWORD PTR 4[rsi+rax*4]
	vfmadd231ss	xmm0, xmm5, DWORD PTR 4[rdi+rax*4]
	cmp	r8d, edx
	jle	.L11
	vmovss	xmm6, DWORD PTR 8[rdi+rax*4]
	vfmadd231ss	xmm0, xmm6, DWORD PTR 8[rsi+rax*4]
	ret
	.p2align 4,,10
	.p2align 3
.L19:
	vzeroupper
.L11:
	ret
	.p2align 4,,10
	.p2align 3
.L16:
	vxorps	xmm0, xmm0, xmm0
	ret
.L17:
	xor	eax, eax
	xor	edx, edx
	vxorps	xmm0, xmm0, xmm0
	jmp	.L13
	.cfi_endproc
.LFE1:
	.size	"_Z20dot_product_restrictPKfS0_i", .-"_Z20dot_product_restrictPKfS0_i"
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
