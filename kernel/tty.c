#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

int codeWithE0;
int shiftL;
int shiftR;
int altL;
int altR;
int ctrlL;
int ctrlR;
int capsLock;
int numLock;
int scrollLock;

void scroll_screen(CONSOLE*, int);

void putCharInKeyBuf(u8 code){
	if (keyboardInput.count < KB_IN_BYTES)
	{
		*(keyboardInput.pHead) = code;
		keyboardInput.pHead++;
		if (keyboardInput.pHead >= keyboardInput.buf + KB_IN_BYTES)
		{
			keyboardInput.pHead = keyboardInput.buf;
		}
		keyboardInput.count++;
	}
	else
	{
		DispStr("FULL!");
	}
}

void KeyboardHandler()
{
	u8 code = In(0x60);
	putCharInKeyBuf(code);
}

u8 getByteFromKBuffer()
{
	u8 code;
	while (keyboardInput.count<=0){}
	DisableInt();
	code = *(keyboardInput.pTail);
	keyboardInput.pTail++;
	if (keyboardInput.pTail >= keyboardInput.buf + KB_IN_BYTES)
	{
		keyboardInput.pTail = keyboardInput.buf;
	}
	keyboardInput.count--;
	EnableInt();
	return code;
}

void in_process(TTY* pT, u32 key)
{
	char output[32] = {'\0', '\0'};
	if (!(key & FLAG_EXT))
	{
		if (pT->inbuf_count < TTY_IN_BYTES) {
			*(pT->p_inbuf_head) = key;
			pT->p_inbuf_head++;
			if (pT->p_inbuf_head == pT->in_buf + TTY_IN_BYTES) {
				pT->p_inbuf_head = pT->in_buf;
			}
			pT->inbuf_count++;
		}
	}
	else
	{
		int raw_code = key & MASK_RAW;
		switch (raw_code)
		{
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
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {
				select_console(raw_code - F1);
			}
			break;
		default:
			break;
		}
	}
}

void keyboardRead(TTY* pT)
{
	u8 code;
	int make;
	if (keyboardInput.count > 0)
	{
		code = getByteFromKBuffer();
		u32 key = 0;
		//
		if (code == 0xE0)
		{
			code = getByteFromKBuffer();

			/* PrintScreen 被按下 */
			if (code == 0x2A)
			{
				if (getByteFromKBuffer() == 0xE0)
				{
					if (getByteFromKBuffer() == 0x37)
					{
						key = PRINTSCREEN;
						make = 1;
					}
				}
			}
			/* PrintScreen 被释放 */
			if (code == 0xB7)
			{
				if (getByteFromKBuffer() == 0xE0)
				{
					if (getByteFromKBuffer() == 0xAA)
					{
						key = PRINTSCREEN;
						make = 0;
					}
				}
			}
			/* 不是PrintScreen, 此时scan_code为0xE0紧跟的那个值. */
			if (key == 0)
			{
				codeWithE0 = 1;
			}
		}
		else if (code == 0xE1)
		{
			int i;
			u8 pausebrk_scode[] = {0xE1, 0x1D, 0x45,
								   0xE1, 0x9D, 0xC5};
			int is_pausebreak = 1;
			for (i = 1; i < 6; i++)
			{
				if (getByteFromKBuffer() != pausebrk_scode[i])
				{
					is_pausebreak = 0;
					break;
				}
			}
			if (is_pausebreak)
			{
				key = PAUSEBREAK;
			}
		}
		if ((key != PAUSEBREAK) && (key != PRINTSCREEN))
		{
			make = (code & FLAG_BREAK ? FALSE : TRUE);
			int keyCol = 0;
			u32 *keyRow = &keymap[(code & 0x7F) * MAP_COLS];
			if (shiftL || shiftR)
			{
				keyCol = 1;
			}
			if (codeWithE0)
			{
				keyCol = 2;
				codeWithE0 = 0;
			}

			key = keyRow[keyCol];
			switch (key)
			{
			case SHIFT_L:
				shiftL = make;
				break;
			case SHIFT_R:
				shiftR = make;
				break;
			case CTRL_L:
				ctrlL = make;
				break;
			case CTRL_R:
				ctrlR = make;
				break;
			case ALT_L:
				altL = make;
				break;
			case ALT_R:
				altR = make;
				break;
			default:
				break;
			}
			if (make)
			{
				key |= shiftL ? FLAG_SHIFT_L : 0;
				key |= shiftR ? FLAG_SHIFT_R : 0;
				key |= ctrlL ? FLAG_CTRL_L : 0;
				key |= ctrlR ? FLAG_CTRL_R : 0;
				key |= altL ? FLAG_ALT_L : 0;
				key |= altR ? FLAG_ALT_R : 0;
				in_process(pT, key); //terrible name
			}
		}
	}
}
//****************************************
//CONSOLE about

void initScreen(TTY* pT){
	int nr_tty = pT - ttyTable;
	pT->p_console = consoleTable + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1;	/* 显存总大小 (in WORD) */

	int con_v_mem_size                   = v_mem_size / NR_CONSOLES;
	pT->p_console->original_addr      = nr_tty * con_v_mem_size;
	pT->p_console->v_mem_limit        = con_v_mem_size;
	pT->p_console->current_start_addr = pT->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	pT->p_console->cursor = pT->p_console->original_addr;

	if (nr_tty == 0) {
		/* 第一个控制台沿用原来的光标位置 */
		pT->p_console->cursor = DispPos / 2;
		DispPos = 0;
	}
	else {
		out_char(pT->p_console, nr_tty + '0');
		out_char(pT->p_console, '#');
	}
	set_cursor(pT->p_console->cursor);
}


void set_cursor(unsigned int position)
{
	DisableInt();
	Out(CRTC_ADDR_REG, CURSOR_H);
	Out(CRTC_DATA_REG, (position >> 8) & 0xFF);
	Out(CRTC_ADDR_REG, CURSOR_L);
	Out(CRTC_DATA_REG, position & 0xFF);
	EnableInt();
}

void set_video_start_addr(u32 addr)
{
	DisableInt();
	Out(CRTC_ADDR_REG, START_ADDR_H);
	Out(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	Out(CRTC_ADDR_REG, START_ADDR_L);
	Out(CRTC_DATA_REG, addr & 0xFF);
	EnableInt();
}

void out_char(CONSOLE* p_con, char ch)
{
	u8* p_vmem = (u8*)(V_MEM_BASE + p_con->cursor * 2);
	*p_vmem++ = ch;
	*p_vmem++ = DEFAULT_CHAR_COLOR;
	p_con->cursor++;
	set_cursor(p_con->cursor);
}

int isCurrentConsole(CONSOLE* pC){
	return(pC == &consoleTable[nrCurrentConsole]);
}

void select_console(int nr_console)	/* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES)) {
		return;
	}

	nrCurrentConsole = nr_console;

	set_cursor(consoleTable[nr_console].cursor);
	set_video_start_addr(consoleTable[nr_console].current_start_addr);
}

void scroll_screen(CONSOLE* p_con, int direction)
{
	if (direction == SCR_UP) {
		if (p_con->current_start_addr > p_con->original_addr) {
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN) {
		if (p_con->current_start_addr + SCREEN_SIZE <
		    p_con->original_addr + p_con->v_mem_limit) {
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}

//********************************************
//TTY about
void initTty(TTY * pT){
	pT->inbuf_count = 0;
	pT->p_inbuf_head = pT->p_inbuf_tail = pT->in_buf;
	initScreen(pT);
}

void ttyRead(TTY* pT){
	if(isCurrentConsole(pT->p_console)){
		keyboardRead(pT);
	}
}

void ttyWrite(TTY* pT){
	if (pT->inbuf_count) {
		char ch = *(pT->p_inbuf_tail);
		pT->p_inbuf_tail++;
		if (pT->p_inbuf_tail == pT->in_buf + TTY_IN_BYTES) {
			pT->p_inbuf_tail = pT->in_buf;
		}
		pT->inbuf_count--;
		out_char(pT->p_console, ch);
	}	
}

void taskTty()
{
	TTY * pTty;
	for(pTty=ttyTable; pTty<ttyTable+NR_CONSOLES; pTty++){
		initTty(pTty);
	}
	select_console(0);
	while (1)
	{
		for(pTty=ttyTable; pTty<ttyTable+NR_CONSOLES; pTty++){
			ttyRead(pTty);
			ttyWrite(pTty);
		}
	}
}