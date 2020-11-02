ADD $t0,$zero,$zero
LW $at, 0x00($zero)
LW $v0, 0x04($zero)
LW $v1, 0x08($zero)
ADD $v1,$v0,$at
OR $a0,$v0,$at
SUB $a1,$v0,$at
AND $a2,$v0,$at

BEQ $a3,$t0, 0x02
LUI $t1, 0x2000
ORI $t1,$t1, 0x14
SW $t0, 0x00($t1)

LW  $t2, 0x0($t1)
JR  $at
