ENTRY(_start)

PHDRS
{
    text   PT_LOAD FLAGS(5);
    rodata PT_LOAD FLAGS(4);
    data   PT_LOAD FLAGS(6);
}

SECTIONS
{
    . = 0xffffffff80000000;

    .text : ALIGN(4K)
    {
        *(.text*)
    } :text

    . = ALIGN(4K);

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
        *(.limine_requests*)
    } :rodata

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
