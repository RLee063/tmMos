#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

void putCharInTtyBuf(TTY *pT, u32 key)
{
	if (pT->inbuf_count < TTY_IN_BYTES)
	{
		*(pT->p_inbuf_head) = key;
		pT->p_inbuf_head++;
		if (pT->p_inbuf_head == pT->in_buf + TTY_IN_BYTES)
		{
			pT->p_inbuf_head = pT->in_buf;
		}
		pT->inbuf_count++;
	}
}

void in_process(TTY *pT, u32 key)
{
	if (!(key & FLAG_EXT))
	{
		putCharInTtyBuf(pT, key);
	}
	else
	{
		int raw_code = key & MASK_RAW;
		switch (raw_code)
		{
		case ENTER:
			putCharInTtyBuf(pT, '\0');
			break;
		case BACKSPACE:
			putCharInTtyBuf(pT, '\b');
			break;
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
			{
				scroll_screen(pT->p_console, SCR_UP);
			}
			break;
		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
			{
				scroll_screen(pT->p_console, SCR_DN);
			}
			break;
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:
			/* Alt + F1~F12 */
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R))
			{
				select_console(raw_code - F1);
			}
			break;
		default:
			break;
		}
	}
}

void initTty(TTY *pT)
{
	pT->inbuf_count = 0;
	pT->p_inbuf_head = pT->p_inbuf_tail = pT->in_buf;
	initScreen(pT);
	initKeyboard();
}

void ttyDoRead(TTY *pT)
{
	if (isCurrentConsole(pT->p_console))
	{
		keyboardRead(pT);
	}
}

void ttyDoWrite(TTY *pT)
{
	if (pT->inbuf_count)
	{
		char ch = *(pT->p_inbuf_tail);
		pT->p_inbuf_tail++;
		if (pT->p_inbuf_tail == pT->in_buf + TTY_IN_BYTES)
		{
			pT->p_inbuf_tail = pT->in_buf;
		}
		pT->inbuf_count--;
		out_char(pT->p_console, ch);
	}
}

void taskTty()
{
	TTY *pTty;
	for (pTty = ttyTable; pTty < ttyTable + NR_CONSOLES; pTty++)
	{
		initTty(pTty);
	}
	select_console(0);
	while (1)
	{
		for (pTty = ttyTable; pTty < ttyTable + NR_CONSOLES; pTty++)
		{
			ttyDoRead(pTty);
			ttyDoWrite(pTty);
		}
	}
}

void ttyWrite(TTY* pT, char* buf, int length){
	while(length){
		out_char(pT->p_console, *buf++);
		length--;
	}
}
