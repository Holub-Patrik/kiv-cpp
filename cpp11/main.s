	.text
	.intel_syntax noprefix
	.file	"main.cpp"
                                        # Start of file scope inline assembly
	.globl	_ZSt21ios_base_library_initv

                                        # End of file scope inline assembly
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function _Z5powerdm
.LCPI0_0:
	.long	1127219200                      # 0x43300000
	.long	1160773632                      # 0x45300000
	.long	0                               # 0x0
	.long	0                               # 0x0
.LCPI0_1:
	.quad	0x4330000000000000              # double 4503599627370496
	.quad	0x4530000000000000              # double 1.9342813113834067E+25
	.text
	.globl	_Z5powerdm
	.p2align	4, 0x90
	.type	_Z5powerdm,@function
_Z5powerdm:                             # @_Z5powerdm
	.cfi_startproc
# %bb.0:
	vmovq	xmm1, rdi
	vpunpckldq	xmm1, xmm1, xmmword ptr [rip + .LCPI0_0] # xmm1 = xmm1[0],mem[0],xmm1[1],mem[1]
	vsubpd	xmm1, xmm1, xmmword ptr [rip + .LCPI0_1]
	vshufpd	xmm2, xmm1, xmm1, 1             # xmm2 = xmm1[1,0]
	vaddsd	xmm1, xmm2, xmm1
	jmp	pow@PLT                         # TAILCALL
.Lfunc_end0:
	.size	_Z5powerdm, .Lfunc_end0-_Z5powerdm
	.cfi_endproc
                                        # -- End function
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4, 0x0                          # -- Begin function main
.LCPI1_0:
	.long	1127219200                      # 0x43300000
	.long	1160773632                      # 0x45300000
	.long	0                               # 0x0
	.long	0                               # 0x0
.LCPI1_1:
	.quad	0x4330000000000000              # double 4503599627370496
	.quad	0x4530000000000000              # double 1.9342813113834067E+25
	.text
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	push	rbp
	.cfi_def_cfa_offset 16
	push	r15
	.cfi_def_cfa_offset 24
	push	r14
	.cfi_def_cfa_offset 32
	push	r13
	.cfi_def_cfa_offset 40
	push	r12
	.cfi_def_cfa_offset 48
	push	rbx
	.cfi_def_cfa_offset 56
	sub	rsp, 24
	.cfi_def_cfa_offset 80
	.cfi_offset rbx, -56
	.cfi_offset r12, -48
	.cfi_offset r13, -40
	.cfi_offset r14, -32
	.cfi_offset r15, -24
	.cfi_offset rbp, -16
	mov	r14, qword ptr [rip + _ZSt3cin@GOTPCREL]
	lea	rbx, [rip + .L.str]
	lea	r15, [rsp + 8]
	lea	r12, [rip + .L.str.1]
	lea	r13, [rsp + 16]
	lea	rbp, [rip + .L.str.2]
	jmp	.LBB1_4
	.p2align	4, 0x90
.LBB1_1:                                #   in Loop: Header=BB1_4 Depth=1
	vmulsd	xmm1, xmm0, xmm0
	vmulsd	xmm1, xmm0, xmm1
.LBB1_2:                                #   in Loop: Header=BB1_4 Depth=1
	vmulsd	xmm0, xmm0, xmm1
.LBB1_3:                                #   in Loop: Header=BB1_4 Depth=1
	mov	al, 1
	mov	rdi, rbp
	call	printf@PLT
.LBB1_4:                                # =>This Inner Loop Header: Depth=1
	mov	rdi, rbx
	xor	eax, eax
	call	printf@PLT
	mov	rdi, r14
	mov	rsi, r15
	call	_ZNSi10_M_extractIdEERSiRT_@PLT
	mov	rdi, r12
	xor	eax, eax
	call	printf@PLT
	mov	rdi, r14
	mov	rsi, r13
	call	_ZNSi10_M_extractImEERSiRT_@PLT
	vmovsd	xmm0, qword ptr [rsp + 8]       # xmm0 = mem[0],zero
	mov	rax, qword ptr [rsp + 16]
	cmp	rax, 3
	je	.LBB1_1
# %bb.5:                                #   in Loop: Header=BB1_4 Depth=1
	cmp	rax, 2
	jne	.LBB1_7
# %bb.6:                                #   in Loop: Header=BB1_4 Depth=1
	vmulsd	xmm1, xmm0, xmm0
	jmp	.LBB1_2
	.p2align	4, 0x90
.LBB1_7:                                #   in Loop: Header=BB1_4 Depth=1
	vmovq	xmm1, rax
	vpunpckldq	xmm1, xmm1, xmmword ptr [rip + .LCPI1_0] # xmm1 = xmm1[0],mem[0],xmm1[1],mem[1]
	vsubpd	xmm1, xmm1, xmmword ptr [rip + .LCPI1_1]
	vshufpd	xmm2, xmm1, xmm1, 1             # xmm2 = xmm1[1,0]
	vaddsd	xmm1, xmm2, xmm1
	call	pow@PLT
	jmp	.LBB1_3
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Zadejde cislo: "
	.size	.L.str, 16

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"Zadejte exponent: "
	.size	.L.str.1, 19

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"%f"
	.size	.L.str.2, 3

	.section	".linker-options","e",@llvm_linker_options
	.ident	"clang version 18.1.8"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym _ZSt3cin
