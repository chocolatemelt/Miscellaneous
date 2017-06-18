# hw3_2.s
# Kevin Zhang and Tina Guo
# Calculates number of whitespace, nonwhitespace, and words.
# Searches the input string for the search string and if it
# exists, it prints out the relevant text.
# Note:
# Since the search text is entered as {text}\n, when searching
# this naturally would search for {text}\n instead of just
# {text}. Therefore, instead of checking for the null
# terminator, I check for \n.

# text and variables
.data
    mnstr1: .asciiz "Enter a input line: "
    mnstr2: .asciiz "Enter a pattern to search for: "
    res0:   .asciiz "Results:\n"
    res1:   .asciiz "# of whitespace characters: "
    res2:   .asciiz "# of non-whitespace characters: "
    res3:   .asciiz "# of words: "
    cho1:   .asciiz "The user pattern was found within the input line\n"
    cho2:   .asciiz "The user pattern was NOT found within the input line\n"
    err:    .asciiz "Line contains whitespace characters only!\n"
    endl:   .asciiz "\n"
    strvar: .space 100  # string variable
    serstr: .space 100  # search string variable
.text
# function main
.globl main
main:
    # initialize some registers 
    addi $s0, $0, 0     # string to be analyzed
    addi $s1, $0, 0     # whitespace characters
    addi $s2, $0, 0     # non-whitespace characters
    addi $s3, $0, 0     # words
    addi $s4, $0, 0     # string to be searched
    la $a0, mnstr1      # print mnstr1
    li $v0, 4
    syscall
    li $v0, 8           # take in input
    la $a0, strvar      # load "strvar" byte space
    syscall
    move $s0, $a0       # save to s0
    la $a0, mnstr2      # print mnstr2
    li $v0, 4
    syscall
    li $v0, 8           # take in input
    la $a0, serstr      # load "serstr" byte space
    syscall
    move $s4, $a0       # save to $s4
    jal count           # call count
    beq $s2, 0, error   # error: all whitespace
    la $a0, res1        # print first set of results
    li $v0, 4
    syscall
    move $a0, $s1       # print # of whitespace
    li $v0, 1
    syscall
    la $a0, endl        # print newline
    li $v0, 4
    syscall
    la $a0, res2        # print second set of results
    li $v0, 4
    syscall
    move $a0, $s2       # print # of non-whitespace
    li $v0, 1
    syscall
    la $a0, endl        # print newline
    li $v0, 4
    syscall
    la $a0, res3        # print third set of results
    li $v0, 4
    syscall
    move $a0, $s3       # print # of words
    li $v0, 1
    syscall
    la $a0, endl        # print newline
    li $v0, 4
    syscall
    jal searchstring    # call searchstring
    li $v0, 10          # quit
    syscall
# error, quit program
error:
    la $a0, err         # print err
    li $v0, 4
    syscall
    li $v0, 10          # quit
    syscall
# function count
#   assigns to $s1 the number of whitespace
#   assigns to $s2 the number of nonwhitespace
#   assigns to $s3 the number of words
count:
    addi $t0, $0, 0     # [i]
    addi $t4, $0, 0     # temporary boolean denoting a word
# loop through $s0 (input string)
loop:
    add $t1, $s0, $t0   # point to strvar[i]
    lb $t2, 0($t1)      # load strvar[i]'s character
    beq $t2, 0, exit    # check for the null operator (end of input)
    # check for equality here - if white then restart
    beq $t2, ' ', white
    beq $t2, '\t', white
    beq $t2, '\n', white
    # if we reached this point, it must have been non-whitespace
    # check if we're in a new word or not; if so, increment word
    # and toggle inword, if not then go straight to normal nonwhite
    # incrementing
    beq $t4, 0, inword  # if(inword)
    j nonwhite
# case white
white:
    addi $s1, $s1, 1    # increment $s1
    addi $t4, $0, 0     # if we were in a word, we aren't anymore
    j end               # jump to end (of loop iteration)
# case inword
inword:
    addi $t4, $0, 1     # set to true now
    addi $s3, $s3, 1    # increment word
# case nonwhite
nonwhite:
    addi $s2, $s2, 1    # increment $s2
# case end
end:
    addi $t0, $t0, 1    # i++; point to next character
    j loop              # iterate again
# exit to main for jal calls
exit:
    jr $31              # return to main
# function searchstring
#   checks if the string is in the word
#   and outputs the appropriate result text
searchstring:
    addi $t0, $0, 0     # i
    addi $t3, $0, 0     # j
sloop:
    add $t1, $s0, $t0   # point to strvar[i]
    lb $t2, 0($t1)      # load strvar[i]
    add $t4, $s4, $t3   # point to serstr[j]
    lb $t5, 0($t4)      # load serstr[j]
    beq $t5, '\n', instr# reached end of search string, i.e. in string
    beq $t2, 0, ninstr  # reached null of input string, i.e. not in string
    bne $t2, $t5, reset # reset if not same character
    # same character, keep checking
    addi $t0, $t0, 1    # i++
    addi $t3, $t3, 1    # j++
    j sloop             # iterate again
# case not same character, reset the pointer of serstr
reset:
    addi $t0, $t0, 1    # i++
    addi $t3, $0, 1     # j=0
    j sloop
# case in string
instr:
    la $a0, cho1        # print cho1
    li $v0, 4
    syscall
    j exit
# case not in string
ninstr:
    la $a0, cho2        # print cho2
    li $v0, 4
    syscall
    j exit
