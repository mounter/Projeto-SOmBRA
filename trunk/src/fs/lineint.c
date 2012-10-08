#include "lineint.h"
#include <string.h>
#include <kernel/mm.h>

char **line_words = NULL;
char *lineint_path = NULL;
size_t line_count;

void lineint_init(char *path)
{
    line_count = 0;

    if (line_words != NULL)
        free(line_words);

    if (lineint_path != NULL)
        free(lineint_path);

    lineint_path = malloc((strlen(path) + 1) * sizeof(char));

    if (lineint_path == NULL)
        return;

    strcpy(lineint_path, path);

    lineint_intr();
}

void lineint_intr(void)
{
    unsigned i, s;
    size_t lsz = strlen(lineint_path);

    for (i = 0; i < lsz; i++)
        if (lineint_path[i] == '/')
            line_count++;

    line_words = malloc(line_count * sizeof(char *));

    for (i = 0, s = 0; i < lsz; i++)
        if (lineint_path[i] == '/')
        {
            lineint_path[i] = 0;
            line_words[s++] = &lineint_path[i+1];
        }
}

size_t lineint_count(void)
{
    return line_count;
}

char *lineint_index(unsigned index)
{
    if (index >= line_count) return NULL;

    return line_words[index];
}
