# This one Makefile works with Microsoft nmake and Unix make.
# They use different conditional syntax, but each can be nested and inverted within the other.

all: default

ifdef MAKEDIR:
!ifdef MAKEDIR

# Windows code.

CXX = cl
CXXFLAGS = /nologo /MD /Gy /Z7
CC = $(CXX)
CFLAGS = $(CXXFLAGS)
O = obj
EXE = .exe
RM_F = del 2>nul /f

!else
else

# Unix code.
O = o
EXE=
RM_F = rm -f
CXXFLAGS = -g

endif
!endif :

# Common Unix/Windows code.

default: rpn$(EXE)

clean:
	$(RM_F) *.$O rpn$(EXE)

rpn$(EXE): rpn.cpp
	$(CXX) $(CXXFLAGS) rpn.cpp
