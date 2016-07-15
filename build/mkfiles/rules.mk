
SDCC ?= sdcc


%.rel : %.c
	$(SDCC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@