yess: decodeStage.o dump.o executeStage.o fetchStage.o main.o memory.o memoryStage.o registers.o tools.o writebackStage.o loader.o
	gcc -g decodeStage.o dump.o executeStage.o fetchStage.o main.o memory.o memoryStage.o registers.o tools.o writebackStage.o loader.o -o yess

decodeStage.o: decodeStage.h tools.h decodeStage.c

dump.o: decodeStage.h dump.h executeStage.h fetchStage.h memory.h memoryStage.h registers.h writebackStage.h

executeStage.o: executeStage.h tools.h executeStage.c

fetchStage.o: fetchStage.h tools.h fetchStage.c

main.o: main.c tools.h memory.h dump.h

memory.o: memory.h tools.h memory.c

memoryStage.o: memoryStage.h tools.h memoryStage.c

registers.o: registers.h tools.h registers.c

tools.o: tools.h tools.c

writebackStage.o: writebackStage.h tools.h writebackStage.c

loader.o: loader.h loader.c

clean:
	rm -f *.o
