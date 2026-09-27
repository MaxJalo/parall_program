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
	mov	r8, rsi
	mov	r9d, edx
	jle	.L7
	lea	eax, -1[rdx]
	vpxor	xmm0, xmm0, xmm0
	cmp	eax, 30
	jbe	.L8
	mov	ecx, edx
	mov	rdx, rsi
	vmovaps	ymm3, ymm0
	mov	rax, rdi
	shr	ecx, 5
	vmovaps	ymm1, ymm0
	vmovaps	ymm2, ymm0
	mov	esi, ecx
	sal	rsi, 7
	add	rsi, rdi
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L4:
	vmovups	ymm4, YMMWORD PTR [rax]
	vmovups	ymm5, YMMWORD PTR 32[rax]
	sub	rax, -128
	sub	rdx, -128
	vmovups	ymm6, YMMWORD PTR -64[rax]
	vmovups	ymm7, YMMWORD PTR -32[rax]
	vfmadd231ps	ymm0, ymm4, YMMWORD PTR -128[rdx]
	vfmadd231ps	ymm2, ymm5, YMMWORD PTR -96[rdx]
	vfmadd231ps	ymm1, ymm6, YMMWORD PTR -64[rdx]
	vfmadd231ps	ymm3, ymm7, YMMWORD PTR -32[rdx]
	cmp	rax, rsi
	jne	.L4
	vaddps	ymm0, ymm0, ymm2
	vaddps	ymm1, ymm1, ymm3
	mov	eax, ecx
	sal	eax, 5
	cmp	r9d, eax
	vaddps	ymm1, ymm1, ymm0
	vextractf128	xmm0, ymm1, 0x1
	vaddps	xmm0, xmm0, xmm1
	vmovhlps	xmm2, xmm0, xmm0
	vaddps	xmm2, xmm2, xmm0
	vshufps	xmm1, xmm2, xmm2, 85
	vaddps	xmm1, xmm1, xmm2
	je	.L28
	mov	ecx, eax
	vzeroupper
.L3:
	mov	r10d, r9d
	sub	r10d, eax
	lea	edx, -1[r10]
	cmp	edx, 2
	jbe	.L5
	mov	edx, r10d
	sal	rax, 2
	shr	edx, 2
	lea	rsi, [rdi+rax]
	add	rax, r8
	cmp	edx, 1
	vmovups	xmm3, XMMWORD PTR [rax]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR [rsi]
	je	.L6
	cmp	edx, 2
	vmovups	xmm3, XMMWORD PTR 16[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 16[rax]
	je	.L6
	cmp	edx, 3
	vmovups	xmm2, XMMWORD PTR 32[rsi]
	vfmadd231ps	xmm0, xmm2, XMMWORD PTR 32[rax]
	je	.L6
	cmp	edx, 4
	vmovups	xmm3, XMMWORD PTR 48[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 48[rax]
	je	.L6
	cmp	edx, 5
	vmovups	xmm3, XMMWORD PTR 64[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 64[rax]
	je	.L6
	cmp	edx, 6
	vmovups	xmm4, XMMWORD PTR 80[rsi]
	vfmadd231ps	xmm0, xmm4, XMMWORD PTR 80[rax]
	je	.L6
	vmovups	xmm5, XMMWORD PTR 96[rsi]
	vfmadd231ps	xmm0, xmm5, XMMWORD PTR 96[rax]
.L6:
	vmovhlps	xmm1, xmm0, xmm0
	vaddps	xmm0, xmm1, xmm0
	sal	edx, 2
	cmp	r10d, edx
	vshufps	xmm1, xmm0, xmm0, 85
	vaddps	xmm1, xmm1, xmm0
	je	.L1
	add	ecx, edx
.L5:
	lea	edx, 1[rcx]
	mov	eax, ecx
	cmp	r9d, edx
	vmovss	xmm3, DWORD PTR [rdi+rax*4]
	vfmadd231ss	xmm1, xmm3, DWORD PTR [r8+rax*4]
	jle	.L1
	add	ecx, 2
	vmovss	xmm2, DWORD PTR 4[r8+rax*4]
	vfmadd231ss	xmm1, xmm2, DWORD PTR 4[rdi+rax*4]
	cmp	r9d, ecx
	jle	.L1
	vmovss	xmm6, DWORD PTR 8[rdi+rax*4]
	vfmadd231ss	xmm1, xmm6, DWORD PTR 8[r8+rax*4]
.L1:
	vmovaps	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L7:
	vxorps	xmm1, xmm1, xmm1
	vmovaps	xmm0, xmm1
	ret
.L8:
	xor	eax, eax
	xor	ecx, ecx
	vxorps	xmm1, xmm1, xmm1
	jmp	.L3
.L28:
	vzeroupper
	jmp	.L1
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
	mov	r8, rsi
	mov	r9d, edx
	jle	.L36
	lea	eax, -1[rdx]
	vpxor	xmm0, xmm0, xmm0
	cmp	eax, 30
	jbe	.L37
	mov	ecx, edx
	mov	rdx, rsi
	vmovaps	ymm3, ymm0
	mov	rax, rdi
	shr	ecx, 5
	vmovaps	ymm1, ymm0
	vmovaps	ymm2, ymm0
	mov	esi, ecx
	sal	rsi, 7
	add	rsi, rdi
	.p2align 6
	.p2align 4,,10
	.p2align 3
.L33:
	vmovups	ymm4, YMMWORD PTR [rax]
	vmovups	ymm5, YMMWORD PTR 32[rax]
	sub	rax, -128
	sub	rdx, -128
	vmovups	ymm6, YMMWORD PTR -64[rax]
	vmovups	ymm7, YMMWORD PTR -32[rax]
	vfmadd231ps	ymm0, ymm4, YMMWORD PTR -128[rdx]
	vfmadd231ps	ymm2, ymm5, YMMWORD PTR -96[rdx]
	vfmadd231ps	ymm1, ymm6, YMMWORD PTR -64[rdx]
	vfmadd231ps	ymm3, ymm7, YMMWORD PTR -32[rdx]
	cmp	rax, rsi
	jne	.L33
	vaddps	ymm0, ymm0, ymm2
	vaddps	ymm1, ymm1, ymm3
	mov	eax, ecx
	sal	eax, 5
	cmp	r9d, eax
	vaddps	ymm1, ymm1, ymm0
	vextractf128	xmm0, ymm1, 0x1
	vaddps	xmm0, xmm0, xmm1
	vmovhlps	xmm2, xmm0, xmm0
	vaddps	xmm2, xmm2, xmm0
	vshufps	xmm1, xmm2, xmm2, 85
	vaddps	xmm1, xmm1, xmm2
	je	.L57
	mov	ecx, eax
	vzeroupper
.L32:
	mov	r10d, r9d
	sub	r10d, eax
	lea	edx, -1[r10]
	cmp	edx, 2
	jbe	.L34
	mov	edx, r10d
	sal	rax, 2
	shr	edx, 2
	lea	rsi, [rdi+rax]
	add	rax, r8
	cmp	edx, 1
	vmovups	xmm3, XMMWORD PTR [rax]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR [rsi]
	je	.L35
	cmp	edx, 2
	vmovups	xmm3, XMMWORD PTR 16[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 16[rax]
	je	.L35
	cmp	edx, 3
	vmovups	xmm2, XMMWORD PTR 32[rsi]
	vfmadd231ps	xmm0, xmm2, XMMWORD PTR 32[rax]
	je	.L35
	cmp	edx, 4
	vmovups	xmm3, XMMWORD PTR 48[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 48[rax]
	je	.L35
	cmp	edx, 5
	vmovups	xmm3, XMMWORD PTR 64[rsi]
	vfmadd231ps	xmm0, xmm3, XMMWORD PTR 64[rax]
	je	.L35
	cmp	edx, 6
	vmovups	xmm4, XMMWORD PTR 80[rsi]
	vfmadd231ps	xmm0, xmm4, XMMWORD PTR 80[rax]
	je	.L35
	vmovups	xmm5, XMMWORD PTR 96[rsi]
	vfmadd231ps	xmm0, xmm5, XMMWORD PTR 96[rax]
.L35:
	vmovhlps	xmm1, xmm0, xmm0
	vaddps	xmm0, xmm1, xmm0
	sal	edx, 2
	cmp	r10d, edx
	vshufps	xmm1, xmm0, xmm0, 85
	vaddps	xmm1, xmm1, xmm0
	je	.L30
	add	ecx, edx
.L34:
	lea	edx, 1[rcx]
	mov	eax, ecx
	cmp	r9d, edx
	vmovss	xmm3, DWORD PTR [rdi+rax*4]
	vfmadd231ss	xmm1, xmm3, DWORD PTR [r8+rax*4]
	jle	.L30
	add	ecx, 2
	vmovss	xmm2, DWORD PTR 4[r8+rax*4]
	vfmadd231ss	xmm1, xmm2, DWORD PTR 4[rdi+rax*4]
	cmp	r9d, ecx
	jle	.L30
	vmovss	xmm6, DWORD PTR 8[rdi+rax*4]
	vfmadd231ss	xmm1, xmm6, DWORD PTR 8[r8+rax*4]
.L30:
	vmovaps	xmm0, xmm1
	ret
	.p2align 4,,10
	.p2align 3
.L36:
	vxorps	xmm1, xmm1, xmm1
	vmovaps	xmm0, xmm1
	ret
.L37:
	xor	eax, eax
	xor	ecx, ecx
	vxorps	xmm1, xmm1, xmm1
	jmp	.L32
.L57:
	vzeroupper
	jmp	.L30
	.cfi_endproc
.LFE1:
	.size	"_Z20dot_product_restrictPKfS0_i", .-"_Z20dot_product_restrictPKfS0_i"
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
