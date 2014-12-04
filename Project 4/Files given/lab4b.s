
	.text

main:	li $5, 7
	li $f0, 0
	li $f1, 10
	li $f3, 100
	li $f4, 1000
	

loop:	subi $5, $5, 1
	fadd $f0, $f0, $f1
	fadd $f0, $f0, $f3
	fadd $f0, $f0, $f4
	bge  $5, $0, loop
	nop
	nop

	li $2, 10
	syscall
	