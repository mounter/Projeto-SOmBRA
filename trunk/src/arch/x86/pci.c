/*****************************************************************************
**                                                                          **
**	Arquivo: pci.c                                                          **
**	Descrição: Manipula a interface PCI                                     **
**	Autor: Leonardo Monteiro Silva (gnomo)                                  **
**	Data: 19/05/2008                                                        **
**                                                                          **
*****************************************************************************/
#include <x86/pci.h>
#include <kernel/mm.h>

const char*	pci_class[] =
{
	"Dispositivo desconhecido (Pre PCI-2.0)",
	"Controlador de armazenamento em massa",
	"Controlador de Rede",
	"Controlador de tela",
	"Dispositivo de multimidia",
	"Controlador de memoria",
	"Dispositivo de ponte",
	"Controlador de comunicacao simples",
	"Periferico basico do sistema",
	"Estacao de atracamento",
	"Processador",
	"Controlador USB",
	0
};

const char*	DEV_UNKNW = "Dispositivo desconhecido";
const char*	DEV_MISC = "Misc";
const char*	DEV_GEN	= "Dispositivo generico";

struct	pci_subclass
{
	const char*	str[STRING_MAX];
	uint8_t	max, can_generic;
} subclass[] =
{
	{ .str[0] = "Controlador de Video", .str[1] = "VGA Compativel", .max = 2, .can_generic = 0 },
	{ .str[0] = "Controlador SCSI", .str[1] = "Controlador IDE", .str[2] = "Controlador de Disquete",
	  .str[3] = "Controlador IPI", .str[4] = "Controlador RAID", .max = 5, .can_generic = 1	 },
	{ .str[0] = "Controlador Ethernet", .str[1] = "Anel Coletor (Token Ring)", .str[2] = "Controlador FDDI",
	  .str[3] = "Controlador ATM", .max = 4, .can_generic = 1 },
	{ .str[0] = "Controlador VGA Compativel", .str[1] = "Controlador XGA", .max = 2, .can_generic = 1 },
	{ .str[0] = "Dispositivo de Video", .str[1] = "Dispositivo de Audio", .max = 2, .can_generic = 1 },
	{ .str[0] = "Memoria de Acesso Randomico (RAM)", .str[1] = "Memoria Flash", .max = 2, .can_generic = 1 },
	{ .str[0] = "Ponte Host-PCI", .str[1] = "Ponte PCI-ISA", .str[2] = "Ponte PCI-EISA",
	  .str[3] = "Ponte PCI-Microcanal", .str[4] = "Ponte PCI-PCI", .str[5] = "Ponte PCI-PCMCIA",
	  .str[6] = "Ponte PCI-NuBus", .str[7] = "Ponte PCI-CardBus", .max = 8, .can_generic = 1 },
	{ .str[0] = "Controlador Serial", .str[1] = "Porta Paralela", .max = 2, .can_generic = 1 },
	{ .str[0] = "Controlador PIC", .str[1] = "Controlador DMA", .str[2] = "Temporizador",
	  .str[3] = "Controlador de Tempo Real (RTC)", .max = 4, .can_generic = 1 },
	{ .str[0] = "Controlador de Teclado", .str[1] = "Caneta", .str[2] = "Controlador de Mouse",
	  .max = 3, .can_generic = 1 }
};

pci_t*	pci;

/* Descobre todas unidades PCI existentes, configurando cada uma se possível e solicitado */
uint16_t	pci_discover(uint8_t method, bool configure)
{
	int u, b, f;
	uint16_t vend/*, dev*/;
	uint32_t val, class;
	void*	ptr;

	pci = (pci_t*)malloc(sizeof(pci_t));
	pci->dev = (pci_device_t*)malloc(sizeof(pci_device_t));
	ptr = (void*)pci->dev;

	if(pci->dev)
	{
	pci->max = 0;
	printf("Escaneando interface PCI...");
	switch(method)
	{
		/* Método de configuração 1 */
		// O mecanismo 1 consiste em escanear todos os Bus, Units e Functions
		case 1:
			for(b = 0; b<=0xFF; b++)
			{
				for(u = 0; u < 32; u++)
				{
					for(f = 0; f<8; f++)
					{
						val = pci_read(PCI_ADDRESS_REG(b,u,f,0));
						vend = (uint16_t)val;
						// se Vendor é diferente de 0xFFFF ou 0x0
						// então existe um dispositivo
						if(vend != 0xFFFF && vend != 0x0000)
						{
							class = pci_read(PCI_ADDRESS_REG(b,u,f,8));
							pci->max += 1;
							pci->dev->bus = b;
							pci->dev->unit = u;
							pci->dev->func = f;
							pci->dev->vend = vend;
							pci->dev->dev = (uint16_t)(val >> 16);
							pci->dev->cls = (uint8_t)((class >> 24) & 0xFF);
							pci->dev->sub = (uint8_t)((class >> 16) & 0xFF);

							//pci->dev->class_str = pci_class_string(pci->dev->cls);
							//pci->dev->subclass_str = pci_subclass_string(pci->dev->cls, pci->dev->sub);

							/* if(configure) */
							//TODO: Procura e carrega driver
							//TODO: Salvar informação (bus, slot, func...) em arquivo somente leitura, de forma que fique facilmente acessivel ao usuário
							pci->dev->next = (pci_device_t*)malloc(sizeof(pci_device_t));
							pci->dev = pci->dev->next;
							if(!pci->dev)
							{
								printf("pci_discover(): memoria nao reservada.\n*** Alguns dispositivos podem nao ter sido detectados!");
								break;
							}
						}
					}
				}
			}
			break;
		case 2:
			//TODO: Metodo de configuração PCI #2
			// Mecanismo 2 não implementado, tentar mecanismo 1...
			return pci_discover(1,configure);
			break;
		case 3:
			//TODO: Metodo de configuração PCI #3
			// Mecanismo 3 não implementado, tentar mecanismo 1...
			return pci_discover(1,configure);
			break;
		default:
			panic("PCI: Metodo de configuração desconhecido.\n");
			break;
	}
	} else return NULL;
	pci->dev->next = NULL;
	pci->dev = (pci_device_t*)ptr;
	//printf(" pronto!\n");

	//retorna o número de dispositivos encontrados
	return pci->max;
}

/* Retorna o número de dispositivos PCI */
uint16_t	pci_getmax()
{
	return pci->max;
}

/* A função pci_discover() deve ser chamada antes. */
uint8_t	pci_setdevice(pci_device_t* dev, uint16_t id)
{
	pci_device_t* tmp;
	uint16_t aux;
	aux = id;
	if(id > pci_getmax())
	{
		return	PCI_ERROR_NODEV;
	}

	/* tmp = primeira entrada registrada */
	tmp = pci->dev;
	/* Se id > 0 então percorre lista de dispositivos. */
	while(aux--)
	{
		tmp = tmp->next;
	}

	if(tmp == NULL)
		return	PCI_ERROR_NODEV;

	dev->vend = tmp->vend;
	dev->dev = tmp->dev;
	dev->cls = tmp->cls;
	dev->sub = tmp->sub;
	dev->bus = tmp->bus;
	dev->unit = tmp->unit;
	dev->func = tmp->func;
	//strcpy(dev->class_str, tmp->class_str);
	//strcpy(dev->subclass_str, tmp->subclass_str);
	return 0;
}

/* Lê 4-bytes da área de configuração de uma unidade PCI */
uint32_t	pci_read(uint32_t reg)
{
	uint32_t tmp = 0;

	outportl (PCI_CONFIG_ADDRESS, reg);
	tmp = (uint32_t)inportl(PCI_CONFIG_DATA);

	return tmp;

}

/* Escreve 4-bytes na área de configuração de uma unidade PCI */
void	pci_write(uint32_t reg, uint32_t val)
{
	outportl(PCI_CONFIG_ADDRESS, reg);
	outportl(PCI_CONFIG_DATA, val);
}

/* Converte classe ID para texto correspondente */
const char*	pci_class_string(uint8_t _class)
{
	if (_class <= 0xC)
		return pci_class[_class];
	else
	if (_class == 0xFF)
		return DEV_MISC;
	else
		return DEV_UNKNW;
}

/* Converte Classe/Subclasse ID's para texto correspondente ao dispositivo */
const char*	pci_subclass_string(uint8_t _class, uint8_t sub)
{
	if(_class < 0xA) // 0xA = Entradas em subclass[]; TODO: criar macro para contar o número de entradas. (???)
	{
		if(sub < subclass[_class].max)
			return subclass[_class].str[sub];
		else
		if(subclass[_class].can_generic && sub == 0x80)
			return DEV_GEN;
		else
			return DEV_UNKNW;
	}
	return (const char*)"Error (?)";
}


/* Registra dispositivos PCI em (RAM-)arquivo de configuração */
uint8_t	pci_register_devices(bool register_only)
{
	//TODO: ...
	return	0;
}

