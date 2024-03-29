#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = 
CFLAGS = -Wall
RESINC = 
LIBDIR = 
LIB = 
LDFLAGS = 

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -std=c++11 -g
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB) -lnetfilter_queue -lnfnetlink -lpthread
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = src
DEP_RELEASE =
OUT_RELEASE = dambox 

OBJ_RELEASE = $(OBJDIR_RELEASE)/main.o $(OBJDIR_RELEASE)/nfq.o $(OBJDIR_RELEASE)/ordonnanceur.o $(OBJDIR_RELEASE)/process_rcv_write.o $(OBJDIR_RELEASE)/process_read_send.o $(OBJDIR_RELEASE)/ressources.o $(OBJDIR_RELEASE)/args.o

all: release
clean: clean_release

before_release: 

after_release: 
	rm $(OBJDIR_RELEASE)/*.o

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/main.o: $(OBJDIR_RELEASE)/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/main.cpp -o $(OBJDIR_RELEASE)/main.o

$(OBJDIR_RELEASE)/nfq.o: $(OBJDIR_RELEASE)/nfq.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/nfq.cpp -o $(OBJDIR_RELEASE)/nfq.o

$(OBJDIR_RELEASE)/ordonnanceur.o: $(OBJDIR_RELEASE)/ordonnanceur.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/ordonnanceur.cpp -o $(OBJDIR_RELEASE)/ordonnanceur.o

$(OBJDIR_RELEASE)/process_rcv_write.o: $(OBJDIR_RELEASE)/process_rcv_write.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/process_rcv_write.cpp -o $(OBJDIR_RELEASE)/process_rcv_write.o

$(OBJDIR_RELEASE)/process_read_send.o: $(OBJDIR_RELEASE)/process_read_send.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/process_read_send.cpp -o $(OBJDIR_RELEASE)/process_read_send.o

$(OBJDIR_RELEASE)/ressources.o: $(OBJDIR_RELEASE)/ressources.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/ressources.cpp -o $(OBJDIR_RELEASE)/ressources.o

$(OBJDIR_RELEASE)/args.o: $(OBJDIR_RELEASE)/args.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c $(OBJDIR_RELEASE)/args.cpp -o $(OBJDIR_RELEASE)/args.o

install: dambox
	install -D dambox /usr/bin/dambox

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release

.PHONY: before_release after_release clean_release
