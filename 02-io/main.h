#ifndef __MAIN_H__
#define __MAIN_H__

#define LOG(...) logger_log_f(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define DIE(message) logger_die(__FILE__, __LINE__, __func__, (message))
#define PERROR_DIE(context)                                                    \
    logger_perror_die(__FILE__, __LINE__, __func__, (context))

int logger_log_f(const char *file, int line, const char *func,
                 const char *format, ...);
int logger_die(const char *file, int line, const char *func,
               const char *message);
int logger_perror_die(const char *file, int line, const char *func,
                      const char *context);

extern int follow;
extern int use_stdio;
extern unsigned int buffer_size;

#endif
