
BUILD=cl /nologo /EHsc /O2 /std:c++20
LINK=/link User32.lib Shell32.lib

all: winfocus.exe

winfocus.exe: winfocus.cpp rectangle.h config.cpp config.h
	$(BUILD) /Fe: winfocus.exe winfocus.cpp config.cpp $(LINK)

