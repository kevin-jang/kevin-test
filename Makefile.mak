#*****************************************************************************
# Makefile.mak
##############################################################################


#----------------------------------------------------------------------------
# Environment
#----------------------------------------------------------------------------
# ifndef AIM_TOOLS
# 	$(error The AIM_TOOLS environment variable is not defined!)
# endif
# 
# include $(AIM_TOOLS)/make_init.txt
# include $(AIM_SYSTEM)/api/sysapi_proj.txt
# include $(AIM_SYSTEM)/api/tcltk_proj.txt

CC = gcc
OBJ_EXT = .o
LIB_EXT = .a

LIBCSRC =  \
	winutl.c \
	ser.c \
	eth.c
#	pipe.c

LIBOBJS = $(LIBCSRC:.c=$(OBJ_EXT))

UTLLIB = ../utl/utllib$(LIB_EXT) 

#----------------------------------------------------------------------------
# Standard targets
#----------------------------------------------------------------------------

default:: all

#all:: lib exe
all:: lib

lib: $(UTLLIB)

#bootlib: lib

exe: $(BINS)

clean::
	rm -f $(LOCLIB) $(LIBOBJS) $(BINOBJS) $(BINS)

neat::
	rm -f $(LIBOBJS) $(BINOBJS)

distclean::
	rm -f $(DEPENDFILE)

#----------------------------------------------------------------------------
# OSI SYS targets
#----------------------------------------------------------------------------

# Note:
# Symbolic links are broken - these are files shared among several developers.


$(UTLLIB): $(LIBCSRC)
	$(CC) $(CFLAGS) -c -DLINUX=1 $(LIBCSRC)
	$(AR) $(ARFLAGS) $(AROUT)$@ $(LIBOBJS)
#	$(CC) $(CFLAGS) -c -DNT=1 -DAIM=1 $(LIBCSRC)

#-----------------------------------------------------------------------------
# Compilation Dependencies
#-----------------------------------------------------------------------------

-include $(DEPENDFILE)

	
#-----------------------------------------------------------------------------
# Tell make how to create .o/.obj files
#-----------------------------------------------------------------------------
include $(AIM_TOOLS)/make_def.txt
