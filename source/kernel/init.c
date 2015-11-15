/**
 * Contem o codigo de entrada para o kernel.
 *
 * @date 15-03-2015
 */

#include <compiler.h>
#include <initium.h>

INITIUM_IMAGE(INITIUM_IMAGE_SECTIONS || INITIUM_IMAGE_LOG);

/**
 * Ponto de entrada do kernel.
 */
__init_text void kmain()
{
    /* Cria um ciclo infinito */
    while(1);
}
