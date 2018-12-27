ENTRY(_start);

SECTIONS
{
    _start = 0x80110000;
    . = 0x80110000;
    .data : {
        *(.data)
    }
}
