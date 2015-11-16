/**
 * Contem o codigo de entrada para o kernel.
 *
 * @date 15-03-2015
 */

#include <compiler.h>
#include <initium.h>

/** Indica ao Initium que é um Kernel suportado */
INITIUM_IMAGE(INITIUM_IMAGE_SECTIONS || INITIUM_IMAGE_LOG);

/** Endereço da lista de tags do Initium */
static initium_tag_t *initium_tag_list __init_text;

/**
 * Ponto de entrada do kernel.
 *
 * @param magic     Numero magico do Initium.
 * @param tags      Apontador para a lista de tags.
 */
__init_text void kmain(uint32_t magic, initium_tag_t *tags)
{
    /** Guarda o endereço a lista de tags. */
    initium_tag_list = tags;

    /* Cria um ciclo infinito */
    while(1);

    /** TODO: iniciar o kdb */

    /** TODO: iniciar as funcs. basicas da consola */

    /** TODO: inicia o as funções basicas do log */

    /** TODO: inicia as funções basicas da CPU */

    /** TODO: inicia a gestão de memoria */

    /** TODO: configura a consola por completo */

    /** TODO: finaliza a inicialização da CPU */

    /** TODO: (...) */
}
