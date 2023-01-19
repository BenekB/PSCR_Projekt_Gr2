#include "projekt_2.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "tsi148.h"
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/neutrino.h>

/* Michal Penkal
 * Benedykt Bela
 *
 */

// kończymy tym, że generujemy wirtualny wskaźnik do mastera
// wygenerować wirtualny wskaźnik do mastera podobnie jak pierwsza grupa
// korzystać z memory map device


/*
    Skróty i odniesienia wykorzystywane w kodzie:
        PM ->   Tsi148 PCI/X-to-VME Bus Programming Manual
*/








uint32_t change_endians32(uint32_t in)
{
    uint32_t b0, b1, b2, b3;
    uint32_t res;

    b0 = (in & 0x000000ff) << 24u;
    b1 = (in & 0x0000ff00) << 8u;
    b2 = (in & 0x00ff0000) >> 8u;
    b3 = (in & 0xff000000) >> 24u;

    res = b0 | b1 | b2 | b3;   

    return res; 
}


uint16_t change_endians16(uint16_t in)
{
    uint16_t b0, b1;
    uint16_t res;

    b0 = (in & 0x00ff) << 8u;
    b1 = (in & 0xff00) >> 8u;

    res = b0 | b1;   

    return res; 
}


/*
    Funkcja ustawia adres początkowy określonegoo obszaru pamięci na magistrali PCI/X, który jest wykorzystywany
    w celu uzyskania dostępu do zasobów magistrali VMEbus. (PM s. 184-185)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]
    upper_address ->    32 bardziej znaczące bity adresu
    lower_address ->    16 mniej znaczących bitów adresu

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_outbound_translation_starting_address(crg_t *ptr, uint8_t index, uint32_t upper_address, uint32_t lower_address)
{
    upper_address = change_endians32(upper_address);
    uint32_t help = 0x0;
    help = help | lower_address;
    help = change_endians32(lower_address);

    switch (index)
    {
        case MASTER_0:
            ptr->LCSR.OTSAU0 = upper_address; 
                // ^ Rejestr przechowujący bardziej znaczącą część adresu.          
            ptr->LCSR.OTSAL0 = help;
                /*
                Rejestr przechowujący mniej znaczącą część adresu.
                Mniej znacząca część adresu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_address przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case MASTER_1:
            ptr->LCSR.OTSAU1 = upper_address;
            ptr->LCSR.OTSAL1 = lower_address << 16;
            break;
        case MASTER_2:
            ptr->LCSR.OTSAU2 = upper_address;
            ptr->LCSR.OTSAL2 = lower_address << 16;
            break;
        case MASTER_3:
            ptr->LCSR.OTSAU3 = upper_address;
            ptr->LCSR.OTSAL3 = lower_address << 16;
            break;
        case MASTER_4:
            ptr->LCSR.OTSAU4 = upper_address;
            ptr->LCSR.OTSAL4 = lower_address << 16;
            break;
        case MASTER_5:
            ptr->LCSR.OTSAU5 = upper_address;
            ptr->LCSR.OTSAL5 = lower_address << 16;
            break;
        case MASTER_6:
            ptr->LCSR.OTSAU6 = upper_address;
            ptr->LCSR.OTSAL6 = lower_address << 16;
            break;
        case MASTER_7:
            ptr->LCSR.OTSAU7 = upper_address;
            ptr->LCSR.OTSAL7 = lower_address << 16;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia adres początkowy określonego obszaru pamięci związanego z mapowaniem przestrzeni magistrali
    VMEbus na magistralę PCI/X. (PM s.229-230)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7]
    upper_address ->    32 bardziej znaczące bity adresu.
    lower_address ->    32 mniej znaczące bity adresu. W zależności od szerokości magistrali VMEbus wykorzystywane są 
                        różne zakresy bitów spośród 32-bitowej danej. Szczegóły w definicji funkcji oraz w PM na stronie 230.

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_inbound_translation_starting_address(crg_t* ptr, uint8_t index, uint32_t upper_address, uint32_t lower_address)
{
    // upper_address = change_endians32(upper_address);
    // lower_address = change_endians32(lower_address);

    switch (index)
    {
        case SLAVE_0:
            ptr->LCSR.ITSAU0 = upper_address; 
                // ^ Rejestr przechowujący bardziej znaczącą część adresu.          
            ptr->LCSR.ITSAL0 = lower_address;     
                /*
                Rejestr przechowujący mniej znaczącą część adresu.

                Jeżeli magistrala adresowa VMEbus jest 64-bitowa lub 32-bitowa
                to mniej znaczące bity adresu początkowego od 31 do 16 są porównywane
                do bitów od 31 do 16 adresu magistrali VMEbus.

                Jeżeli magistrala adresowa VMEbus jest 24-bitowa, to mniej
                znaczące bity adresu początkowego od 23 do 12 są porównywane
                do bitów od 23 do 12 adresu magistrali VMEbus.

                Jeżeli magistrala adresowa VMEbus jest 16-bitowa, to mniej
                znaczące bity adresu początkowego od 15 do 4 są porównywane
                do bitów od 15 do 4 adresu magistrali VMEbus.

                Szczegóły w PM s. 230.
                */
            break;
        case SLAVE_1:
            ptr->LCSR.ITSAU1 = upper_address;
            ptr->LCSR.ITSAL1 = lower_address;
            break;
        case SLAVE_2:
            ptr->LCSR.ITSAU2 = upper_address;
            ptr->LCSR.ITSAL2 = lower_address;
            break;
        case SLAVE_3:
            ptr->LCSR.ITSAU3 = upper_address;
            ptr->LCSR.ITSAL3 = lower_address;
            break;
        case SLAVE_4:
            ptr->LCSR.ITSAU4 = upper_address;
            ptr->LCSR.ITSAL4 = lower_address;
            break;
        case SLAVE_5:
            ptr->LCSR.ITSAU5 = upper_address;
            ptr->LCSR.ITSAL5 = lower_address;
            break;
        case SLAVE_6:
            ptr->LCSR.ITSAU6 = upper_address;
            ptr->LCSR.ITSAL6 = lower_address;
            break;
        case SLAVE_7:
            ptr->LCSR.ITSAU7 = upper_address;
            ptr->LCSR.ITSAL7 = lower_address;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia adres końcowy określonegoo obszaru pamięci na magistrali PCI/X, który jest wykorzystywany
    w celu uzyskania dostępu do zasobów magistrali VMEbus. (PM s. 186-187)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]
    upper_address ->    32 bardziej znaczące bity adresu
    lower_address ->    16 mniej znaczących bitów adresu

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_outbound_translation_ending_address(crg_t* ptr, uint8_t index, uint32_t upper_address, uint32_t lower_address)
{
	upper_address = change_endians32(upper_address);
	uint32_t help = 0x0;
	help = help | lower_address;
	help = change_endians32(lower_address);

    switch (index)
    {
        case MASTER_0:
            ptr->LCSR.OTEAU0 = upper_address;
                // ^ Rejestr przechowujący bardziej znaczącą część adresu. 
            ptr->LCSR.OTEAL0 = help;
                /*
                Rejestr przechowujący mniej znaczącą część adresu.
                Mniej znacząca część adresu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_address przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case MASTER_1:
            ptr->LCSR.OTEAU1 = upper_address;
            ptr->LCSR.OTEAL1 = lower_address << 16;
            break;
        case MASTER_2:
            ptr->LCSR.OTEAU2 = upper_address;
            ptr->LCSR.OTEAL2 = lower_address << 16;
            break;
        case MASTER_3:
            ptr->LCSR.OTEAU3 = upper_address;
            ptr->LCSR.OTEAL3 = lower_address << 16;
            break;
        case MASTER_4:
            ptr->LCSR.OTEAU4 = upper_address;
            ptr->LCSR.OTEAL4 = lower_address << 16;
            break;
        case MASTER_5:
            ptr->LCSR.OTEAU5 = upper_address;
            ptr->LCSR.OTEAL5 = lower_address << 16;
            break;
        case MASTER_6:
            ptr->LCSR.OTEAU6 = upper_address;
            ptr->LCSR.OTEAL6 = lower_address << 16;
            break;
        case MASTER_7:
            ptr->LCSR.OTEAU7 = upper_address;
            ptr->LCSR.OTEAL7 = lower_address << 16;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia adres końcowy określonego obszaru pamięci związanego z mapowaniem przestrzeni magistrali
    VMEbus na magistralę PCI/X. (PM s. 231-232)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7]
    upper_address ->    32 bardziej znaczące bity adresu.
    lower_address ->    32 mniej znaczące bity adresu. W zależności od szerokości magistrali VMEbus wykorzystywane są 
                        różne zakresy bitów spośród 32-bitowej danej. Szczegóły w definicji funkcji oraz w PM na stronie 232.

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_inbound_translation_ending_address(crg_t* ptr, uint8_t index, uint32_t upper_address, uint32_t lower_address)
{
    // upper_address = change_endians32(upper_address);
    // lower_address = change_endians32(lower_address);

    switch (index)
    {
        case SLAVE_0:
            ptr->LCSR.ITEAU0 = upper_address; 
                // ^ Rejestr przechowujący bardziej znaczącą część adresu.          
            ptr->LCSR.ITEAL0 = lower_address;     
                /*
                Rejestr przechowujący mniej znaczącą część adresu.

                Jeżeli magistrala adresowa VMEbus jest 64-bitowa lub 32-bitowa
                to mniej znaczące bity adresu końcowego od 31 do 16 są porównywane
                do bitów od 31 do 16 adresu magistrali VMEbus.

                Jeżeli magistrala adresowa VMEbus jest 24-bitowa, to mniej
                znaczące bity adresu końcowego od 23 do 12 są porównywane
                do bitów od 23 do 12 adresu magistrali VMEbus.

                Jeżeli magistrala adresowa VMEbus jest 16-bitowa, to mniej
                znaczące bity adresu końcowego od 15 do 4 są porównywane
                do bitów od 15 do 4 adresu magistrali VMEbus.

                Szczegóły w PM s. 232.
                */
            break;
        case SLAVE_1:
            ptr->LCSR.ITEAU1 = upper_address;
            ptr->LCSR.ITEAL1 = lower_address;
            break;
        case SLAVE_2:
            ptr->LCSR.ITEAU2 = upper_address;
            ptr->LCSR.ITEAL2 = lower_address;
            break;
        case SLAVE_3:
            ptr->LCSR.ITEAU3 = upper_address;
            ptr->LCSR.ITEAL3 = lower_address;
            break;
        case SLAVE_4:
            ptr->LCSR.ITEAU4 = upper_address;
            ptr->LCSR.ITEAL4 = lower_address;
            break;
        case SLAVE_5:
            ptr->LCSR.ITEAU5 = upper_address;
            ptr->LCSR.ITEAL5 = lower_address;
            break;
        case SLAVE_6:
            ptr->LCSR.ITEAU6 = upper_address;
            ptr->LCSR.ITEAL6 = lower_address;
            break;
        case SLAVE_7:
            ptr->LCSR.ITEAU7 = upper_address;
            ptr->LCSR.ITEAL7 = lower_address;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia offset translacji adresu z magistrali PCI/X na magistralę VMEbus.
    Ustawiony offset zostanie dodany do linii adresu PCI/X w zakresie od bitu A64 do bitu A16
    w celu utworzenia odpowiadającego adresu na magistrali VMEbus (PM s. 188-189)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]
    upper_offset ->    32 bardziej znaczące bity offsetu
    lower_offset ->    16 mniej znaczących bitów offsetu

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_outbound_translation_offset(crg_t* ptr, uint8_t index, uint32_t upper_offset, uint32_t lower_offset)
{

	upper_offset = change_endians32(upper_offset);
	uint32_t help = 0x0;
	help = help | lower_offset;
	help = change_endians32(lower_offset);

    switch (index)
    {
        case MASTER_0:
            ptr->LCSR.OTOFU0 = upper_offset;
                // ^ Rejestr przechowujący bardziej znaczącą część offsetu.
            ptr->LCSR.OTOFL0 = help;
                /*
                Rejestr przechowujący mniej znaczącą część offsetu.
                Mniej znacząca część offsetu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_offset przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case MASTER_1:
            ptr->LCSR.OTOFU1 = upper_offset;
            ptr->LCSR.OTOFL1 = lower_offset << 16;
            break;
        case MASTER_2:
            ptr->LCSR.OTOFU2 = upper_offset;
            ptr->LCSR.OTOFL2 = lower_offset << 16;
            break;
        case MASTER_3:
            ptr->LCSR.OTOFU3 = upper_offset;
            ptr->LCSR.OTOFL3 = lower_offset << 16;
            break;
        case MASTER_4:
            ptr->LCSR.OTOFU4 = upper_offset;
            ptr->LCSR.OTOFL4 = lower_offset << 16;
            break;
        case MASTER_5:
            ptr->LCSR.OTOFU5 = upper_offset;
            ptr->LCSR.OTOFL5 = lower_offset << 16;
            break;
        case MASTER_6:
            ptr->LCSR.OTOFU6 = upper_offset;
            ptr->LCSR.OTOFL6 = lower_offset << 16;
            break;
        case MASTER_7:
            ptr->LCSR.OTOFU7 = upper_offset;
            ptr->LCSR.OTOFL7 = lower_offset << 16;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia offset translacji adresu związany z mapowaniem przestrzeni adresowej magistrali
    VMEbus na przestrzeń adresową magistrali PCI/X. (PM s. 233-234)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7]
    upper_offset ->    32 bardziej znaczące bity offsetu
    lower_offset ->    32 mniej znaczące bity offsetu. W zależności od szerokości magistrali VMEbus wykorzystywane są 
                       różne zakresy bitów spośród 32-bitowej danej. Szczegóły w definicji funkcji oraz w PM na stronie 234.

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_inbound_translation_offset(crg_t* ptr, uint8_t index, uint32_t upper_offset, uint32_t lower_offset)
{
    // upper_offset = change_endians32(upper_offset);
    // lower_offset = change_endians32(lower_offset);

    switch (index)
    {
        case SLAVE_0:
            ptr->LCSR.ITOFU0 = upper_offset;
                // ^ Rejestr przechowujący bardziej znaczącą część offsetu.          
            ptr->LCSR.ITOFL0 = lower_offset;
                /*
                Rejestr przechowujący mniej znaczącą część offsetu. Jest ona dodawana
                do mniej znaczącej części adresu magistrali VMEbus w celu wyznaczenia 
                odpowiadający adres na magistrali PCI/X.

                Jeżeli magistrala adresowa VMEbus jest 24-bitowa, to bity wewnętrzne adresu
                magistrali VMEbus od 31 do 24 są zerowane a następnie bity offsetu od 31 do 12
                są dodawane.

                Jeżeli magistrala adresowa VMEbus jest 16-bitowa, to bity wewnętrzne adresu
                magistrali VMEbus od 31 do 16 są zerowane a następnie bity offsetu od 31 do 4
                są dodawane.

                Szczegóły w PM s. 234.
                */
            break;
        case SLAVE_1:
            ptr->LCSR.ITOFU1 = upper_offset;
            ptr->LCSR.ITOFL1 = lower_offset;
            break;
        case SLAVE_2:
            ptr->LCSR.ITOFU2 = upper_offset;
            ptr->LCSR.ITOFL2 = lower_offset;
            break;
        case SLAVE_3:
            ptr->LCSR.ITOFU3 = upper_offset;
            ptr->LCSR.ITOFL3 = lower_offset;
            break;
        case SLAVE_4:
            ptr->LCSR.ITOFU4 = upper_offset;
            ptr->LCSR.ITOFL4 = lower_offset;
            break;
        case SLAVE_5:
            ptr->LCSR.ITOFU5 = upper_offset;
            ptr->LCSR.ITOFL5 = lower_offset;
            break;
        case SLAVE_6:
            ptr->LCSR.ITOFU6 = upper_offset;
            ptr->LCSR.ITOFL6 = lower_offset;
            break;
        case SLAVE_7:
            ptr->LCSR.ITOFU7 = upper_offset;
            ptr->LCSR.ITOFL7 = lower_offset;
            break;
        default:
            return WRONG_INDEX;
    }


    return CORRECT;
}



/*
    Funkcja ustawia "Outbound Translation 2eSST Broadcast Select Registers" (PM s. 190)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]
    broadcast_select_2eSST ->   32 bity wpisywane do odpowiedniego rejestru OTBS w celu jego ustawienia

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_outbound_translation_2eSST_broadcast_select(crg_t* ptr, uint8_t index, uint32_t broadcast_select_2eSST)
{
    // broadcast_select_2eSST = change_endians32(broadcast_select_2eSST);

    switch (index)
    {
        case MASTER_0:  ptr->LCSR.OTBS0 = broadcast_select_2eSST;   break;
        case MASTER_1:  ptr->LCSR.OTBS1 = broadcast_select_2eSST;   break;
        case MASTER_2:  ptr->LCSR.OTBS2 = broadcast_select_2eSST;   break;
        case MASTER_3:  ptr->LCSR.OTBS3 = broadcast_select_2eSST;   break;
        case MASTER_4:  ptr->LCSR.OTBS4 = broadcast_select_2eSST;   break;
        case MASTER_5:  ptr->LCSR.OTBS5 = broadcast_select_2eSST;   break;
        case MASTER_6:  ptr->LCSR.OTBS6 = broadcast_select_2eSST;   break;
        case MASTER_7:  ptr->LCSR.OTBS7 = broadcast_select_2eSST;   break;
        default:    return WRONG_INDEX;
    }

    return CORRECT;
}




/*
    Funkcja ustawia atrybuty translacji (PM s. 191-194)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]
    attribute ->    32 bity wpisywane do odpowiedniego rejestru atrybutów w celu jego ustawienia. Możemy uzyskać pożądany ciąg 
                    binarny poprzez wygenerowanie sumy logicznej zdefiniowanych stałych odpowiadających konkretnym atrybutom.
                    Na przykład chcąc ustawić bit EN, szybkość transmicji na 160 MB/s oraz tryb adresowania na A24 musimy 
                    w miejsce argumentu attribute podać attribute = (EN_SET | TR_RATE_160 | ADMODE_A24)

    Dostępne stałe generujące argument attribute (w kolejnych liniach znajdują się flagi ustawiające jedną wartość, możemy wykorzystać
    tylko jedną flagę z danej listy):
        [EN_SET, EN_CLR]
        [MRPFD_SET, MRPFD_CLR]
        [PFS_2, PFS_4, PFS_8, PFS_16]
        [TR_RATE_160, TR_RATE_267, TR_RATE_320]
        [TR_MODE_SCT, TR_MODE_BLT, TR_MODE_MBLT, TR_MODE_2eVME, TR_MODE_2eSST, TR_MODE_2eSST_BR]
        [DBW_16, DBW_32]
        [SUP_SET, SUP_CLR]
        [PGM_SET, PGM_CLR]
        [ADMODE_A16, ADMODE_A24, ADMODE_A64, ADMODE_CRCSR, ADMODE_USER1, ADMODE_USER2, ADMODE_USER3, ADMODE_USER4]

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_outbound_translation_attribute(crg_t* ptr, uint8_t index, uint32_t attribute)
{
    attribute = change_endians32(attribute);

    switch (index)
    {
        case MASTER_0:  ptr->LCSR.OTAT0 = attribute;    break;
        case MASTER_1:  ptr->LCSR.OTAT1 = attribute;    break;
        case MASTER_2:  ptr->LCSR.OTAT2 = attribute;    break;
        case MASTER_3:  ptr->LCSR.OTAT3 = attribute;    break;
        case MASTER_4:  ptr->LCSR.OTAT4 = attribute;    break;
        case MASTER_5:  ptr->LCSR.OTAT5 = attribute;    break;
        case MASTER_6:  ptr->LCSR.OTAT6 = attribute;    break;
        case MASTER_7:  ptr->LCSR.OTAT7 = attribute;    break;
        default:    return WRONG_INDEX;
    }

    return CORRECT;
}



/*
    Funkcja ustawia rejestry związane z mapowaniem przestrzeni magistrali VMEbus
    na przestrzeń magistrali PCI/X (PM s. 235-238)

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7]
    attribute ->    32 bity wpisywane do odpowiedniego rejestru atrybutów w celu jego ustawienia. Możemy uzyskać pożądany ciąg 
                    binarny poprzez wygenerowanie sumy logicznej zdefiniowanych stałych odpowiadających konkretnym atrybutom.
                    Na przykład chcąc ustawić bit EN, rozmiar FIFO na 128 Bajtów oraz przestrzeń adresowania na A24 musimy 
                    w miejsce argumentu attribute podać attribute = (SEN_SET | VFS_128 | AS_24)
        

    Dostępne stałe generujące argument attribute (w kolejnych liniach znajdują się flagi ustawiające jedną wartość, możemy wykorzystać
    tylko jedną flagę z danej listy):
        [SEN_SET, SEN_CLR]
        [TH_SET, TH_CLR]
        [VFS_64, VFS_128, VFS_256, VFS_512]
        [SST_160, SST_267, SST_320]
        [eSSTB_SET, eSSTB_CLR]
        [eSST_SET, eSST_CLR]
        [eVME_SET, eVME_CLR]
        [MBLT_SET, MBLT_CLR]
        [BLT_SET, BLT_CLR]
        [AS_16, AS_24, AS_32, AS_64]
        [SUPR_SET, SUPR_CLR]
        [NPRIV_SET, NPRIV_CLR]
        [SPGM_SET, SPGM_CLR]
        [DATA_SET, DATA_CLR]

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_inbound_translation_attribute(crg_t* ptr, uint8_t index, uint32_t attribute)
{
    // attribute = change_endians32(attribute);

    switch (index)
    {
        case SLAVE_0:  ptr->LCSR.ITAT0 = attribute;    break;
        case SLAVE_1:  ptr->LCSR.ITAT1 = attribute;    break;
        case SLAVE_2:  ptr->LCSR.ITAT2 = attribute;    break;
        case SLAVE_3:  ptr->LCSR.ITAT3 = attribute;    break;
        case SLAVE_4:  ptr->LCSR.ITAT4 = attribute;    break;
        case SLAVE_5:  ptr->LCSR.ITAT5 = attribute;    break;
        case SLAVE_6:  ptr->LCSR.ITAT6 = attribute;    break;
        case SLAVE_7:  ptr->LCSR.ITAT7 = attribute;    break;
        default:    return WRONG_INDEX;
    }

    return CORRECT;
}



/*
    Funkcja włącza okno kanału VME Slave lub włącza kanał Master. Funkcjonalność
    zależy od zadanego argumentu wejściowego index.

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave lub master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7,
                MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]    

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int set_enable(crg_t* ptr, uint8_t index)
{
    uint32_t bufor;
    uint32_t set_endian = EN_SET;
    set_endian = change_endians32(set_endian);

    switch (index)
    {
        case MASTER_0:
            bufor = ptr->LCSR.OTAT0;
            ptr->LCSR.OTAT0 = bufor | set_endian;   break;
        case MASTER_1:
            bufor = ptr->LCSR.OTAT1;
            ptr->LCSR.OTAT1 = bufor | EN_SET;   break;
        case MASTER_2:
            bufor = ptr->LCSR.OTAT2;
            ptr->LCSR.OTAT2 = bufor | EN_SET;   break;
        case MASTER_3:
            bufor = ptr->LCSR.OTAT3;
            ptr->LCSR.OTAT3 = bufor | EN_SET;   break;
        case MASTER_4:
            bufor = ptr->LCSR.OTAT4;
            ptr->LCSR.OTAT4 = bufor | EN_SET;   break;
        case MASTER_5:
            bufor = ptr->LCSR.OTAT5;
            ptr->LCSR.OTAT5 = bufor | EN_SET;   break;
        case MASTER_6:
            bufor = ptr->LCSR.OTAT6;
            ptr->LCSR.OTAT6 = bufor | EN_SET;   break;
        case MASTER_7:
            bufor = ptr->LCSR.OTAT7;
            ptr->LCSR.OTAT7 = bufor | EN_SET;   break;

        case SLAVE_0:
            bufor = ptr->LCSR.ITAT0;
            ptr->LCSR.ITAT0 = bufor | EN_SET;   break;
        case SLAVE_1:
            bufor = ptr->LCSR.ITAT1;
            ptr->LCSR.ITAT1 = bufor | EN_SET;   break;
        case SLAVE_2:
            bufor = ptr->LCSR.ITAT2;
            ptr->LCSR.ITAT2 = bufor | EN_SET;   break;
        case SLAVE_3:
            bufor = ptr->LCSR.ITAT3;
            ptr->LCSR.ITAT3 = bufor | EN_SET;   break;
        case SLAVE_4:
            bufor = ptr->LCSR.ITAT4;
            ptr->LCSR.ITAT4 = bufor | EN_SET;   break;
        case SLAVE_5:
            bufor = ptr->LCSR.ITAT5;
            ptr->LCSR.ITAT5 = bufor | EN_SET;   break;
        case SLAVE_6:
            bufor = ptr->LCSR.ITAT6;
            ptr->LCSR.ITAT6 = bufor | EN_SET;   break;
        case SLAVE_7:
            bufor = ptr->LCSR.ITAT7;
            ptr->LCSR.ITAT7 = bufor | EN_SET;   break;
        default:    return WRONG_INDEX;
    }

    return CORRECT;
}



/*
    Funkcja wyłącza okno kanału VME Slave lub wyłącza kanał Master. Funkcjonalność
    zależy od zadanego argumentu wejściowego index.

    ptr ->  Wskaźnik na strukturę danych mapującą układ rejestrów pamięci mostka tsi148.
            Do pól tej sstruktury będziemy się odwoływać aby ustawić wartości poszczególnych rejestrów.
    index ->    Numer kanału slave lub master, który chcemy ustawić. Poprawne wartości ustawiane są za pomocą stałych z podanej listy:
                [SLAVE_0, SLAVE_1, SLAVE_2, SLAVE_3, SLAVE_4, SLAVE_5, SLAVE_6, SLAVE_7,
                MASTER_0, MASTER_1, MASTER_2, MASTER_3, MASTER_4, MASTER_5, MASTER_6, MASTER_7]    

    Funkcja zwraca:
        -> Stałą WRONG_INDEX jeżeli podany w wywołaniu funkcji indeks jest spoza doswolonego zakresu.
        -> Stałą CORRECT jeżeli funkcja została wykonana poprawnie
*/
int clr_enable(crg_t* ptr, uint8_t index)
{
    uint32_t bufor;

    switch (index)
    {
        case MASTER_0:
            bufor = ptr->LCSR.OTAT0;
            ptr->LCSR.OTAT0 = bufor & (~EN_SET);    break;
        case MASTER_1:
            bufor = ptr->LCSR.OTAT1;
            ptr->LCSR.OTAT1 = bufor & (~EN_SET);    break;
        case MASTER_2:
            bufor = ptr->LCSR.OTAT2;
            ptr->LCSR.OTAT2 = bufor & (~EN_SET);    break;
        case MASTER_3:
            bufor = ptr->LCSR.OTAT3;
            ptr->LCSR.OTAT3 = bufor & (~EN_SET);    break;
        case MASTER_4:
            bufor = ptr->LCSR.OTAT4;
            ptr->LCSR.OTAT4 = bufor & (~EN_SET);    break;
        case MASTER_5:
            bufor = ptr->LCSR.OTAT5;
            ptr->LCSR.OTAT5 = bufor & (~EN_SET);    break;
        case MASTER_6:
            bufor = ptr->LCSR.OTAT6;
            ptr->LCSR.OTAT6 = bufor & (~EN_SET);    break;
        case MASTER_7:
            bufor = ptr->LCSR.OTAT7;
            ptr->LCSR.OTAT7 = bufor & (~EN_SET);    break;

        case SLAVE_0:
            bufor = ptr->LCSR.ITAT0;
            ptr->LCSR.ITAT0 = bufor & (~EN_SET);    break;
        case SLAVE_1:
            bufor = ptr->LCSR.ITAT1;
            ptr->LCSR.ITAT1 = bufor & (~EN_SET);    break;
        case SLAVE_2:
            bufor = ptr->LCSR.ITAT2;
            ptr->LCSR.ITAT2 = bufor & (~EN_SET);    break;
        case SLAVE_3:
            bufor = ptr->LCSR.ITAT3;
            ptr->LCSR.ITAT3 = bufor & (~EN_SET);    break;
        case SLAVE_4:
            bufor = ptr->LCSR.ITAT4;
            ptr->LCSR.ITAT4 = bufor & (~EN_SET);    break;
        case SLAVE_5:
            bufor = ptr->LCSR.ITAT5;
            ptr->LCSR.ITAT5 = bufor & (~EN_SET);    break;
        case SLAVE_6:
            bufor = ptr->LCSR.ITAT6;
            ptr->LCSR.ITAT6 = bufor & (~EN_SET);    break;
        case SLAVE_7:
            bufor = ptr->LCSR.ITAT7;
            ptr->LCSR.ITAT7 = bufor & (~EN_SET);    break;
        default:    return WRONG_INDEX;
    }

    return CORRECT;
}


// tak żeby był main, na razie bez znaczenia
int main()
{
    crg_t *ptr;

    // request I/O privity
    ThreadCtl(_NTO_TCTL_IO, 0 );

    ptr = get_tsi148_ptr();

// A16, D16, pojedyncze cykle, supervisor, dane program na data
    uint32_t master_upper_address = 0x00;
    uint32_t master_lower_address = 0xC1000000;

    set_outbound_translation_starting_address(ptr, MASTER_0, master_upper_address, master_lower_address);

    uint32_t master_end_upper_address = 0x0;
    uint32_t master_end_lower_address = 0xC1001000;

    set_outbound_translation_ending_address(ptr, MASTER_0, master_end_upper_address, master_end_lower_address);

    uint32_t master_upper_offset = 0x0;
    uint32_t master_lower_offset = 0xC1000000 * (-1);

    set_outbound_translation_offset(ptr, MASTER_0, master_upper_offset, master_lower_offset);

    uint32_t master_attributes = ADMODE_A16 | SUP_SET | PGM_CLR | DBW_16 | TR_MODE_SCT;

    master_attributes = change_endians32(master_attributes);

    set_outbound_translation_attribute(ptr, MASTER_0, master_attributes);

    set_enable(ptr, MASTER_0);

    void *master_virtual_address;
    void *addr;
    size_t len = 0x10000;
    int prot = PROT_READ | PROT_WRITE | PROT_NOCACHE;
    int flags = MAP_SHARED;
    uint64_t physical = (master_upper_address << 32) | master_lower_address;


    // na końcu otrzymujemy wskaźnik na wirtualny obszar magistrali
    // poniżej przedstawiono odczytywanie danych za pomocą tego wirtualnego adresu - działa
    master_virtual_address = mmap_device_memory(NULL, len, prot, flags, physical);

    uint8_t *tmp = (uint8_t*)master_virtual_address;
    uint8_t data;

    tmp += 0x81;
    data = *tmp;

    printf("%c", data);

    tmp += 0x2;
    data = *tmp;

    printf("%c", data);

    tmp += 0x2;
    data = *tmp;

    printf("%c", data);

    tmp += 0x2;
    data = *tmp;

    printf("%c\n", data);

    pci_detach(0);



    return 0;
}









