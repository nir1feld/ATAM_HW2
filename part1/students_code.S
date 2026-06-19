.global sum_of_text


.extern get_path
.extern text_to_number

sum_of_text:
    // done :)
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %rbp
    movq %rsp, %rbp

    movb %dil, %r13b // sep stored in %r13b

    // open file syscall
    call get_path   // puts path in rax.
    movq %rax, %rdi // path in -arg1
    movl $2, %rax   //open syscall id
    movq $0, %rsi   //read only -arg2
    movl $0, %rdx   //mode -arg3
    syscall
    movq %rax, %r12 //store FD in r12

    xor %r14, %r14   //r14 will be the curr_sum variable
    movq $1, %rbx // will be used as the EOF flag, counts as set if less/eq to 0

    movq %rsp, %rbp // this funcitons as the defcato str start. MAYBE BAD!!!

    // read syscall (one char at a time)
    read_char:
    dec %rsp         //make space in the stack for the char
    movl $0, %rax    // read syscall
    movq %r12, %rdi  // set FD -arg1
    movq %rsp, %rsi  // char storage destination -arg2
    movl $1, %rdx    // read size - 1 byte - arg3
    syscall          // char will be at %eax

    cmpq $0, %rax //if we read 0 bytes file ended, if negative err
    jle handle_eof

    // load char to %al:
    movb (%rsi), %al

    cmpb $13, %al // cmp with  "\n"
    je handle_word_end

    cmpb $10, %al // cmp with "\r"
    je handle_word_end

    cmpb %r13b, %al 
    je handle_word_end

    jmp read_char


    handle_eof:
    movq %rax, %rbx

    handle_word_end:
    movb $0, (%rsp) //replace seperator with null terminator
    
    //call converter func
    movq %rsp, %rdi
    call text_to_number

    // reset stack
    movq %rbp, %rsp

    cmpw $-1, %ax
    je check_eof_flag

    // else its a number so:
    movswq %ax, %rax // cast to 64bit num
    addq %rax, %r14 // add the number to the sum
    
    check_eof_flag:
    cmpq $0, %rbx //check if EOF flag is up (its up if 0 or less)
    jle end

    jmp read_char 

end:
xorq %rax, %rax
movw %r14w, %ax
popq %rbp
popq %r14
popq %r13
popq %r12
popq %rbx
ret

