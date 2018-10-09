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

void KeyboardHandler()
{
	u8 code = In(0x60);
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

u8 getByteFromKBuffer()
{
	u8 code;
	DisableInt();
	code = *(keyboardInput.pTail);
	keyboardInput.pTail++;
	if (keyboardInput.pTail >= keyboardInput.buf + KB_IN_BYTES)
	{
		keyboardInput.pTail = keyboardInput.buf;
	}
	keyboardInput.count--;
	EnableInt();
	// DispStr(" Origininput:");
	// DispInt(code);
	return code;
}

void in_process(u32 key)
{
	char output[32] = {'\0', '\0'};
	if (!(key & FLAG_EXT))
	{
		output[0] = key & 0xFF;
		DispStr(output);

		DisableInt();
		Out(CRTC_ADDR_REG, CURSOR_H);
		Out(CRTC_DATA_REG, ((DispPos / 2) >> 8) & 0xFF);
		Out(CRTC_ADDR_REG, CURSOR_L);
		Out(CRTC_DATA_REG, (DispPos / 2) & 0xFF);
		EnableInt();
	}
	else
	{
		int raw_code = key & MASK_RAW;
		switch (raw_code)
		{
		case UP:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
			{
				DisableInt();
				Out(CRTC_ADDR_REG, START_ADDR_H);
				Out(CRTC_DATA_REG, ((80 * 15) >> 8) & 0xFF);
				Out(CRTC_ADDR_REG, START_ADDR_L);
				Out(CRTC_DATA_REG, (80 * 15) & 0xFF);
				EnableInt();
			}
			break;
		case DOWN:
			if ((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R))
			{
				/* Shift+Down, do nothing */
			}
			break;
		default:
			break;
		}
	}
}

void keyboardRead()
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
			// DispStr(" Codeinkeymap:");
			// DispInt(key);
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
				// DispStr(" Keytoin_process:");
				// DispInt(key);
				in_process(key); //terrible name
			}
		}
	}
}

void taskTty()
{
	while (1)
	{
		keyboardRead();
	}
}