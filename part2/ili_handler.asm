.globl my_ili_handler
.extern what_to_do, old_ili_handler

.text
.align 4, 0x90
my_ili_handler:
  ####### Some smart student's code here #######
    ## save all registers
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rdi
    pushq %rsi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    ## get problematic command address (120 = 15 registers * 8 bytes)
    movq 120(%rsp), %r10
    movb (%r10), %al ## store command first byte in al

    xor %rdi, %rdi ## set rdi to 0

    cmpb $0x0F, %al
    je handle_long_opcode

    ## one byte opcode
    movb %al, %dil ## load first byte to dil
    incq %r10 ## the return address will advance one line after the original command
    jmp call_what_to_do

handle_long_opcode:
    movb 1(%r10), %dil ## load second byte to dil
    addq $2, %r10  ## the return address will advance two lines after the original command

call_what_to_do:
    call what_to_do

    cmpq $0, %rax
    je return_to_default_behavior

    ## advance to the command after problematic command
    movq %r10, 120(%rsp) ## override original rip to the next command
    movq %rax, 80(%rsp) ## override original rdi to the result of what to do

    ## return original values (reverse order)
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rsi
    popq %rdi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    iretq

return_to_default_behavior:
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rsi
    popq %rdi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax
    jmp *old_ili_handler(%rip)