#include "type.h"
#include "kernel.h"
#include "protect.h"
#include "funcs.h"
#include "global.h"

void initScreen(TTY *pT)
{
	int nr_tty = pT - ttyTable;
	pT->p_console = consoleTable + nr_tty;

	int v_mem_size = V_MEM_SIZE >> 1; /* 显存总大小 (in WORD) */

	int con_v_mem_size = v_mem_size / NR_CONSOLES;
	pT->p_console->original_addr = nr_tty * con_v_mem_size;
	pT->p_console->v_mem_limit = con_v_mem_size;
	pT->p_console->current_start_addr = pT->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	pT->p_console->cursor = pT->p_console->original_addr;

	if (nr_tty == 0)
	{
		/* 第一个控制台沿用原来的光标位置 */
		pT->p_console->cursor = DispPos / 2;
		DispPos = 0;
	}
	else
	{
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

void out_char(CONSOLE *p_con, char ch)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	switch (ch)
	{
	case '\0':
		if(p_con->cursor+SCREEN_WIDTH < p_con->original_addr + p_con->v_mem_limit){
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * 
				((p_con->cursor - p_con->original_addr) /
				 SCREEN_WIDTH + 1);			
		}
		break;
	case '\b':
		if (p_con->cursor > p_con->original_addr){
			p_con->cursor--;
			*(p_vmem-2) = ' ';
			*(p_vmem-1) = DEFAULT_CHAR_COLOR;
		}
		break;
	default:
		if (p_con->cursor <
		    p_con->original_addr + p_con->v_mem_limit - 1) {
			*p_vmem++ = ch;
			*p_vmem++ = DEFAULT_CHAR_COLOR;
			p_con->cursor++;
		}
		break;
	}

	if (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE) {
		scroll_screen(p_con, SCR_DN);
	}

	if(isCurrentConsole(p_con)){
		set_cursor(p_con->cursor);
		set_video_start_addr(p_con->current_start_addr);
	}
}

int isCurrentConsole(CONSOLE *pC)
{
	return (pC == &consoleTable[nrCurrentConsole]);
}

void select_console(int nr_console) /* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
	{
		return;
	}

	nrCurrentConsole = nr_console;

	set_cursor(consoleTable[nr_console].cursor);
	set_video_start_addr(consoleTable[nr_console].current_start_addr);
}

void scroll_screen(CONSOLE *p_con, int direction)
{
	if (direction == SCR_UP)
	{
		if (p_con->current_start_addr > p_con->original_addr)
		{
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN)
	{
		if (p_con->current_start_addr + SCREEN_SIZE <
			p_con->original_addr + p_con->v_mem_limit)
		{
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else
	{
	}
	if(isCurrentConsole(p_con)){
		set_video_start_addr(p_con->current_start_addr);
		set_cursor(p_con->cursor);
	}

}
