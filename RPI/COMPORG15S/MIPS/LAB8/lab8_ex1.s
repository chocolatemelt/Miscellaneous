# Program to calculate the surface area
# and volume of a sphere given the radius

# Data Declarations
         .data
pi:      .float 3.14159
four:    .float 4.0
three:   .float 3.0
prompt:  .asciiz "Enter an input radius: "
saprmpt: .asciiz "The surface area of the sphere is: "
vlprmpt: .asciiz "The volume of the sphere is: "
newline: .asciiz "\n"

# text/code section
         .text
         .globl main

main:
# Compute: (4.0 * pi) which is used for both equations
        lwc1 $f1, four
        lwc1 $f2, pi
        lwc1 $f7, three
        mul.s $f3, $f1, $f2

# Prompt the user for an input radius
        la $a0, prompt # print prompt
        li $v0, 4
        syscall
        li $v0, 6
        syscall

# Calculate surface area of a sphere
#   surface_area = 4.0 * pi * radius^2
       mul.s $f5, $f0, $f0
       mul.s $f12, $f3, $f5

# Print surface area to user
        la $a0, saprmpt
        li $v0, 4
        syscall
        li $v0, 2
        syscall
        la $a0, newline
        li $v0, 4
        syscall

# Calculate volume of a sphere:
#   volume = (4.0 * pi / 3.0) * radius^3
        mul.s $f4, $f5, $f0
        div.s $f6, $f3, $f7
        mul.s $f12, $f6, $f4

# Print surface area to user
        la $a0, vlprmpt
        li $v0, 4
        syscall
        li $v0, 2
        syscall
        la $a0, newline
        li $v0, 4
        syscall

# Exit program
         li      $v0, 10         # To the bat mobile
         syscall

