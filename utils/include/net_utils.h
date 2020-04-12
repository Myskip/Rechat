#ifndef __NET_UTILS_H__
#define __NET_UTILS_H__

#ifndef u32 
#define u32 unsigned int
#endif
#ifndef u16 
#define u16 unsigned short
#endif
#ifndef u8 
#define u8 unsigned char
#endif

int readn(int fd, u8 *buf, u32 len);
int writen(int fd, u8 *buf, u32 len);

#endif