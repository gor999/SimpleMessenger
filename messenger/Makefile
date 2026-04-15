CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -Iinclude -fPIC $(shell pkg-config --cflags Qt5Widgets)
LIBS = $(shell pkg-config --libs Qt5Widgets)

SRC = src/HttpUtils.cpp src/MessengerServer.cpp src/MainWindow.cpp main.cpp
OBJ = $(SRC:.cpp=.o)
MOC_SRC = include/MessengerServer.hpp include/MainWindow.hpp
MOC_OBJ = $(MOC_SRC:include/%.hpp=src/moc_%.cpp)
MOC_OBJ_FILES = $(MOC_OBJ:.cpp=.o)

NAME = messenger

all: $(MOC_OBJ) $(NAME)

$(NAME): $(OBJ) $(MOC_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ) $(MOC_OBJ_FILES) -o $(NAME) $(LIBS)

src/moc_%.cpp: include/%.hpp
	moc $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(MOC_OBJ) $(MOC_OBJ_FILES)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re