/*
 * Milkymist SoC (Software)
 * Copyright (C) 2007, 2008, 2009, 2010 Sebastien Bourdeauducq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <hw/interrupts.h>
#include <irq.h>
#include <uart.h>

#include <hal/usb.h>
#include <hal/tmu.h>

void isr()
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & IRQ_UART)
		uart_isr();
/*		
	if(irqs & IRQ_TMU)
		tmu_isr();

	if(irqs & IRQ_USB)
		usb_isr();
*/
}
