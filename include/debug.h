void debug_init(void);
int debug_send(char* s);
int debug_send_arb(char* s, int len);
int debug_sendfc(char* fmt, char arg);
void debug_sendf(char* fmt, ...);
