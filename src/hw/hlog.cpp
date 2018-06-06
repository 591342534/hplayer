#include "hlog.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <mutex>
#include "hcos.h"

#define LOGBUF_SIZE         (1<<13)  // 8k
#define LOGFILE_MAXSIZE     (1<<23)  // 8M

static FILE* s_logfp = NULL;
static char s_logfile[256] = DEFAULT_LOG_FILE;
static int s_loglevel = DEFAULT_LOG_LEVEL;
static char s_logbuf[LOGBUF_SIZE];
static std::mutex s_mutex;

int hlog_set_file(const char* logfile) {
    if (logfile && strlen(logfile) > 0) {
        strncpy(s_logfile, logfile, 256);
    }
    
    if (s_logfp) {
        fclose(s_logfp);
        s_logfp = NULL;
    }

    s_logfp = fopen(s_logfile, "a");

    return s_logfp ? 0 : -1;
}

void hlog_set_level(int level){
    s_loglevel = level;
}

int hlog_printf(int level, const char* fmt, ...) {
    if (level < s_loglevel)
        return -10;

    const char* pcolor = CL_WHITE;
    const char* plevel = "LOG";
#define CASE_LOG(id, str, clr) \
    case id: plevel = str; pcolor = clr; break;

    switch (level) {
        FOREACH_LOG(CASE_LOG)
    }
#undef CASE_LOG

#if !LOG_WITH_COLOR
    pcolor = "";
#endif

    std::lock_guard<std::mutex> locker(s_mutex);

    if (!s_logfp){
        if (hlog_set_file(s_logfile) != 0)
            return -20;
    }

    if (ftell(s_logfp) > LOGFILE_MAXSIZE){
        fclose(s_logfp);
        s_logfp = fopen(s_logfile, "w");
        if (!s_logfp)
            return -30;
    }

    datetime_t now = get_datetime();

    int len = snprintf(s_logbuf, LOGBUF_SIZE, "%s[%04d:%02d:%02d %02d-%02d-%02d.%03d][%s]: ",
        pcolor, now.year, now.month, now.day, now.hour, now.min, now.sec, now.ms, plevel);
    va_list ap;
    va_start(ap, fmt);
    len += vsnprintf(s_logbuf + len, LOGBUF_SIZE-len, fmt, ap);
    va_end(ap);

    fprintf(s_logfp, "%s\n" CL_CLR, s_logbuf);
    fflush(NULL);

    return len;
}