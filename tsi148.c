#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
// #include <hw/pci.h>
// #include <sys/mman.h>

#include "tsi148.h"

/*
 * Grupa 2, podgrupa projektowa 1
 * Autorzy:
 * Rafal Osadnik 192239
 * Michal Koruszowic 192243
 * Natalia Rucińska 176285
 * */


#define TSI148_CHKERR(error) if(error != PCI_SUCCESS){return error;} // Makro do obslugi bledow na magistrali PCI.

int32_t get_tsi148_ptr(crg_t* ptr){

	/*
	 * Funkcja wyszukuje mostka TSI148 na magistrali PCI, dokonuje ekstrakcji jego adresu sprz�towego
	 * a nast�pnie generuje wska�nik wirtualny aby umo�liwic komunikacj� z procesorem.
	 *
	 * Parametry wejsciowe:
	 *  - brak
	 *
	 *  Parametry wyjsciowe:
	 *  ptr - wska�nik do obiektu o typie crg_t do kt�rego przypisany zostanie wygenerowany adres wirtualny.
	 *
	 *  Wartosc zwracana:
	 *  - PCI_SUCCESS
	 *  - PCI_DEVICE_NOT_FOUNS
	 *  - PCI_BAD_REGISTER_NUMBER
	 *
	 *  Przyklad uzycia:
	 *
	 *  int main(int argc, char *argv[]) {
     *
     *		crg_t bridge_struct;
     *
     *		int32_t errc = get_tsi148_ptr(&bridge_struct);
     *
     *		if(errc == PCI_SUCCESS){
     *			printf("%s", "Success!\n");
     *			printf("INT line ID: %x\n", bridge_struct.PCFS.INTL);
     *		}
     *		else{
     *			printf("%s", "Error occurred!\n");
     *		}
     *		return EXIT_SUCCESS;
     *	}
	 */
		const unsigned venid = 0x10e3; // VendorID z dokumentacji
		const unsigned devid = 0x0148; // DeviceID z dokumentacji

		unsigned bus = 0;        // A pointer to a location where the function can store the bus number of the device or function found.
		unsigned dev_func = 0;   // A pointer to a location where the function can store the device or function ID
                                 // of the nth device or function found with the specified device and vendor IDs.
                                 // The device number is in bits 7 through 3, and the function number in bits 2 through 0.


        // http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_attach.html
		pci_attach(0);

        // http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_attach.html
        // Find the PCI device with a given device ID and vendor ID
		int status = pci_find_device(devid, venid, 0, &bus, &dev_func);

		TSI148_CHKERR(status);

		uint32_t upper = 0;
		uint32_t lower = 0;
		uint64_t base_address = 0;

		// http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_attach.html
		// Read 32-bit values from the configuration space of a device
		status = pci_read_config32(bus, dev_func, 0x14, 1, &upper);

		TSI148_CHKERR(status);

		status = pci_read_config32(bus, dev_func, 0x10,1, &lower);

		TSI148_CHKERR(status);

		base_address = (upper << 32) + lower;

		base_address &= ~0xFFF;

        // http://www.qnx.com/developers/docs/6.5.0/index.jsp?topic=%2Fcom.qnx.doc.neutrino_lib_ref%2Fp%2Fpci_attach.html
        // Map a device's physical memory into a process's address space
		*ptr = *((crg_t*)mmap_device_memory( 0, sizeof(crg_t), PROT_READ|PROT_WRITE|PROT_NOCACHE, 0,  base_address));

		pci_detach(0);

		return PCI_SUCCESS;
}


// int main(int argc, char *argv[]) {

// 	crg_t bridge_struct;

// 	int32_t errc = get_tsi148_ptr(&bridge_struct);

// 	if(errc == PCI_SUCCESS){
// 		printf("%s", "Success!\n");
// 		printf("INT line ID: %x\n", bridge_struct.PCFS.INTL);
// 	}
// 	else{
// 		printf("%s", "Error occurred!\n");
// 	}
// 	return EXIT_SUCCESS;
// }

