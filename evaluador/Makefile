
LDFLAGS=-lrt -pthread
SRCDIR=src
${SRCDIR}/evaluator: ${SRCDIR}/main.o ${SRCDIR}/init.o ${SRCDIR}/reg.o ${SRCDIR}/ctrl.o ${SRCDIR}/rep.o ${SRCDIR}/stop.o ${SRCDIR}/procesando.o
	$(CXX) -o $@ $^ $(LDFLAGS)

${SRCDIR}/main.o: ${SRCDIR}/main.cpp ${SRCDIR}/init.h ${SRCDIR}/reg.h ${SRCDIR}/ctrl.h ${SRCDIR}/rep.h

${SRCDIR}/init.o: ${SRCDIR}/init.cpp ${SRCDIR}/init.h

${SRCDIR}/reg.o: ${SRCDIR}/reg.cpp ${SRCDIR}/reg.h

${SRCDIR}/ctrl.o: ${SRCDIR}/ctrl.cpp ${SRCDIR}/ctrl.h

${SRCDIR}/rep.o: ${SRCDIR}/rep.cpp ${SRCDIR}/rep.h

${SRCDIR}/stop.o: ${SRCDIR}/stop.cpp ${SRCDIR}/stop.h

${SRCDIR}/procesando.o: ${SRCDIR}/procesando.cpp ${SRCDIR}/procesando.h 

clean:
	rm -f ${SRCDIR}/*.o
	rm -f ${SRCDIR}/*.*~ *~
	rm -f ${SRCDIR}/evaluator
