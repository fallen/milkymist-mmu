BOARD_SRC=$(wildcard $(BOARD_DIR)/*.v) $(BOARD_DIR)/../../gen_capabilities.v

ASFIFO_SRC=$(wildcard $(CORES_DIR)/asfifo/rtl/*.v)
CONBUS_SRC=$(wildcard $(CORES_DIR)/conbus/rtl/*.v)
LM32_SRC=							\
	$(CORES_DIR)/lm32/rtl/lm32_include.v			\
	$(CORES_DIR)/lm32/rtl/lm32_cpu.v			\
	$(CORES_DIR)/lm32/rtl/lm32_instruction_unit.v		\
	$(CORES_DIR)/lm32/rtl/lm32_decoder.v			\
	$(CORES_DIR)/lm32/rtl/lm32_load_store_unit.v		\
	$(CORES_DIR)/lm32/rtl/lm32_adder.v			\
	$(CORES_DIR)/lm32/rtl/lm32_addsub.v			\
	$(CORES_DIR)/lm32/rtl/lm32_logic_op.v			\
	$(CORES_DIR)/lm32/rtl/lm32_shifter.v			\
	$(CORES_DIR)/lm32/rtl/lm32_multiplier.v	\
	$(CORES_DIR)/lm32/rtl/lm32_mc_arithmetic.v		\
	$(CORES_DIR)/lm32/rtl/lm32_interrupt.v			\
	$(CORES_DIR)/lm32/rtl/lm32_ram.v			\
	$(CORES_DIR)/lm32/rtl/lm32_dp_ram.v			\
	$(CORES_DIR)/lm32/rtl/lm32_icache.v			\
	$(CORES_DIR)/lm32/rtl/lm32_dcache.v			\
	$(CORES_DIR)/lm32/rtl/lm32_top.v			\
	$(CORES_DIR)/lm32/rtl/lm32_debug.v			\
	$(CORES_DIR)/lm32/rtl/lm32_jtag.v			\
	$(CORES_DIR)/lm32/rtl/lm32_itlb.v			\
	$(CORES_DIR)/lm32/rtl/lm32_dtlb.v			\
	$(CORES_DIR)/lm32/rtl/jtag_cores.v			\
	$(CORES_DIR)/lm32/rtl/jtag_tap_spartan6.v
FMLARB_SRC=$(wildcard $(CORES_DIR)/fmlarb/rtl/*.v)
FMLBRG_SRC=$(wildcard $(CORES_DIR)/fmlbrg/rtl/*.v)
CSRBRG_SRC=$(wildcard $(CORES_DIR)/csrbrg/rtl/*.v)
NORFLASH_SRC=$(wildcard $(CORES_DIR)/norflash16/rtl/*.v)
UART_SRC=$(wildcard $(CORES_DIR)/uart/rtl/*.v)
SYSCTL_SRC=$(wildcard $(CORES_DIR)/sysctl/rtl/*.v)
HPDMC_SRC=$(wildcard $(CORES_DIR)/hpdmc_ddr32/rtl/*.v) $(wildcard $(CORES_DIR)/hpdmc_ddr32/rtl/spartan6/*.v)
VGAFB_SRC=$(wildcard $(CORES_DIR)/vgafb/rtl/*.v)
MEMCARD_SRC=$(wildcard $(CORES_DIR)/memcard/rtl/*.v)
AC97_SRC=$(wildcard $(CORES_DIR)/ac97/rtl/*.v)
PFPU_SRC=$(wildcard $(CORES_DIR)/pfpu/rtl/*.v)
TMU_SRC=$(wildcard $(CORES_DIR)/tmu2/rtl/*.v)
ETHERNET_SRC=$(wildcard $(CORES_DIR)/minimac2/rtl/*.v)
FMLMETER_SRC=$(wildcard $(CORES_DIR)/fmlmeter/rtl/*.v)
VIDEOIN_SRC=$(wildcard $(CORES_DIR)/bt656cap/rtl/*.v)
IR_SRC=$(wildcard $(CORES_DIR)/rc5/rtl/*.v)
DMX_SRC=$(wildcard $(CORES_DIR)/dmx/rtl/*.v)
USB_SRC=$(wildcard $(CORES_DIR)/softusb/rtl/*.v)
MEMTEST_SRC=$(wildcard $(CORES_DIR)/memtest/rtl/*.v)
MONITOR_SRC=$(wildcard $(CORES_DIR)/monitor/rtl/*.v)

CORES_SRC=$(ASFIFO_SRC) $(CONBUS_SRC) $(LM32_SRC) $(FMLARB_SRC) $(FMLBRG_SRC) $(CSRBRG_SRC) $(NORFLASH_SRC) $(UART_SRC) $(SYSCTL_SRC) $(HPDMC_SRC) $(VGAFB_SRC) $(MEMCARD_SRC) $(AC97_SRC) $(PFPU_SRC) $(TMU_SRC) $(ETHERNET_SRC) $(FMLMETER_SRC) $(VIDEOIN_SRC) $(DMX_SRC) $(IR_SRC) $(USB_SRC) $(MEMTEST_SRC) $(MONITOR_SRC)
