CXX:=g++
CXXFLAGS:=-std=c++14 -Wall -MMD
EXEC:=WORLDJENNY
OBJ:=main.o worldgen.o
DEP:=${OBJ:.o=.d}

${EXEC}: ${OBJ}
	${CXX} ${CXXFLAGS} ${OBJ} -o ${EXEC}

-include ${DEP}

clean:
	rm ${OBJ} ${DEP} ${EXEC}

.PHONY: clean
