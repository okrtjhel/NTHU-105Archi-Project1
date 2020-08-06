CC = g++ -std=c++11
OBJS = simulator.o memory.o regfile.o instruction.o error.o

single_cycle:${OBJS}
	${CC} -o single_cycle ${OBJS}
error.o:error.cpp
	${CC} -g -c error.cpp
regfile.o:regfile.cpp
	${CC} -g -c regfile.cpp
memory.o:memory.cpp regfile.h error.h
	${CC} -g -c memory.cpp
instruction.o:instruction.cpp memory.h regfile.h error.h
	${CC} -g -c instruction.cpp
simulator.o:simulator.cpp memory.h regfile.h instruction.h
	${CC} -g -c simulator.cpp
clean:
	rm -f single_cycle ${OBJS}
clean_others:
	rm -f snapshot.rpt _snapshot.rpt error_dump.rpt _error_dump.rpt iimage.bin dimage.bin
diff_snap:
	diff snapshot.rpt _snapshot.rpt -u
diff_error:
	diff error_dump.rpt _error_dump.rpt -u