ENTRY(_start);

SECTIONS
{
    _start = 0x100000000;
    . = 0x100000000;
    .data : {
        *(.data)
    }
}
