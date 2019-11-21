#include <fsti.h>


int main()
{
    struct fsti_simset simset;
    fsti_simset_init(&simset);
    fsti_simset_free(&simset);
}
