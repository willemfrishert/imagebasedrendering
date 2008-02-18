STDLIBS=-lGLEW -lglut -lGLU -lGL -lXmu -lX11 -lm
SRCDIR=./src
INCDIR=./inc
OBJDIR=./obj
BINDIR=./bin
IMGDIR=./img

CC=g++
CFLAGS=-g -I/usr/include -I/usr/local/include -I/usr/include/GL -I./inc -Wall
LFLAGS=-O
LIBS=-L/usr/lib -L/usr/lib64 -L/usr/X11R6/lib $(STDLIBS)
OBJECTS=$(OBJDIR)/BloomEffect.o \
	$(OBJDIR)/BlurTexture.o \
	$(OBJDIR)/CodecRGBE.o \
	$(OBJDIR)/CubeMap.o \
	$(OBJDIR)/Diffuse.o \
	$(OBJDIR)/ExtensionsLoader.o \
	$(OBJDIR)/FrameBufferObject.o \
	$(OBJDIR)/glm_avl.o \
	$(OBJDIR)/GLMenu.o \
	$(OBJDIR)/GPUParallelReductor.o \
	$(OBJDIR)/hdrloader.o \
	$(OBJDIR)/IBLPerfectReflection.o \
	$(OBJDIR)/IBLRefraction.o \
	$(OBJDIR)/LuminanceConverter.o \
	$(OBJDIR)/Main.o \
	$(OBJDIR)/Matrix4.o \
	$(OBJDIR)/Mesh.o \
	$(OBJDIR)/OBJMeshLoader.o \
	$(OBJDIR)/OpenGLUtility.o \
	$(OBJDIR)/PhotographicToneMapper.o \
	$(OBJDIR)/Porcelain.o \
	$(OBJDIR)/Quaternion.o \
	$(OBJDIR)/Renderer.o \
	$(OBJDIR)/ScreenCapture.o \
	$(OBJDIR)/ShaderAttributeObject.o \
	$(OBJDIR)/ShaderObject.o \
	$(OBJDIR)/ShaderProgram.o \
	$(OBJDIR)/ShaderUniformObject.o \
	$(OBJDIR)/SShapedCurveToneMapper.o \
	$(OBJDIR)/UIHandler.o \
	$(OBJDIR)/Vertex.o \
	$(OBJDIR)/VirtualTrackball.o

PROGS=rtibr
VPATH=$(SRCDIR):$(INCDIR)

all: $(PROGS)
.PHONY : all clean remove_backups rebuild

rtibr: $(OBJECTS)
	$(CC) -o $(BINDIR)/$@ $^ $(CFLAGS) $(LFLAGS) $(LIBS)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

rebuild: clean all

clean:
	rm -f $(BINDIR)/$(PROGS) $(OBJDIR)/*.o $(SRCDIR)/*~ $(INCDIR)/*~ $(IMGDIR)/*

remove_backups:
	rm -f *.cpp~ *h~
