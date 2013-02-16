#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

void *flib;
int (*pfunc)(void);
int (*pfunc_cksm_uint16)(uint8_t *ptr, uint16_t *cksm);
char *error_message;

int get_uint16_by_plugin(uint8_t *ptr, uint16_t *cksm, char *name)
{
    int result = 0;

    flib = dlopen("./plugin/libplugin.so", RTLD_LAZY);
    error_message = dlerror();

    if (error_message) {
        printf("err: %s.\n", error_message);
        return (-1);
    }
    *(void **)(&pfunc_cksm_uint16) = dlsym(flib, name);
    error_message = dlerror();
    if (error_message) {
        printf("err message chunxiao: %s.\n", error_message);
        return (-1);
    }

    result = pfunc_cksm_uint16(ptr, cksm);
    //printf("result: %d, check sum: %x.\n", result, *cksm);

    dlclose(flib);
    error_message = dlerror();

    if (error_message) {
        printf("err: %s.\n", error_message);
        return (-1);
    }

    return result;
}

int plugin_test()
{
    int result = 0;

    flib = dlopen("./plugin/libplugin.so", RTLD_LAZY);
    error_message = dlerror();

    if (error_message) {
        printf("err: %s.\n", error_message);
        return (-1);
    }

    *(void **)(&pfunc) = dlsym(flib, "plugin_test1");
    error_message = dlerror();
    if (error_message) {
        printf("err message chunxiao: %s.\n", error_message);
        return (-1);
    }

    result = pfunc();
    printf("result : %d.\n", result);

    *(void **)(&pfunc) = dlsym(flib, "plugin_test2");
    error_message = dlerror();
    if (error_message) {
        printf("err message chunxiao: %s.\n", error_message);
        return (-1);
    }

    result = pfunc();
    printf("result : %d.\n", result);

    dlclose(flib);
    error_message = dlerror();

    if (error_message) {
        printf("err: %s.\n", error_message);
        return (-1);
    }

    return 0;
}
