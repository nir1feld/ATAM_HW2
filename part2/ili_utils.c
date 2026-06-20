#include <asm/desc.h>

// TO_DO: store the address of the IDT at the parameter given
void my_store_idt(struct desc_ptr *idtr) {
// <STUDENT FILL>
    asm volatile("sidt %0" : "=m"(*idtr));
// </STUDENT FILL>
}////

// TO_DO: load the address of the IDT from the parameter given
void my_load_idt(struct desc_ptr *idtr) {
// <STUDENT FILL>
    asm volatile("lidt %0" : : "m"(*idtr));
// <STUDENT FILL>
}

// TO_DO: set the address in the right place at the parameter gate
// try to remember - how this information is stored?
void my_set_gate_offset(gate_desc *gate, unsigned long addr) {
// <STUDENT FILL>
    //addr -> 32byte high | 16 byte middle| 16 low
    //extract parts using bit shift with masks for how many bytes to get after each shift
    gate->offset_low = (addr & 0xFFFF);
    gate->offset_middle = ((addr >> 16) & 0xFFFF);
    gate->offset_high = ((addr >> 32) & 0xFFFFFFFF);
// </STUDENT FILL>
}

// TO_DO: return val is the address stored at the parameter gate_desc gate
// try to remember - how this information is stored?
unsigned long my_get_gate_offset(gate_desc *gate) {
// <STUDENT FILL>
    unsigned long val = 0;

    // undo the bit shifts from the struct
    val = gate->offset_low;
    val |= ((unsigned long)gate->offset_middle << 16);
    val |= ((unsigned long)gate->offset_high << 32);

    return val;
// </STUDENT FILL>
}
