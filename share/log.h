#ifndef __LOG_H__
#define __LOG_H__

#define condition_if_false_ret(statement, rv)\
    do{\
        if (!(statement)) {\
            printf("%s, not satisfied\n", #statement);\
            return rv;\
        }\
    }while(0)

#define condition_if_true_ret(statement, rv)\
    do{\
        if (statement) {\
            printf("%s, not satisfied\n", #statement);\
            return rv;\
        }\
    }while(0)
#endif
