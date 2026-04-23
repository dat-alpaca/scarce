SECTIONS
{
    . = 0x0;

    .text : {
        KEEP(*(.text.entry))
        *(.text.*)

        *(.rodata .rodata.*)
        *(.data.rel.ro* .text.rel.ro*)
        *(.got .got.*)
        *(.data .data.*)
    }

    /DISCARD/ : {
        *(.note*)
        *(.comment)
        *(.eh_frame)
    }
}