#ifndef	_PCI_H_
#define	_PCI_H_

#include <kernel/types.h>
#include <stdio.h>
#include <system.h>

#define	PCI_CONFIG_DATA		0x0CFC
#define	PCI_CONFIG_ADDRESS	0x0CF8
#define	PCI_CSE			0x0CF8
#define	PCI_FORWARD		0x0CFA

#define	PCI_ERROR_NODEV		1

#define	PCI_ADDRESS_REG(b,u,f,o)		\
			(o & 0xFC)	| 	\
			(f << 8)	|	\
			(u << 11)	|	\
			(b << 16) | 0x80000000

#define	STRING_MAX	64

typedef struct pci_device
{
	uint16_t	vend, dev;
	uint8_t	cls, sub;
	uint8_t	bus, unit, func;
//	void		(*pci_probe)(void*);
//	void		(*pci_unprobe)(void*);
	//char*		class_str;
	//char*		subclass_str;
	struct pci_device*	next;
} pci_device_t;

typedef	struct	pci
{
	uint16_t	max;
	pci_device_t*	dev;
} pci_t;

//TODO: função para carregar a estrutura PCI (usar em PCI discover)

uint16_t	pci_discover(uint8_t method, bool configure);
uint16_t	pci_getmax();
uint8_t	pci_setdevice(pci_device_t* dev, uint16_t id);
uint32_t	pci_read(uint32_t reg);
void		pci_write(uint32_t reg, uint32_t val);
const char*	pci_class_string(uint8_t _class);
const char*	pci_subclass_string(uint8_t _class, uint8_t subclass);

#endif	/* _PCI_H_ */
