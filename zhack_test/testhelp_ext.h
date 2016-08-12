#ifndef __TESTHELP_EXT_H
#define __TESTHELP_EXT_H

//增强显示，突出PASS和FAILED
#define MY_COLOR_RED       "\033[1;31;40m"
#define MY_COLOR_RED_FLASH "\033[5;1;31;40m"
#define MY_COLOR_GREEN     "\033[1;32;40m"
#define MY_COLOR_YELLOW    "\033[1;33;40m"
#define MY_COLOR_BLUE      "\033[1;34;40m"
#define MY_COLOR_MAGENTA   "\033[1;35;40m"
#define MY_COLOR_CYAN      "\033[1;36;40m"
#define MY_COLOR_WHITE     "\033[1;37;40m"
#define MY_COLOR_RESET    "\033[0m"

int __failed_tests_ext = 0;
int __test_num_ext = 0;
#define test_cond_ext(descr,_c) do { \
    __failed_tests_ext++; printf("%d - %s: ", __failed_tests_ext, descr); \
    if(_c) printf( MY_COLOR_GREEN "PASSED" MY_COLOR_RESET "\n" ); \
    else {printf(MY_COLOR_RED_FLASH "FAILED" MY_COLOR_RESET "\n"); __failed_tests_ext++;} \
} while(0);
#define test_report_ext() do { \
    printf( "\n"MY_COLOR_WHITE "%d tests" MY_COLOR_RESET "," \
                MY_COLOR_GREEN "%d passed " MY_COLOR_RESET"," \
                MY_COLOR_RED "%d"MY_COLOR_RESET MY_COLOR_YELLOW " failed" MY_COLOR_RESET "\n\n" , \
            __failed_tests_ext, \
            __failed_tests_ext-__failed_tests_ext, __failed_tests_ext); \
    if (__failed_tests_ext) { \
        printf( MY_COLOR_YELLOW "=== WARNING === We have failed tests here..." MY_COLOR_RESET "\n\n"); \
        exit(1); \
    } \
} while(0);

#endif
