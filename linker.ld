SECTIONS
{
    . = 0x0;
    _app_start = .; 

    .text : {
        KEEP(*(.text.entry))
        *(.text .text.*)
    }

    .rodata : { *(.rodata .rodata.*) }

    .data : {
        *(.data .data.*)
        *(.got .got.*)
        *(.data.rel.ro*) 
    }
    
    _app_end = .;
}