CXX = clang++
CXXFLAGS = -O2 -Wall -Werror -std=c++11
LDFLAGS = -lsfml-audio -lsfml-system
OBJS = play.o SoundPool.o TechnikaSong.o

all: play

play: $(OBJS)
	$(CXX) -o play $(LDFLAGS) $(OBJS)

clean:
	rm -rf *.o
	rm -rf play

