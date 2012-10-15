#ifndef __COMMON_H__
#define __COMMON_H__
static void *sp_zmalloc(size_t size)
{
    void *p = malloc(size);
    if (p == NULL) {
        return NULL;
    }
    memset(p, 0, size);
    return p;
}
#endif
