all:run
quantum:quantum.c
	cc -O3 -mcpu=cortex-a72 -mtune=cortex-a72 -mfpu=neon-fp-armv8 -mfloat-abi=hard -mneon-for-64bits -ffast-math -fno-trapping-math -fopenmp -o quantum quantum.c
run:quantum
	./quantum 1 100000000 1 10 6
touch:
	touch quantum.c
