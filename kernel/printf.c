int printf(const char *fmt, ...)
{
	int i;
	char buf[256];

	char* arg = ((char*)(&fmt) + 4); /*4是参数fmt所占堆栈中的大小*/
	i = vsprintf(buf, fmt, arg);
	Write(buf, i);
	return i;
}

int vsprintf(char *buf, const char *fmt, char* args)
{
	char*	p;
	char	tmp[256];
	char*	p_next_arg = args;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {
			*p++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt) {
		case 'x':
			itoa(tmp, *((int*)p_next_arg));
			StrCpy(p, tmp);
			p_next_arg += 4;
			p += StrLen(tmp);
			break;
		case 's':
			break;
		default:
			break;
		}
	}

	return (p - buf);
}
