.global sum_of_text


.extern get_path
.extern text_to_number

sum_of_text:
    //todo push all registers to stack here (convention)
    movb %dil, %r11 //store given separator

    subq $1, %rsp //make space in the stack for the char value
    call get_path
    movl $2, %rax //open syscall
    movq %rax, %rdi // path in -arg1
    movq $0, %rsi //read only -arg2
    movl $0, %rdx//mode -arg3
    syscall

    movq %eax, %r12 //store FD
    movl $0, %r14 //r14 will be the counter

    loop:
    movl $0, %rax //read syscall
    movq %r12, %rdi // set FD -arg1
    movq %rsp, %rsi //read destination -arg2
    movl %1, %rdx //read size - 1 byte - arg3
    syscall //char will be at %eax

    cmpb $0, %rax //if end of file reached
    je return_sum

    movb (%rsp), %cl
    movl %r12, %r10

    find_separator_with_null:
    movb (%r10), %cl //%cl - next char
    cmpb %r11b, %cl //separator
    je find_word_return_separator

    cmpb $0x0A, %cl //end of line
    je find_word

    incq %r10 //go to next char
    jmp find_separator_with_null


    find_word_return_separator:
    movb $0, (%r10) //overwrite separator
    movl %r12, %rdi //pass char*
    call text_to_number
    cmpb $-1, %eax
    je loop
    addl %eax, %r14
    movb %r11b, (%r10) //return separator
    movl %r10, %r12
    incb %r12
    jmp loop


    find_word_end_line:
    movl %r12, %rdi //pass char*
    call text_to_number
    cmpb $-1, %eax
    je loop
    addl %eax, %r14
    movl %r10, %r12
    incb %r12
    jmp loop

    return_sum:
    movl %r14, %eax
  ret
