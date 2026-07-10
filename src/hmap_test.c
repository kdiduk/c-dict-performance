#include "hmap.h"


int main(void)
{
    struct hmap* hmap = hmap_create(42);

    hmap_destroy(hmap);

    return 0;
}
