# This MAL program will find the average value of an
# integer array of size 12 (elements)

            .data
intarr:     .word     1, 2048, 4, 512, 16, 64, 32, 128, 8, 512, 1024, 2
length:     .word     12
average:    .double   0.0
outstr:     .asciiz   "The average value in the array is: "

            .text
            .globl main
main:       move  $t0, $0        # $t0 is a loop index
            lw    $s1, length    # $s1 holds max # elements
            la    $s0, intarr    # $s0 holds the start addr of arr
            move  $t1, $0        # $t1 holds the sum of values from array

            # Sum the elements in the array
loop:       beq   $t0, $s1, compute # When $t0 = $s1, we hit end of array
            # TODO
            lw $t2, 0($s0) # load word
            add $t1, $t1, $t2 # add to sum
            addi $s0, $s0, 4 # get next word
            addi $t0, $t0, 1 # increment i (for sentinel value)
            b     loop

            # Compute the average using the FP coprocessor
            #    and store the result in average
compute:
            # TODO
            lw $t3, length # load length
            mtc1.d $t3, $f4 # convert to double
            cvt.d.w $f2, $f4
            mtc1 $t1, $f1 # convert to double
            cvt.d.w $f0, $f1
            div.d $f0, $f0, $f2 # divide
            sdc1 $f0, average # store in average
            mov.d $f12, $f0 # move to $f12 for printing

            # Print out average value
            la      $a0, outstr     # Output string
            li      $v0, 4
            syscall

            li $v0, 3
            syscall

            li      $v0, 10         # To the bat mobile
            syscall
