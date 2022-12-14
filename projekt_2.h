//  NOTES
/*
    Processor bus - big endian
    VMEbus - big endian
    PCI/X bus - little endian
    Register -little endian
*/



#define     RESET_VALUE_32      0x00000000
    /* ^ 
    Przypisanie tej wartości do zmiennej 32-bitowej powoduje ustawienie 
    wszystkich jej bitów na 0.
    */
#define     RESET_VALUE_16      0x0000
    /* ^ 
    Przypisanie tej wartości do zmiennej 16-bitowej powoduje ustawienie 
    wszystkich jej bitów na 0.
    */


#define     WRONG_INDEX         -2
    /* ^ 
    Informacja zwracana w przypadku podania indeksu spoza dozwolonego zakresu.
    */
#define     CORRECT             1
    /* ^ 
    Informacja zwracana w przypadku poprawnego wykonania danej funkcji.
    */


//  INDEX OF MASTER CANAL
#define     MASTER_0        10
#define     MASTER_1        11
#define     MASTER_2        12
#define     MASTER_3        13
#define     MASTER_4        14
#define     MASTER_5        15
#define     MASTER_6        16
#define     MASTER_7        17
    /* ^ 
    Dostępnych jest 8 kanałów typu Master.
    W wywołaniu funkcji zawsze musimy podać do którego się odnosimy.
    */


//  INDEX OF SLAVE CANAL
#define     SLAVE_0        20
#define     SLAVE_1        21
#define     SLAVE_2        22
#define     SLAVE_3        23
#define     SLAVE_4        24
#define     SLAVE_5        25
#define     SLAVE_6        26
#define     SLAVE_7        27
    /* ^ 
    Dostępnych jest 8 kanałów typu Slave.
    W wywołaniu funkcji zawsze musimy podać do którego się odnosimy.
    */


// OUTBOUND TRANSLATION ATTRIBUTES
    /* 
    Aby otrzymać 32-bitową daną wpisywaną do rejestru atrybutów translacji
    musimy wykonać operację sumy logicznej poniżej zdefiniowanych atrybutów.
    Na przykład chcąc ustawić bit EN, szybkość transmicji na 160 MB/s oraz tryb
    adresowania na A24 musimy wykonać następującą operację:
        EN_SET | TR_RATE_160 | ADMODE_A24 
    Następnie wynik powyższej sumy logicznej przekazać do funkcji ustawiającej
    rejestr atrybutów translacji.
    */
#define     EN_SET      0b1 0000000000000000000000000000000
#define     EN_CLR      0b0 0000000000000000000000000000000  
    /* ^ 
    EN (Enable) 
    If set, the corresponding outbound translation function is enabled. 
    */
#define     MRPFD_SET   0b0000000000000 1 000000000000000000
#define     MRPFD_CLR   0b0000000000000 0 000000000000000000
    /* ^ 
    MRPFD (Memory Read Prefetch Disable) 
    If set, prefetching is disabled for all memory read commands.
    If cleared, a cache line is prefetched when a PCI/X bus memory
    read burst is received. 
    */ 
#define     PFS_2       0b00000000000000 00 0000000000000000
#define     PFS_4       0b00000000000000 01 0000000000000000
#define     PFS_8       0b00000000000000 10 0000000000000000
#define     PFS_16      0b00000000000000 11 0000000000000000  
    /* ^ 
    PFS (Prefetch Size)
    This field sets the data read prefetch size for PCI/X bus read 
    multiple commands.
    PFS_2 -> 2 Cache Lines
    PFS_4 -> 4 Cache Lines
    PFS_8 -> 8 Cache Lines
    PFS_16 -> 16 Cache Lines
    */  
#define     TR_RATE_160     0b000000000000000000 000 00000000000 
#define     TR_RATE_267     0b000000000000000000 001 00000000000
#define     TR_RATE_320     0b000000000000000000 010 00000000000   
    /* ^ 
    2eSSTM (2eSST Mode)
    This field defines the 2eSST Transfer Rate
    TR_RATE_160 -> 160 MB/s
    TR_RATE_267 -> 267 MB/s
    TR_RATE_320 -> 320 MB/s
    */
#define     TR_MODE_SCT         0b000000000000000000000 000 00000000
#define     TR_MODE_BLT         0b000000000000000000000 001 00000000
#define     TR_MODE_MBLT        0b000000000000000000000 010 00000000
#define     TR_MODE_2eVME       0b000000000000000000000 011 00000000
#define     TR_MODE_2eSST       0b000000000000000000000 100 00000000
#define     TR_MODE_2eSST_BR    0b000000000000000000000 101 00000000
    /* ^ 
    TM (Transfer Mode)
    The field defines the VMEbus transfer mode
    TR_MODE_SCT -> SCT
    TR_MODE_BLT -> BLT
    TR_MODE_MBLT -> MBLT
    TR_MODE_2eVME -> 2eVME
    TR_MODE_2eSST -> 2eSST
    TR_MODE_2eSST_BR -> 2eSST Broadcast
    */
#define     DBW_16      0b000000000000000000000000 00 000000
#define     DBW_32      0b000000000000000000000000 01 000000
    /* ^ 
    DBW (VMEbus Data Bus Width)
    These bits define the maximum data bus width for VMEbus transfer
    initialized by the correspodning outbound translation function.
    These bits apply to SCT and BLT transfers. MBLT, 2eVME and 2eSST
    transfers are always 64-bit.
    DBW_16 -> 16 bit
    DBW_32 -> 32 bit
    */
#define     SUP_SET     0b00000000000000000000000000 1 00000
#define     SUP_CLR     0b00000000000000000000000000 0 00000
    /* ^ 
    SUP (VMEbus Supervisory Mode)
    When this bit is set the AM code indicates Supervisory Access, when 
    required. When this bit is cleared
    */
#define     PGM_SET     0b000000000000000000000000000 1 0000
#define     PGM_CLR     0b000000000000000000000000000 0 0000
    /* ^ 
    PGM (VMEbus Program Mode)
    When this bit is set the AM code indicates Program Access. When this
    bit is cleared the AM code indicates Data Access.
    */
#define     ADMODE_A16      0b0000000000000000000000000000 0000
#define     ADMODE_A24      0b0000000000000000000000000000 0001
#define     ADMODE_A32      0b0000000000000000000000000000 0010
#define     ADMODE_A64      0b0000000000000000000000000000 0100
#define     ADMODE_CRCSR    0b0000000000000000000000000000 0101
#define     ADMODE_USER1    0b0000000000000000000000000000 1000
#define     ADMODE_USER2    0b0000000000000000000000000000 1001
#define     ADMODE_USER3    0b0000000000000000000000000000 1010
#define     ADMODE_USER4    0b0000000000000000000000000000 1011
    /* ^ 
    AMODE (Address Mode)
    This field defines the VMEbus Address mode.
    When the User1-User4 modes are used, the AM[1] bit is defined by the SUP bit
    and the AM[0] bit is defined by the PGM bit.
    Description as in the designation.
    */


// VME MASTER CONTROL REGISTER FLAGS
    /* 
    Aby otrzymać 32-bitową daną wpisywaną do rejestru kontrolnego kanału Master
    musimy wykonać operację sumy logicznej poniżej zdefiniowanych flag.
    Na przykład chcąc ustawić bit EN, szybkość transmicji na 160 MB/s oraz tryb
    adresowania na A24 musimy wykonać następującą operację:
        EN_SET | TR_RATE_160 | ADMODE_A24 
    Następnie wynik powyższej sumy logicznej przekazać do funkcji ustawiającej
    rejestr atrybutów translacji.
    */
#define     VS_SET      0b00000 1 00000000000000000000000000
#define     VS_CLR      0b00000 0 00000000000000000000000000


#define     DWB_SET     0b0000000 1 000000000000000000000000
#define     DWB_CLR     0b0000000 0 000000000000000000000000


#define     RMWEN_SET       0b00000000000 1 00000000000000000000
#define     RMWEN_CLR       0b00000000000 0 00000000000000000000


#define     A64DS_SET       0b000000000000000 1 0000000000000000
#define     A64DS_CLR       0b000000000000000 0 0000000000000000


#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000
#define     VTOFF       0b00000000000000000 000 000000000000


#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000
#define     VTON        0b000000000000000000000 000 00000000


#define     VREL        0b000000000000000000000000000 00 000
#define     VREL        0b000000000000000000000000000 00 000
#define     VREL        0b000000000000000000000000000 00 000
#define     VREL        0b000000000000000000000000000 00 000


#define     VFAIR        0b00000000000000000000000000000 0 00


#define     VREQL_00    0b000000000000000000000000000000 00
#define     VREQL_01    0b000000000000000000000000000000 01
#define     VREQL_10    0b000000000000000000000000000000 10
#define     VREQL_11    0b000000000000000000000000000000 11


