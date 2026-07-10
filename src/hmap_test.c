#include <assert.h>
#include <stdio.h>

#include "hmap.h"


int main(void)
{
    struct hmap* hmap = hmap_create(42);

    assert(hmap_find(hmap, "abc") == NULL);

    hmap_destroy(hmap);

    printf("All tests passed!\n");

    return 0;
}
