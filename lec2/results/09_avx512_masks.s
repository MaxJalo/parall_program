	.file	"09_avx512_masks.cpp"
	.intel_syntax noprefix
	.text
	.p2align 4
	.globl	"_Z17masked_add_avx512PKfS0_PfPKbm"
	.type	"_Z17masked_add_avx512PKfS0_PfPKbm", @function
"_Z17masked_add_avx512PKfS0_PfPKbm":
.LFB7345:
	.cfi_startproc
	mov	r9, rcx
	cmp	r8, 15
	jbe	.L5
	vmovdqa	xmm5, XMMWORD PTR .LC0[rip]
	mov	eax, 16
	vmovdqa	xmm4, XMMWORD PTR .LC1[rip]
	vpxor	xmm3, xmm3, xmm3
	.p2align 4
	.p2align 3
.L3:
	vpcmpeqb	xmm1, xmm3, XMMWORD PTR -16[r9+rax]
	vmovups	zmm2, ZMMWORD PTR -64[rsi+rax*4]
	vaddps	zmm2, zmm2, ZMMWORD PTR -64[rdi+rax*4]
	add	rax, 16
	vpternlogd	zmm1, zmm1, zmm1, 0x55
	vpmovsxbw	xmm0, xmm1
	vpsrldq	xmm1, xmm1, 8
	vpmovsxbw	xmm1, xmm1
	vpand	xmm1, xmm1, xmm4
	vpternlogd	zmm0, zmm5, zmm1, 234
	vpsrldq	xmm1, xmm0, 8
	vpor	xmm0, xmm0, xmm1
	vpsrldq	xmm1, xmm0, 4
	vpor	xmm0, xmm0, xmm1
	vpsrldq	xmm1, xmm0, 2
	vpor	xmm0, xmm0, xmm1
	vpextrw	ecx, xmm0, 0
	kmovw	k1, ecx
	vmovups	ZMMWORD PTR [rdx]{k1}, zmm2
	add	rdx, 64
	cmp	r8, rax
	jnb	.L3
	vzeroupper
.L5:
	ret
	.cfi_endproc
.LFE7345:
	.size	"_Z17masked_add_avx512PKfS0_PfPKbm", .-"_Z17masked_add_avx512PKfS0_PfPKbm"
	.p2align 4
	.globl	"_Z12clamp_avx512Pfffm"
	.type	"_Z12clamp_avx512Pfffm", @function
"_Z12clamp_avx512Pfffm":
.LFB7346:
	.cfi_startproc
	cmp	rsi, 15
	jbe	.L11
	shr	rsi, 4
	vbroadcastss	zmm0, xmm0
	vbroadcastss	zmm1, xmm1
	sal	rsi, 6
	lea	rax, [rsi+rdi]
	.p2align 5
	.p2align 4
	.p2align 3
.L9:
	vmovups	zmm2, ZMMWORD PTR [rdi]
	add	rdi, 64
	vmaxps	zmm2, zmm2, zmm0
	vminps	zmm2, zmm2, zmm1
	vmovups	ZMMWORD PTR -64[rdi], zmm2
	cmp	rdi, rax
	jne	.L9
	vzeroupper
.L11:
	ret
	.cfi_endproc
.LFE7346:
	.size	"_Z12clamp_avx512Pfffm", .-"_Z12clamp_avx512Pfffm"
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC0:
	.value	1
	.value	2
	.value	4
	.value	8
	.value	16
	.value	32
	.value	64
	.value	128
	.align 16
.LC1:
	.value	256
	.value	512
	.value	1024
	.value	2048
	.value	4096
	.value	8192
	.value	16384
	.value	-32768
	.ident	"GCC: (GNU) 16.2.1 20260810"
	.section	.note.GNU-stack,"",@progbits
