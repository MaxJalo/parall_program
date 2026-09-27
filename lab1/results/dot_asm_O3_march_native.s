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
	test	edx, edx
	mov	rcx, rsi
	jle	.L9
	lea	eax, -1[rdx]
	cmp	eax, 6
	jbe	.L10
	mov	esi, edx
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
	shr	esi, 3
	sal	rsi, 5
	.p2align 4,,10
	.p2align 3
.L4:
	vmovups	ymm4, YMMWORD PTR [rdi+rax]
	vmulps	ymm1, ymm4, YMMWORD PTR [rcx+rax]
	add	rax, 32
	cmp	rsi, rax
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
	mov	eax, edx
	and	eax, -8
	cmp	edx, eax
	mov	esi, eax
	je	.L17
	vzeroupper
.L3:
	mov	r8d, edx
	sub	r8d, esi
	lea	r9d, -1[r8]
	cmp	r9d, 2
	jbe	.L7
	vmovups	xmm5, XMMWORD PTR [rdi+rsi*4]
	vmulps	xmm1, xmm5, XMMWORD PTR [rcx+rsi*4]
	mov	esi, r8d
	and	esi, -4
	add	eax, esi
	and	r8d, 3
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	je	.L1
.L7:
	movsx	r8, eax
	vmovss	xmm6, DWORD PTR [rdi+r8*4]
	lea	rsi, 0[0+r8*4]
	vfmadd231ss	xmm0, xmm6, DWORD PTR [rcx+r8*4]
	lea	r8d, 1[rax]
	cmp	edx, r8d
	jle	.L1
	add	eax, 2
	vmovss	xmm7, DWORD PTR 4[rdi+rsi]
	vfmadd231ss	xmm0, xmm7, DWORD PTR 4[rcx+rsi]
	cmp	edx, eax
	jle	.L1
	vmovss	xmm7, DWORD PTR 8[rdi+rsi]
	vfmadd231ss	xmm0, xmm7, DWORD PTR 8[rcx+rsi]
	ret
	.p2align 4,,10
	.p2align 3
.L9:
	vxorps	xmm0, xmm0, xmm0
.L1:
	ret
	.p2align 4,,10
	.p2align 3
.L17:
	vzeroupper
	ret
.L10:
	xor	esi, esi
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
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
	test	edx, edx
	mov	rcx, rsi
	jle	.L26
	lea	eax, -1[rdx]
	cmp	eax, 6
	jbe	.L27
	mov	esi, edx
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
	shr	esi, 3
	sal	rsi, 5
	.p2align 4,,10
	.p2align 3
.L21:
	vmovups	ymm4, YMMWORD PTR [rdi+rax]
	vmulps	ymm1, ymm4, YMMWORD PTR [rcx+rax]
	add	rax, 32
	cmp	rsi, rax
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
	jne	.L21
	mov	eax, edx
	and	eax, -8
	cmp	edx, eax
	mov	esi, eax
	je	.L33
	vzeroupper
.L20:
	mov	r8d, edx
	sub	r8d, esi
	lea	r9d, -1[r8]
	cmp	r9d, 2
	jbe	.L24
	vmovups	xmm5, XMMWORD PTR [rdi+rsi*4]
	vmulps	xmm1, xmm5, XMMWORD PTR [rcx+rsi*4]
	mov	esi, r8d
	and	esi, -4
	add	eax, esi
	and	r8d, 3
	vaddss	xmm0, xmm0, xmm1
	vshufps	xmm2, xmm1, xmm1, 85
	vaddss	xmm0, xmm0, xmm2
	vunpckhps	xmm2, xmm1, xmm1
	vshufps	xmm1, xmm1, xmm1, 255
	vaddss	xmm0, xmm0, xmm2
	vaddss	xmm0, xmm0, xmm1
	je	.L18
.L24:
	movsx	r8, eax
	vmovss	xmm6, DWORD PTR [rdi+r8*4]
	lea	rsi, 0[0+r8*4]
	vfmadd231ss	xmm0, xmm6, DWORD PTR [rcx+r8*4]
	lea	r8d, 1[rax]
	cmp	edx, r8d
	jle	.L18
	add	eax, 2
	vmovss	xmm7, DWORD PTR 4[rdi+rsi]
	vfmadd231ss	xmm0, xmm7, DWORD PTR 4[rcx+rsi]
	cmp	edx, eax
	jle	.L18
	vmovss	xmm7, DWORD PTR 8[rdi+rsi]
	vfmadd231ss	xmm0, xmm7, DWORD PTR 8[rcx+rsi]
	ret
	.p2align 4,,10
	.p2align 3
.L26:
	vxorps	xmm0, xmm0, xmm0
.L18:
	ret
	.p2align 4,,10
	.p2align 3
.L33:
	vzeroupper
	ret
.L27:
	xor	esi, esi
	xor	eax, eax
	vxorps	xmm0, xmm0, xmm0
	jmp	.L20
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
