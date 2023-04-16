#define CLOCKS_PER_DELAY_LOOP 3
#define CALL_CYCLES 60

#define _delay_min_ns() (uint32_t)((float)CALL_CYCLES * 1000000000U / (rcc_ahb_frequency))

inline static void _delay_ns(volatile uint32_t ns) {
	volatile uint32_t loop_count;

	loop_count = (float)ns * rcc_ahb_frequency / (CLOCKS_PER_DELAY_LOOP * 1000000000U);

	if(loop_count > CALL_CYCLES / CLOCKS_PER_DELAY_LOOP)
		loop_count -= CALL_CYCLES / CLOCKS_PER_DELAY_LOOP;
	else
		return;

	__asm__ volatile(
		" mov r0, %[loop_count] \n\t"
		"1: subs r0, #1 \n\t"
		" bhi 1b \n\t"
		:
		: [loop_count] "r" (loop_count)
		: "r0"
	);
}