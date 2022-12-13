#include "projekt_2.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "tsi148.h"



/*
    Skróty i odniesienia wykorzystywane w kodzie:
        PM ->   Tsi148 PCI/X-to-VME Bus Programming Manual
*/



// tak żeby był main, na razie bez znaczenia
int main()
{
    while(1)
    {

    }

    return 0;
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
int set_translation_starting_address(crg_t* ptr, uint8_t index, uint32_t upper_address, uint16_t lower_address)
{
    switch (index)
    {
        case 0:
            ptr->LCSR.OTSAU0 = upper_address; 
                // ^ Rejestr przechowujący bardziej znaczącą część adresu.          
            ptr->LCSR.OTSAL0 = lower_address << 16;     
                /*
                Rejestr przechowujący mniej znaczącą część adresu.
                Mniej znacząca część adresu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_address przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case 1:
            ptr->LCSR.OTSAU1 = upper_address;
            ptr->LCSR.OTSAL1 = lower_address << 16;
            break;
        case 2:
            ptr->LCSR.OTSAU2 = upper_address;
            ptr->LCSR.OTSAL2 = lower_address << 16;
            break;
        case 3:
            ptr->LCSR.OTSAU3 = upper_address;
            ptr->LCSR.OTSAL3 = lower_address << 16;
            break;
        case 4:
            ptr->LCSR.OTSAU4 = upper_address;
            ptr->LCSR.OTSAL4 = lower_address << 16;
            break;
        case 5:
            ptr->LCSR.OTSAU5 = upper_address;
            ptr->LCSR.OTSAL5 = lower_address << 16;
            break;
        case 6:
            ptr->LCSR.OTSAU6 = upper_address;
            ptr->LCSR.OTSAL6 = lower_address << 16;
            break;
        case 7:
            ptr->LCSR.OTSAU7 = upper_address;
            ptr->LCSR.OTSAL7 = lower_address << 16;
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
int set_translation_ending_address(crg_t* ptr, uint8_t index, uint32_t upper_address, uint16_t lower_address)
{
    switch (index)
    {
        case 0:
            ptr->LCSR.OTEAU0 = upper_address;
                // ^ Rejestr przechowujący bardziej znaczącą część adresu. 
            ptr->LCSR.OTEAL0 = lower_address << 16;
                /*
                Rejestr przechowujący mniej znaczącą część adresu.
                Mniej znacząca część adresu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_address przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case 1:
            ptr->LCSR.OTEAU1 = upper_address;
            ptr->LCSR.OTEAL1 = lower_address << 16;
            break;
        case 2:
            ptr->LCSR.OTEAU2 = upper_address;
            ptr->LCSR.OTEAL2 = lower_address << 16;
            break;
        case 3:
            ptr->LCSR.OTEAU3 = upper_address;
            ptr->LCSR.OTEAL3 = lower_address << 16;
            break;
        case 4:
            ptr->LCSR.OTEAU4 = upper_address;
            ptr->LCSR.OTEAL4 = lower_address << 16;
            break;
        case 5:
            ptr->LCSR.OTEAU5 = upper_address;
            ptr->LCSR.OTEAL5 = lower_address << 16;
            break;
        case 6:
            ptr->LCSR.OTEAU6 = upper_address;
            ptr->LCSR.OTEAL6 = lower_address << 16;
            break;
        case 7:
            ptr->LCSR.OTEAU7 = upper_address;
            ptr->LCSR.OTEAL7 = lower_address << 16;
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
int set_translation_offset(crg_t* ptr, uint8_t index, uint32_t upper_offset, uint16_t lower_offset)
{
    switch (index)
    {
        case 0:
            ptr->LCSR.OTOFU0 = upper_offset;
                // ^ Rejestr przechowujący bardziej znaczącą część offsetu.
            ptr->LCSR.OTOFL0 = lower_offset << 16;
                /*
                Rejestr przechowujący mniej znaczącą część offsetu.
                Mniej znacząca część offsetu ma 16-bitów, ale jest zapisywana w 32-bitowym rejestrze na bitach 31-16,
                w związku z tym należy argument lower_offset przesunąć bitowo o 16 pozycji w lewo.
                */
            break;
        case 1:
            ptr->LCSR.OTOFU1 = upper_offset;
            ptr->LCSR.OTOFL1 = lower_offset << 16;
            break;
        case 2:
            ptr->LCSR.OTOFU2 = upper_offset;
            ptr->LCSR.OTOFL2 = lower_offset << 16;
            break;
        case 3:
            ptr->LCSR.OTOFU3 = upper_offset;
            ptr->LCSR.OTOFL3 = lower_offset << 16;
            break;
        case 4:
            ptr->LCSR.OTOFU4 = upper_offset;
            ptr->LCSR.OTOFL4 = lower_offset << 16;
            break;
        case 5:
            ptr->LCSR.OTOFU5 = upper_offset;
            ptr->LCSR.OTOFL5 = lower_offset << 16;
            break;
        case 6:
            ptr->LCSR.OTOFU6 = upper_offset;
            ptr->LCSR.OTOFL6 = lower_offset << 16;
            break;
        case 7:
            ptr->LCSR.OTOFU7 = upper_offset;
            ptr->LCSR.OTOFL7 = lower_offset << 16;
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
int set_translation_2eSST_broadcast_select(crg_t* ptr, uint8_t index, uint32_t broadcast_select_2eSST)
{
    switch (index)
    {
        case 0:     ptr->LCSR.OTBS0 = broadcast_select_2eSST;   break;
        case 1:     ptr->LCSR.OTBS1 = broadcast_select_2eSST;   break;
        case 2:     ptr->LCSR.OTBS2 = broadcast_select_2eSST;   break;
        case 3:     ptr->LCSR.OTBS3 = broadcast_select_2eSST;   break;
        case 4:     ptr->LCSR.OTBS4 = broadcast_select_2eSST;   break;
        case 5:     ptr->LCSR.OTBS5 = broadcast_select_2eSST;   break;
        case 6:     ptr->LCSR.OTBS6 = broadcast_select_2eSST;   break;
        case 7:     ptr->LCSR.OTBS7 = broadcast_select_2eSST;   break;
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
int set_translation_attribute(crg_t* ptr, uint8_t index, uint32_t attribute)
{
    switch (index)
    {
        case 0:     ptr->LCSR.OTAT0 = attribute;    break;
        case 1:     ptr->LCSR.OTAT1 = attribute;    break;
        case 2:     ptr->LCSR.OTAT2 = attribute;    break;
        case 3:     ptr->LCSR.OTAT3 = attribute;    break;
        case 4:     ptr->LCSR.OTAT4 = attribute;    break;
        case 5:     ptr->LCSR.OTAT5 = attribute;    break;
        case 6:     ptr->LCSR.OTAT6 = attribute;    break;
        case 7:     ptr->LCSR.OTAT7 = attribute;    break;
        default:    return WRONG_INDEX;
    }


    return CORRECT;
}





