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

uint64_t htonll(uint64_t v)
{
    union {
        uint32_t lv[2];
        uint64_t llv;
    } u;
    u.lv[0] = htonl(v >> 32);
    u.lv[1] = htonl(v & 0xFFFFFFFFULL);
    return u.llv;
}

uint64_t ntohll(uint64_t v)
{
    union {
        uint32_t lv[2];
        uint64_t llv;
    } u;
    u.llv = v;
    return ((uint64_t) ntohl(u.lv[0]) << 32) | (uint64_t) ntohl(u.lv[1]);
}
#endif
