# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: wkorande <willehard@gmail.com>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/06/01 15:48:04 by rjaakonm          #+#    #+#              #
#    Updated: 2022/06/20 17:18:34 by wkorande         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = particle-system

SRCDIR = src

SRC = Application.cpp\
	CLContext.cpp\
	CLKernel.cpp\
	CLProgram.cpp\
	Camera.cpp\
	GLContext.cpp\
	GLObject.cpp\
	GLRenderer.cpp\
	GUIContext.cpp\
	ParticleSystem.cpp\
	Shader.cpp\
	main.cpp

IMGGUI_SRC = lib/imgui/imgui.cpp\
	lib/imgui/imgui_demo.cpp\
	lib/imgui/imgui_draw.cpp\
	lib/imgui/imgui_impl_glfw.cpp\
	lib/imgui/imgui_impl_opengl3.cpp\
	lib/imgui/imgui_tables.cpp\
	lib/imgui/imgui_widgets.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRC)) $(IMGGUI_SRC)

CFLAGS = -std=c++17 #-Wall -Wextra -Werror

OBJS = $(SRCS:.cpp=.o)

INCL = -I include -I lib/imgui/include -I lib/GLFW -I lib/glm -I lib -I /usr/local/include

UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LDFLAGS =  -lglfw -lGL -lGLEW -lm -lXext -lX11
	endif
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS =  -L lib -lglfw3 -L lib -lGLEW -lm -framework OpenGL -framework OpenCL -framework Cocoa -framework IOKit -framework CoreVideo
	endif

CC = clang++

vpath %.cpp $(SRCDIR)

all: $(NAME)

$(NAME): $(OBJS)
	@printf "compiling %s\n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(OBJS) $(LDFLAGS) -o $(NAME) -O3

%.o: %.cpp
	@printf "compiling %s\n" "$<"
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $@ -O3


debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
