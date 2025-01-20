######################## SETTINGS ########################

NAME = ircserv

CC = c++

FLAGS = #-Wall -Wextra -Werror # errors
FLAGS += -std=c++98 # c++ version 
FLAGS += #-Wshadow #-pedantic # syntax helper (can remove)
FLAGS += -g3 # opti
# FLAGS += -fsanitize=address,leak,undefined #sanitize

INCLUDES =	includes

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes 

######################## SOURCES ########################

SERVER = Server.cpp handle_client.cpp commands.cpp utils.cpp

CLIENT =	Client.cpp

CHANNEL =	Channel.cpp

UTILS = utils.cpp
	
SRCS_NAMES =	main.cpp \
				${addprefix server/, ${SERVER}} \
				${addprefix client/, ${CLIENT}} \
				${addprefix channel/, ${CHANNEL}} \
				${addprefix utils/, ${UTILS}}

SRCS_DIR = srcs/

SRCS = ${addprefix ${SRCS_DIR}, ${SRCS_NAMES}}

OBJS_DIR = objs/

OBJS = ${addprefix ${OBJS_DIR}, ${SRCS_NAMES:.cpp=.o}}

#NOTE: allows dependency during compilation, .o will recompile on change
HEADERS = includes/Client.hpp includes/ft_irc.hpp includes/Server.hpp includes/Channel.hpp

######################## BASIC RULES ########################

#TODO: add phony

all : 
	${MAKE} -j ${NAME}

re : fclean
	${MAKE} all

clean :
	rm -rf ${OBJS_DIR}

fclean : clean
	rm -f ${NAME}

######################## COMPILATION ########################

${NAME} : ${OBJS_DIR} ${OBJS}
	${foreach lib, ${LIBS}, ${MAKE} -C ${lib}}
	${CC} ${FLAGS} ${OBJS} ${foreach lib, ${LIBS},${lib}/${lib}.a} -o $@ ${EXTRAFLAGS}

${OBJS_DIR} :
	mkdir $@
	mkdir $@client
	mkdir $@server
	mkdir $@channel
	mkdir $@utils

${OBJS_DIR}%.o : ${SRCS_DIR}%.cpp
	${CC} ${FLAGS} ${EXTRAFLAGS} ${CPPFLAGS} ${foreach include, ${INCLUDES},-I ${include}} -c $< -o $@

######################## TEST ########################

test : base

base : all
	./${NAME}

err : all
	./${NAME} 2>/dev/null

leak : all
	${VALGRIND} ./${NAME}
