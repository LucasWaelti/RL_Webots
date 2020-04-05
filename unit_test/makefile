# See: http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# Note -Wl,rpath,... links for the dynamic linker 

CC = g++
LIBS_PATH = ../libraries
LIBS = -lvpg_network 
CPPFLAGS = -I. -I$(LIBS_PATH) -I../controllers/ -I../controllers/vpg_controller -L$(LIBS_PATH) -Wl,-rpath,$(LIBS_PATH)

DEPS = vpg_test.hpp
OBJ = vpg_test.o 

%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $(DEPS) $(CPPFLAGS)

dqn_test: $(OBJ) 
	$(CC) -Wall -o vpg_test vpg_test.o $(CPPFLAGS) $(LIBS)

.PHONY: clean # do not do anything with files named "clean" 
clean: 
	rm -f *.o *~ vpg_test 