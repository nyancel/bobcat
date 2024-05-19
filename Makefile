build:
	gcc -c lib/**/*.c
	gcc -c lib/*.c
	gcc -c src/*.c
	mkdir -p ./obj
	mv ./*.o ./obj
	mkdir -p ./build
	gcc ./obj/*.o -o ./build/program

test: clean build
	clear
	./build/program

clean:
	rm -rf ./obj
	rm -rf ./build
