# makefile for building Lua
# see luaconf.h for further customization

# Project modified to compile as C++ for throwing exceptions on errors.
CXXFLAGS= -pipe -Wall $(BLDCFLAGS) # -DLUA_USE_APICHECK to have Lua assert for
                                   # correct parameters
AR= ar rcu
RM= rm -f

LUA_A=	liblua.a
CORE_O=	lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o lmem.o \
	lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o ltm.o  \
	lundump.o lvm.o lzio.o
LIB_O=	lauxlib.o lbaselib.o ldblib.o liolib.o lmathlib.o loslib.o ltablib.o \
	lstrlib.o linit.o

ALL_O= $(CORE_O) $(LIB_O)
ALL_A= $(LUA_A)

all: $(LUA_A)

$(LUA_A): $(ALL_O)
	$(AR) $@ $?

clean:
	$(RM) *.a *.o

depend:
	@$(CXX) $(CXXFLAGS) -MM *.cpp

lapi.o: lapi.cpp lua.h luaconf.h lapi.h lobject.h llimits.h ldebug.h \
  lstate.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h \
  lundump.h lvm.h
lauxlib.o: lauxlib.cpp lua.h luaconf.h lauxlib.h
lbaselib.o: lbaselib.cpp lua.h luaconf.h lauxlib.h lualib.h
lcode.o: lcode.cpp lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
  lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h lgc.h \
  ltable.h
ldblib.o: ldblib.cpp lua.h luaconf.h lauxlib.h lualib.h
ldebug.o: ldebug.cpp lua.h luaconf.h lapi.h lobject.h llimits.h lcode.h \
  llex.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h \
  lfunc.h lstring.h lgc.h ltable.h lvm.h
ldo.o: ldo.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lparser.h lstring.h \
  ltable.h lundump.h lvm.h
ldump.o: ldump.cpp lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h \
  lzio.h lmem.h lundump.h
lfunc.o: lfunc.cpp lua.h luaconf.h lfunc.h lobject.h llimits.h lgc.h lmem.h \
  lstate.h ltm.h lzio.h
lgc.o: lgc.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
linit.o: linit.cpp lua.h luaconf.h lualib.h lauxlib.h
liolib.o: liolib.cpp lua.h luaconf.h lauxlib.h lualib.h
llex.o: llex.cpp lua.h luaconf.h ldo.h lobject.h llimits.h lstate.h ltm.h \
  lzio.h lmem.h llex.h lparser.h lstring.h lgc.h ltable.h
lmathlib.o: lmathlib.cpp lua.h luaconf.h lauxlib.h lualib.h
lmem.o: lmem.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h
lobject.o: lobject.cpp lua.h luaconf.h ldo.h lobject.h llimits.h lstate.h \
  ltm.h lzio.h lmem.h lstring.h lgc.h lvm.h
lopcodes.o: lopcodes.cpp lopcodes.h llimits.h lua.h luaconf.h
loslib.o: loslib.cpp lua.h luaconf.h lauxlib.h lualib.h
lparser.o: lparser.cpp lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
  lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h \
  lfunc.h lstring.h lgc.h ltable.h
lstate.o: lstate.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h llex.h lstring.h ltable.h
lstring.o: lstring.cpp lua.h luaconf.h lmem.h llimits.h lobject.h lstate.h \
  ltm.h lzio.h lstring.h lgc.h
lstrlib.o: lstrlib.cpp lua.h luaconf.h lauxlib.h lualib.h
ltable.o: ltable.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h lgc.h ltable.h
ltablib.o: ltablib.cpp lua.h luaconf.h lauxlib.h lualib.h
ltm.o: ltm.cpp lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h lzio.h \
  lmem.h lstring.h lgc.h ltable.h
lundump.o: lundump.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h \
  llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h lundump.h
lvm.o: lvm.cpp lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lstring.h ltable.h lvm.h
lzio.o: lzio.cpp lua.h luaconf.h llimits.h lmem.h lstate.h lobject.h ltm.h \
  lzio.h

# (end of Makefile)
