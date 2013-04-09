ifndef PVM_ROOT
	ERROR=1
endif
ifndef PVM_ARCH
	ERROR=1
endif
all: master slave

master: master.c
ifeq ($(ERROR),1)
	@echo "Variable PVM_ROOT or PVM_ARCH not set -> Not building master"
else
	gcc -o montecarlo_master master.c -I ${PVM_ROOT}/include -I /usr/pkg/include -L ${PVM_ROOT}/lib/${PVM_ARCH}/ -lm -lpvm3 -lgmp -L /usr/pkg/lib -Wl,-R ${PVM_ROOT}/lib/${PVM_ARCH}
endif

slave: slave.c
ifeq ($(ERROR),1)
	@echo "Variable PVM_ROOT or PVM_ARCH not set -> Not building slave"
else
	gcc -o montecarlo_slave slave.c -I ${PVM_ROOT}/include  -L ${PVM_ROOT}/lib/${PVM_ARCH}/ -lm -lpvm3 -DDEBUG -Wl,-R ${PVM_ROOT}/lib/${PVM_ARCH}
endif
	
install:
	cp montecarlo_slave ${PVM_ROOT}/bin/${PVM_ARCH}
	cp montecarlo_master ${PVM_ROOT}/bin/${PVM_ARCH}