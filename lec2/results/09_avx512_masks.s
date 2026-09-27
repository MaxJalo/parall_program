	.file	"09_avx512_masks.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	_Z17masked_add_avx512PKfS0_PfPKbm
	.type	_Z17masked_add_avx512PKfS0_PfPKbm, @function
_Z17masked_add_avx512PKfS0_PfPKbm:
.LFB6454:
	.cfi_startproc
	endbr64
	cmp	r8, 15
	jbe	.L10
	push	rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	mov	rax, rcx
	xor	r11d, r11d
	mov	r10d, 1
	mov	rbp, rsp
	.cfi_def_cfa_register 6
	push	rbx
	.cfi_offset 3, -24
	mov	rbx, r8
	mov	r8, rdx
	.p2align 4,,10
	.p2align 3
.L5:
	vmovups	zmm1, ZMMWORD PTR [rdi+r11*4]
	vmovups	zmm0, ZMMWORD PTR [rsi+r11*4]
	xor	ecx, ecx
	xor	edx, edx
	.p2align 4,,10
	.p2align 3
.L4:
	cmp	BYTE PTR [rax+rcx], 0
	je	.L3
	mov	r9d, r10d
	sal	r9d, cl
	or	edx, r9d
.L3:
	add	rcx, 1
	cmp	rcx, 16
	jne	.L4
	vaddps	zmm0, zmm0, zmm1
	kmovw	k1, edx
	lea	rdx, 16[r11]
	add	r11, 32
	add	rax, 16
	vmovups	ZMMWORD PTR [r8]{k1}, zmm0
	add	r8, 64
	cmp	rbx, r11
	jb	.L14
	mov	r11, rdx
	jmp	.L5
.L14:
	vzeroupper
	mov	rbx, QWORD PTR -8[rbp]
	leave
	.cfi_def_cfa 7, 8
	ret
.L10:
	.cfi_restore 3
	.cfi_restore 6
	ret
	.cfi_endproc
.LFE6454:
	.size	_Z17masked_add_avx512PKfS0_PfPKbm, .-_Z17masked_add_avx512PKfS0_PfPKbm
	.p2align 4
	.globl	_Z12clamp_avx512Pfffm
	.type	_Z12clamp_avx512Pfffm, @function
_Z12clamp_avx512Pfffm:
.LFB6455:
	.cfi_startproc
	endbr64
	vbroadcastss	zmm0, xmm0
	vbroadcastss	zmm1, xmm1
	cmp	rsi, 15
	jbe	.L19
	mov	eax, 16
	.p2align 4,,10
	.p2align 3
.L17:
	vmovups	zmm3, ZMMWORD PTR -64[rdi+rax*4]
	vmaxps	zmm2, zmm3, zmm0
	vminps	zmm2, zmm2, zmm1
	vmovups	ZMMWORD PTR -64[rdi+rax*4], zmm2
	add	rax, 16
	cmp	rsi, rax
	jnb	.L17
.L19:
	vzeroupper
	ret
	.cfi_endproc
.LFE6455:
	.size	_Z12clamp_avx512Pfffm, .-_Z12clamp_avx512Pfffm
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
