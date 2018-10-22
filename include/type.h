#pragma once

typedef unsigned long long  u64;
typedef	unsigned int		u32;
typedef	unsigned short		u16;
typedef	unsigned char		u8;

enum msgtype {
	HARD_INT = 1,
	GET_TICKS,
};

typedef enum _funciont {
    SEND,
    RECEIVE,
}FUNCTION;

typedef enum _procStatus {
    NORMAL,
    SENDING,
    RECEIVING,
}PROC_STATUS;

typedef struct _msg MESSAGE;
