#include <basic_types.h>

typedef enum {
    PKT_TYPE_LOGIN = 0,
    PKT_TYPE_LOGOUT,
    PKT_TYPE_CHAT_MSG,
    PKT_TYPE_HEART_BEAT,
}PKT_TYPE;

typedef struct {
    u16 header;
    u16 len;   
    u16 type; 
}PKT_HEAD;

typedef struct {
    PKT_HEAD head;
    u8 username[32];
    u8 password[32];
    u8 checksum;
}LOGIN_PKT;

typedef struct 
{
    PKT_HEAD head;
    u8 username[32];
    u8 checksum;
};
