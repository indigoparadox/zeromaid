#ifndef	__net_print_h__
#define	__net_print_h__

int net_print_init(const char *rhost, unsigned short port);

int net_print_string( const char* file, int line, const char* format, ...);

int net_print_binary( int format, const void* binary, int len);

#define DEFAULT_NET_PRINT_PORT	5194
#define	DEFAULT_NET_PRINT_HOST  "192.168.250.10" //"collie.home.net"

#endif
