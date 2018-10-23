#include "kernel.h"
#include "global.h"
int printf(const char *fmt, ...)
{
	int i;
	char buf[256];

	char* arg = ((char*)(&fmt) + 4); /*4是参数fmt所占堆栈中的大小*/
	i = vsprintf(buf, fmt, arg);
	buf[i] = 0;
	printx(buf);
	return i;
}

char* i2a(int val, int base, char ** ps)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		i2a(q, base, ps);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	return *ps;
}

int vsprintf(char *buf, const char *fmt, char* args)
{
	char*	p;

	char*	p_next_arg = args;
	int	m;

	char	inner_buf[STR_DEFAULT_LEN];
	char	cs;
	int	align_nr;

	for (p=buf;*fmt;fmt++) {
		DispPos = 0;
		DispStr("flag1 ");
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;
		}

		fmt++;

		if (*fmt == '%') {
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}
		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}
		DispStr("flag2 ");
		char * q = inner_buf;
		MemSet(q, 0, sizeof(inner_buf));

		switch (*fmt) {
		case 'c':
			*q++ = *((char*)p_next_arg);
			p_next_arg += 4;
			break;
		case 'x':
			m = *((int*)p_next_arg);
			i2a(m, 16, &q);
			p_next_arg += 4;
			break;
		case 'd':
			m = *((int*)p_next_arg);
			if (m < 0) {
				m = m * (-1);
				*q++ = '-';
			}
			i2a(m, 10, &q);
			p_next_arg += 4;
			break;
		case 's':
			StrCpy(q, (*((char**)p_next_arg)));
			q += StrLen(*((char**)p_next_arg));
			p_next_arg += 4;
			break;
		default:
			break;
		}
		DispStr("flag3 ");
		int k;
		for (k = 0; k < ((align_nr > StrLen(inner_buf)) ? (align_nr - StrLen(inner_buf)) : 0); k++) {
			*p++ = cs;
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}
	DispStr("flag4 ");
	*p = 0;

	return (p - buf);
}
