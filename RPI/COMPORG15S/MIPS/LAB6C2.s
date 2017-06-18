# This program prints out the following:
# hello2 x: 3
# hello x: 2
# main x: 1
# The program is intended to match a certain C
# program which calls functions hello and hello2
# to print out a variable passed in each time.
# This is accomplished in MIPS via stack pointers.
# Saving stack pointers should be done incrementally
# upwards (0, 4, 8, etc), moving the stack pointer
# a negative distance from where your saved registers
# will be.
# Loading stack pointers involves loading 0($sp)
# and moving the stack pointer downward by
# adding 4 each load call.
# Need to load 4($sp) for subsequent calls because
# the stack pointer is moved.

.data
    mnstr: .asciiz "main x: "
    newl: .asciiz "\n"
    histr: .asciiz "hello x: "
    histr2: .asciiz "hello2 x: "
.text
# function main
.globl main
main:
    addi $s0, $0, 1         # x = 1
    jal hello               # call hello
    lw $s0, 4($sp)          # load x=1
    addi $sp, $sp, 4        # return sp to original position
    la $a0, mnstr           # print mnstr
    li $v0, 4
    syscall
    move $a0, $s0           # print x
    li $v0, 1
    syscall
    la $a0, newl            # print newline
    li $v0, 4
    syscall
    li $v0, 10              # exit
    syscall
# function hello
hello:
    addi $sp, $sp, -12      # move the stack pointer up 12 (bytes)
    sw $31, 0($sp)          # save $ra ($31) on the stack
    sw $s0, 4($sp)          # save x=1 on the stack
    addi $s0, $s0, 1        # x++
    sw $s0, 8($sp)          # save x=2 on the stack
    jal hello2              # call hello2
    lw $31, 0($sp)          # pop $ra ($31) from the stack
    addi $sp, $sp, 4        # move the stack pointer down 4
    lw $s0, 4($sp)          # load x=2
    addi $sp $sp, 4         # move down 4 again
    la $a0, histr           # print histr
    li $v0, 4
    syscall
    move $a0, $s0           # print x
    li $v0, 1
    syscall
    la $a0, newl            # print newline
    li $v0, 4
    syscall
    jr $31                  # return to caller (main)
# function hello2
hello2:
    addi $s0, $s0, 1        # x++ (x=3)
    la $a0, histr2          # print histr2
    li $v0, 4
    syscall
    move $a0, $s0           # print x
    li $v0, 1
    syscall
    la $a0, newl            # print newline
    li $v0, 4
    syscall
    jr $31                  # return to caller (hello)
