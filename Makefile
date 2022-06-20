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

IMGUIDIR = lib/imgui

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

IMGGUI_SRC = imgui.cpp\
	imgui_demo.cpp\
	imgui_draw.cpp\
	imgui_impl_glfw.cpp\
	imgui_impl_opengl3.cpp\
	imgui_tables.cpp\
	imgui_widgets.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRC)) $(addprefix $(IMGUIDIR)/, $(IMGGUI_SRC))

CFLAGS = -std=c++17 #-Wall -Wextra -Werror
OUTDIR = out
OBJS = $(SRCS:.cpp=.o)
OUT = $(notdir $(OBJS))

INCL = -I include -I lib/imgui/include -I lib/GLFW -I lib/glm -I lib -I /usr/local/include

UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),intel)
	LIBGLFW = -lglfw3
	LIBGLEW = -lGLEW
endif
ifeq ($(UNAME_P),arm)
	LIBGLFW =  -lglfw3_arm64
	LIBGLEW = -lGLEW_arm64
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS =  -lglfw -lGL -lGLEW -lm -lXext -lX11
endif
ifeq ($(UNAME_S),Darwin)
	LDFLAGS =  -L lib $(LIBGLFW) -L lib $(LIBGLEW) -lm -framework OpenGL -framework OpenCL -framework Cocoa -framework IOKit -framework CoreVideo
endif

CC = clang++

all: $(NAME)

$(NAME): $(OBJS)
	@printf "Architecture: %s Platform: %s\n" "$(UNAME_S)" "$(UNAME_P)"
	@printf "compiling \n" "$(NAME)"
	@$(CC) $(CFLAGS) $(INCL) $(addprefix $(OUTDIR)/, $(OUT)) $(LDFLAGS) -o $(NAME) -O3

%.o: %.cpp
	@mkdir -p out
	@printf "compiling %s -> %s\n" "$<" "$@"
	@$(CC) $(CFLAGS) $(INCL) -c $< -o $(addprefix $(OUTDIR)/, $(notdir $@)) -O3


debug:
	$(CC) -g $(CFLAGS) $(INCL) $(SRCS) $(LDFLAGS) -o $(NAME)

clean:
	@rm -rf $(addprefix $(OUTDIR)/, $(OUT))
	@rm -Rf $(OUTDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
