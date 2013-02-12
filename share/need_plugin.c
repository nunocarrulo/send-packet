#include <stdio.h>
#include <dlfcn.h>

void *flib;
int (*pfunc)(void);
char *error_message;

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
