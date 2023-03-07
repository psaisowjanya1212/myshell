mytoolkit: mytoolkit.c
	gcc -Wall -Wpedantic -std=c99 -g mytoolkit.c -o mytoolkit.x

mytimeout: mytimeout.c
	gcc -Wall -Wpedantic -g mytimeout.c -o mytimeout.x

mytree: mytree.c
	gcc -Wall -Wpedantic -g mytree.c -o mytree.x

mytime: mytime.c
	gcc -Wall -Wpedantic -g mytime.c -o mytime.x

mymtimes: mymtimes.c
	gcc -Wall -Wpedantic -g mymtimes.c -o mymtimes.x

clean:
	rm -rf *.out *.o mytoolkit.x mytimeout.x

