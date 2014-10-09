## Daniel J. Ellard -- 02/21/94
## palindrome.asm -- read a line of text and test if it is a palindrome.
##
## A palindrome is a string that is exactly the same when read backwards
## as when it is read forward (e.g. anna).
##
## Whitespace is considered significant in this version, so "ahahaha"
## is a palindrome while "aha haha" is not.
##
## Register usage:
## $9 - A - initially, the address of the first character in the string S.
## $10 - B - initially, the address of the the last character in S.
## $11 - the character at address A.
## $12 - the character at address B.
## $2 - syscall parameter / return values.
## $4 - syscall parameters.
## $5 - syscall parameters.

	.text
main:
	
## read the string S:
	la $4, string_space
	li $5, 1024
	li $2, 8		# load "read_string" code into $2.
	syscall

	la $9, string_space	# A = S.
	la $10, string_space	# we need to move B to the end

length_loop:			# length of the string
	lb $11, ($10)		# load the byte at addr B into $11.
	nop
	beqz $11, end_length_loop # if $11 == 0, branch out of loop.
	nop
	
	addi $10, $10, 1	# otherwise, increment B,
	b length_loop		# and repeat the loop.
	nop
	
end_length_loop:
	subi $10, $10, 2	# subtract 2 to move B back past

# the '\0' and '\n'.

test_loop:
	bge $9, $10, is_palin	# if A >= B, it's a palindrome.
	nop
	
	lb $11, ($9)		# load the byte at addr A into $11,
	nop
	lb $12, ($10)		# load the byte at addr B into $12.
	nop
	bne $11, $12, not_palin # if $11 != $12, not a palindrome.
	nop
	
# Otherwise,

	addi $9, $9, 1	# increment A,
	subi $10, $10, 1	# decrement B,
	b test_loop		# and repeat the loop.
	nop
	
is_palin:			# print the is_palin_msg, and exit.

	la $4, is_palin_msg
	li $2, 4
	syscall
	b exit
	nop
	
not_palin:
	la $4, not_palin_msg	## print the not_palin_msg, and exit.
	li $2, 4
	syscall
	b exit
	nop
	
exit:				# exit the program
	li $2, 10		# load "exit" code into $2.
	syscall			# make the system call.


	.data
is_palin_msg: .asciiz "The string is a palindrome.\n"
not_palin_msg: .asciiz "The string is not a palindrome.\n"
string_space: .space 1024	# reserve 1024 bytes for the string.
