OPENEXRDIR = /usr/include

#OPENEXRLIBS = -L$(OPENEXRDIR)/OpenEXR/IlmImf -L$(OPENEXRDIR)/IlmBase/lib -lIlmImf-2_2 -lHalf -lIex-2_2 -lIexMath-2_2 -lIlmThread-2_2 -lImath-2_2
OPENEXRLIBS = -L/usr/lib/x86_64-linux-gnu -lIlmImf -lHalf -lIex -lIexMath -lIlmThread -lImath
LIBS += $(OPENEXRLIBS)

OPENEXRINCLUDE= -I$(OPENEXRDIR) -I$(OPENEXRDIR)/OpenEXR/config -I$(OPENEXRDIR)/IlmBase/config -I$(OPENEXRDIR)/IlmBase/include/OpenEXR
INCLUDES += $(OPENEXRINCLUDE)
