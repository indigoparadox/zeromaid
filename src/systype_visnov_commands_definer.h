
#ifndef command_in
#error "No command specified to define. Define one with #define before \
calling this header."
#else

#define STVNCD_STRUCT typedef struct { \
   SYSTYPE_VISNOV_COMMAND command; \
} SYSTYPE_VISNOV_COMMAND_##command_in;

STVNCD_STRUCT;

#endif /* command_in */
