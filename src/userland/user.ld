ENTRY(_start)

PHDRS
{
    text PT_LOAD FLAGS(5);
    data PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0x0000000000400000;

    .text : ALIGN(4K)
    {
        *(.text*)
    } :text

    . = ALIGN(4K);

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    } :text

    . = ALIGN(4K);

    .data : ALIGN(4K)
    {
        *(.data*)
    } :data

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    } :data
}
