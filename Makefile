all: server client
server: bin/sdstored
client: bin/sdstore

.PHONY: clean
clean:
	rm obj/common/* obj/server/* obj/client/* bin/{sdstore,sdstored}


CC=gcc
FLAGS=-O3 -Wall --pedantic-errors -g #Remove -g for release

COMMON_SRC = $(wildcard common/src/*.c)
SERVER_SRC = $(wildcard server/src/*.c)
CLIENT_SRC = $(wildcard client/src/*.c)

COMMON_OBJS = ${COMMON_SRC:common/src/%.c=obj/common/%.o}
SERVER_OBJS = ${SERVER_SRC:server/src/%.c=obj/server/%.o}
CLIENT_OBJS = ${CLIENT_SRC:client/src/%.c=obj/client/%.o}


obj/common/%.o: common/src/%.c common/include/*.h
	mkdir -p $(dir $@)
	${CC} ${FLAGS} -c -o $@ $< -Icommon/include

obj/server/%.o: server/src/%.c common/include/*.h server/include/*.h
	mkdir -p $(dir $@)
	${CC} ${FLAGS} -c -o $@ $< -Icommon/include -Iserver/include

obj/client/%.o: client/src/%.c common/include/*.h client/include/*.h
	mkdir -p $(dir $@)
	${CC} ${FLAGS} -c -o $@ $< -Icommon/include -Iclient/include


bin/sdstored: ${COMMON_OBJS} ${SERVER_OBJS}
	mkdir -p $(dir $@)
	${CC} ${FLAGS} -o $@ $^

bin/sdstore: ${COMMON_OBJS} ${CLIENT_OBJS}
	mkdir -p $(dir $@)
	${CC} ${FLAGS} -o $@ $^