
BUILD=cl /nologo /EHsc /O2 /std:c++20
LINK=/link User32.lib

all: easy-focus.exe

easy-focus.exe: easy-focus.cpp rectangle.h
	$(BUILD) /Fe: easy-focus.exe easy-focus.cpp $(LINK)

