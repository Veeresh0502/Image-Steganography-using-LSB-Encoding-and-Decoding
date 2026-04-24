obj:=$(patsubst %.c, %.o, $(wildcard *.c))
out.exe: $(obj)
	gcc -o $@ $^
clean:
	rm *.o *.exe