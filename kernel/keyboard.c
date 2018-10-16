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

void initKeyboard()
{
    numLock = 1;
    keyboardInput.pHead = keyboardInput.pTail = keyboardInput.buf;
    keyboardInput.count = 0;
    irqTable[1] = KeyboardHandler;
    set_leds();
    EnableIrq(KEYBOARD_IRQ);
}

void putCharInKeyBuf(u8 code)
{
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

u8 getByteFromKeyBuf()
{
    u8 code;
    while (keyboardInput.count <= 0)
    {
    }
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

void keyboardRead(TTY *pT)
{
    u8 code;
    int make;
    if (keyboardInput.count > 0)
    {
        code = getByteFromKeyBuf();
        u32 key = 0;
        //
        if (code == 0xE0)
        {
            code = getByteFromKeyBuf();

            /* PrintScreen 被按下 */
            if (code == 0x2A)
            {
                if (getByteFromKeyBuf() == 0xE0)
                {
                    if (getByteFromKeyBuf() == 0x37)
                    {
                        key = PRINTSCREEN;
                        make = 1;
                    }
                }
            }
            /* PrintScreen 被释放 */
            if (code == 0xB7)
            {
                if (getByteFromKeyBuf() == 0xE0)
                {
                    if (getByteFromKeyBuf() == 0xAA)
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
                if (getByteFromKeyBuf() != pausebrk_scode[i])
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
            int caps = shiftL||shiftR;
            if (capsLock)
            {
                if ((keyRow[0] >= 'a') && (keyRow[0] <= 'z')){
					caps = !caps;
				}
            }
            if (caps){
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
            case CAPS_LOCK:
                if (make)
                {
                    capsLock = !capsLock;
                    set_leds();
                }
                break;
            case NUM_LOCK:
                if (make)
                {
                    numLock = !numLock;
                    set_leds();
                }
                break;
            case SCROLL_LOCK:
                if (make)
                {
                    scrollLock = !scrollLock;
                    set_leds();
                }
                break;
            default:
                break;
            }
            if (make)
            {
                int pad = 0;
                if ((key >= PAD_SLASH) && (key <= PAD_9))
                {
                    pad = 1;
                    switch (key)
                    {
                    case PAD_SLASH:
                        key = '/';
                        break;
                    case PAD_STAR:
                        key = '*';
                        break;
                    case PAD_MINUS:
                        key = '-';
                        break;
                    case PAD_PLUS:
                        key = '+';
                        break;
                    case PAD_ENTER:
                        key = ENTER;
                        break;
                    default:
                        if (numLock &&
						    (key >= PAD_0) &&
						    (key <= PAD_9)) {
							key = key - PAD_0 + '0';
						}
						else if (numLock &&
							 (key == PAD_DOT)) {
							key = '.';
						}
                        else{
							switch(key) {
							case PAD_HOME:
								key = HOME;
								break;
							case PAD_END:
								key = END;
								break;
							case PAD_PAGEUP:
								key = PAGEUP;
								break;
							case PAD_PAGEDOWN:
								key = PAGEDOWN;
								break;
							case PAD_INS:
								key = INSERT;
								break;
							case PAD_UP:
								key = UP;
								break;
							case PAD_DOWN:
								key = DOWN;
								break;
							case PAD_LEFT:
								key = LEFT;
								break;
							case PAD_RIGHT:
								key = RIGHT;
								break;
							case PAD_DOT:
								key = DELETE;
								break;
							default:
								break;
							}
                        }
                    }
                }
                key |= shiftL ? FLAG_SHIFT_L : 0;
                key |= shiftR ? FLAG_SHIFT_R : 0;
                key |= ctrlL ? FLAG_CTRL_L : 0;
                key |= ctrlR ? FLAG_CTRL_R : 0;
                key |= altL ? FLAG_ALT_L : 0;
                key |= altR ? FLAG_ALT_R : 0;
                key |= pad ? FLAG_PAD : 0;
                in_process(pT, key); //terrible name
            }
        }
    }
}

void kb_wait() /* 等待 8042 的输入缓冲区空 */
{
    u8 kb_stat;

    do
    {
        kb_stat = In(KB_CMD);
    } while (kb_stat & 0x02);
}

void kb_ack()
{
    u8 kb_read;

    do
    {
        kb_read = In(KB_DATA);
    } while (kb_read = !KB_ACK);
}

void set_leds()
{
    u8 leds = (capsLock << 2) | (numLock << 1) | scrollLock;
    kb_wait();
    Out(KB_DATA, LED_CODE);
    kb_ack();
    kb_wait();
    Out(KB_DATA, leds);
    kb_ack();
}