CC := gcc

PACKAGES := \
	drmollineda.so \
	daniel_ispalindrome.so 

%.so: proc_%.c
	$(CC) -shared -fpic -o $@ $^

all: ${PACKAGES}

clean:
	rm -rf *.o *.so
