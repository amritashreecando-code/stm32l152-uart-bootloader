# Bare-Metal UART Bootloader for STM32L152RE

A custom bootloader for the **STM32L152RE (NUCLEO-L152RE)**, written in bare-metal C with no HAL and no CMSIS.
It receives an application over **UART** from a Python script, writes it into flash, and then jumps to it.

Demo video :[watch it on LinkedIn] https://lnkd.in/p/dkJTd3Ea

---

## What it does

1. At reset, the bootloader (at `0x0800 0000`) sets the clock to **HSI 16 MHz** and initialises **USART2** (9600 baud).
2. A Python script on the PC sends the application's size (a 4-byte header), then the application itself.
3. The bootloader **erases** the required flash pages and **writes the application word by word** at `0x0800 4000`.
4. It then **relocates the vector table** (`VTOR`), loads the application's **stack pointer**, and **jumps** to the application's `Reset_Handler`.
5. The application (an LED blink on PA5 / LD2) starts running.

```
 PC (Python)                       STM32L152RE
 ───────────                       ───────────
 4-byte size header  ───────────►  erase pages
                     ◄───────────  'A'
 word 0 (4 bytes)    ───────────►  write word at 0x0800 4000
                     ◄───────────  'A'
 word 1 (4 bytes)    ───────────►  write word at 0x0800 4004
                     ◄───────────  'A'
   ...                               ...
 last word           ───────────►  write last word
                     ◄───────────  'A'
                                   lock flash → set VTOR → set MSP → jump
                                   LED app runs
```

---

## Memory map

| Region | Address range | Size | Contents |
|---|---|---|---|
| Bootloader | `0x0800 0000` – `0x0800 3FFF` | 16 KB reserved (about 2 KB used) | this bootloader |
| Application | `0x0800 4000` – `0x0807 FFFF` | 496 KB | the application (LED blink, 756 bytes) |
| RAM | `0x2000 0000` – `0x2001 3FFF` | 80 KB | shared by both (one at a time) |

The two linker scripts must agree with this layout:

| Project | `FLASH` in `STM32L152RETX_FLASH.ld` |
|---|---|
| `bootloader` | `ORIGIN = 0x8000000, LENGTH = 16K` |
| `led_app` | `ORIGIN = 0x8004000, LENGTH = 496K` |

---

## Transfer protocol

| Step | Direction | Content |
|---|---|---|
| 1 | PC → MCU | 4-byte size of the application, little-endian (padded by the PC to a multiple of 4) |
| 2 | MCU → PC | `'A'`: pages erased, ready |
| 3 | PC → MCU | one 32-bit word (4 bytes) |
| 4 | MCU → PC | `'A'`: word written, ready for the next |
| | | steps 3–4 repeat until all words are sent |

UART settings: **USART2, PA2 (TX) / PA3 (RX), 9600 baud, 8N1**, through the ST-Link Virtual COM Port.

The `'A'` acknowledgement is a simple **flow-control handshake**: the PC never sends while the MCU is busy erasing or programming flash, so no bytes are lost.

---

## Repository structure

```
bootloader/          STM32CubeIDE project (bare metal)
  Inc/               base.h, gpio.h, uart.h, flash.h
  Src/               main.c, gpio.c, uart.c, flash.c
  Startup/           startup_stm32l152retx.s
  STM32L152RETX_FLASH.ld
led_app/             STM32CubeIDE project: LED blink linked at 0x0800 4000
tools/
  upload_word.py     Python script that sends the application
```

### Bootloader modules

| File | Responsibility |
|---|---|
| `uart.c` | system clock (HSI 16 MHz, 1 flash wait state), USART2 setup, byte/word receive, byte transmit |
| `gpio.c` | PA2/PA3 alternate function AF7 for USART2 |
| `flash.c` | flash unlock/lock, page erase, word programming, receive-and-flash loop, jump to application |
| `main.c` | calls the steps in order |

---

## How to run

1. **Build `led_app`** in STM32CubeIDE, with *Convert to binary file (-O binary)* enabled
   (Project Properties → C/C++ Build → Settings → MCU Post build outputs).
   This creates `led_app/Debug/led_app.bin`. Do **not** run this project directly.
2. **Flash the bootloader** with STM32CubeIDE (*Run*). The bootloader starts and waits for the header.
3. **Edit `tools/upload_word.py`**: set the COM port of the ST-Link Virtual COM Port (see Windows Device Manager) and the path to `led_app.bin`.
4. **Run the script** (Python 3 with `pyserial` installed):
   ```
   python upload_word.py
   ```
5. The script prints `All 756 bytes sent and written.`, and LD2 starts blinking.

After any reset, the bootloader runs again and waits for a new application.

---

## Problems solved along the way

**1. Wrong USART register layout.**
The STM32L1 uses the older USART register map (`SR`, `DR`, `BRR`, `CR1` at `0x00`, `0x04`, `0x08`, `0x0C`, with `UE` at bit 13), and `RCC_APB1ENR` is at offset `0x24`. Using offsets from other STM32 families left the USART silent.

**2. Lost bytes during flash operations (UART overrun).**
Erasing or programming flash keeps the CPU busy for milliseconds, while a byte arrives about every 1 ms at 9600 baud. The single-byte receive register overflowed, and bytes were silently lost. An overrun counter (`ORE` flag) was added for diagnosis, and the `'A'` handshake fixed the problem.

**3. CPU lockup during half-page programming.**
Half-page programming, run from flash, made the CPU lock up (PC = `0xFFFFFFFE`), because the flash cannot be read while a half-page write is in progress. Switching to **word programming** (one 32-bit word at a time, waiting for `BSY` to clear) solved it.

**4. Flash wait states.**
At 16 MHz in voltage range 2 (the reset default), RM0038 requires **1 wait state**. The bootloader sets `ACC64` first (in a separate write), then prefetch and `LATENCY`, **before** switching to HSI.

**5. Relocated application.**
The application is linked at `0x0800 4000`, so that its vector table, `Reset_Handler` address and all other addresses are correct for where it is written. Before the jump, the bootloader sets `VTOR = 0x0800 4000` so the application's interrupts use its own vector table.

---

## Tools

- STM32CubeIDE 1.17, arm-none-eabi-gcc
- NUCLEO-L152RE (on-board ST-Link for flashing, debugging and the Virtual COM Port)
- Python 3 with pyserial
- Reference: ST RM0038 (STM32L1xx reference manual)
