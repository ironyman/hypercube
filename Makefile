TARGET=hypercube

all: $(TARGET)
$(TARGET): hypercube.c matrix.c matrix5.c matrix5.h
	gcc $^ -lglut -lGL -lm -lGLEW -g -o $@
run: all
	./$(TARGET)
gdb: all
	gdb ./$(TARGET)
valgrind: all
	valgrind ./$(TARGET)
vgdb: all
	#urxvt -e 'bash' '-c' 'sleep 0.5 && gdb -ex target remote | vgdb'
	valgrind --vgdb-error=0 ./$(TARGET)
