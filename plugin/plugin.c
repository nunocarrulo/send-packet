#include <stdio.h>
int plugin_test1(void)
{
    printf("plugin test: %s.\n", __func__);
    return 0;  
}  

int plugin_test2(void)
{
    printf("plugin test: %s.\n", __func__);
    return 0;  
}  
