NAME = particle-system

SRC = shader.cpp\
	entity.cpp\
	camera.cpp\
	renderer.cpp\
	application.cpp\
	input.cpp\
	obj_loader.cpp\
	mesh.cpp\
	collider.cpp\
	framebuffer.cpp\
	textured_quad.cpp\
	texture.cpp\
	bitmap_font.cpp\
	main.cpp

SRCDIR = src

OBJDIR = build

INCLUDE = include

SRCS = $(addprefix $(SRCDIR)/, $(SRC))

OBJ = $(SRC:.cpp=.o)

OBJS = $(addprefix $(OBJDIR)/, $(OBJ))

CC = clang

CFLAGS = -std=c++17 -lstdc++ #-Wall -Wextra -Werror
LDFLAGS = -lglfw -lGL -lGLEW -lm -ldl -lXrandr -lXi -lX11 -lXxf86vm -lpthread

vpath %.cpp $(SRCDIR)
vpath %.h $(INCLUDE)

all: $(NAME)

$(NAME): $(OBJ)
	@printf "compiling %s\n" "$(NAME)"
	@$(CC) $(CFLAGS) -o $(NAME) $(OBJS) -I $(INCLUDE) $(LDFLAGS)
	@printf "all done\n"

%.o: %.cpp
	@mkdir -p build
	@printf "compiling %s..." "$<"
	@$(CC) -I $(INCLUDE) -c $< -o $(OBJDIR)/$@
	@printf "done\n"

debug:
	$(CC) -g $(CFLAGS) -o $(NAME) $(SRCS) -I $(INCLUDE) $(LDFLAGS) 

check: fclean
	$(CC) $(CFLAGS) -o $(NAME) $(SRCS) -I $(INCLUDE) $(LDFLAGS) CXX=~/Dev/temp/build/bin/include-what-you-use

clean:
	@echo "removing objects"
	@rm -rf $(OBJS)

fclean: clean
	@echo "removing $(NAME)"
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
