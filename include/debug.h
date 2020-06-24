#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef DEBUG
void debug_logs(const char *str);
void debug_logi(int n);
void debug_logx(int n);
#else
#define debug_logs(str)
#define debug_logi(n)
#define debug_logx(n)
#endif

#endif  /* _DEBUG_H */