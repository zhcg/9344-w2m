# Makefile.in generated by automake 1.9.6 from Makefile.am.
# omapip/Makefile.  Generated from Makefile.in by configure.

# Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
# 2003, 2004, 2005  Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.




srcdir = .
top_srcdir = ..

INCLUDES = -I. -I$(srcdir) -I$(top_srcdir) -I$(top_srcdir)/includes -DHMAC_MD5

COMPILE = $(CC) $(DEFS) $(INCLUDES) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@


libomapi_a_OBJECTS = protocol.$(OBJEXT) buffer.$(OBJEXT) \
	alloc.$(OBJEXT) result.$(OBJEXT) connection.$(OBJEXT) \
	errwarn.$(OBJEXT) listener.$(OBJEXT) dispatch.$(OBJEXT) \
	generic.$(OBJEXT) support.$(OBJEXT) handle.$(OBJEXT) \
	message.$(OBJEXT) convert.$(OBJEXT) hash.$(OBJEXT) \
	auth.$(OBJEXT) inet_addr.$(OBJEXT) array.$(OBJEXT) \
	trace.$(OBJEXT) mrtrace.$(OBJEXT) toisc.$(OBJEXT) \
	iscprint.$(OBJEXT)

all:	libomapi.a
	

libomapi.a: $(libomapi_a_OBJECTS)  
	-rm -f libomapi.a
	$(AR) $(ARFLAGS) libomapi.a $(libomapi_a_OBJECTS) 
	$(RANLIB) libomapi.a


.c.o:
	$(COMPILE) -c $<

clean: 
	-rm *.o
	-rm libomapi.a


.PHONY: all clean 
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
