src=$(wildcard *.c)
obj=$(patsubst %.c,%.o,$(src))
target=../myclient
CFLAGS=-Wall -I .
CC=gcc 

$(target):$(obj)
	gcc -o $@  $^ 
#%.o:%.c
#	gcc -c $< -o $@ $(LDFLAGS)

clean:
	rm -f $(obj) $(target)
.PHONY:clean
