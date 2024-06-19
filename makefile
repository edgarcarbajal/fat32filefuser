CXXFLAGS+=-std=c++17 -Wall -Wextra -Werror -pedantic -I.

all: fat32file_fuser

# ($^) brings all listed files after the colon as arguments for the g++ cmd
fat32file_fuser: fat32file_fuser.cc
	g++ $(CXXFLAGS) -o fat32file_fuser $^


# removes any object files and executables when called
clean:
	rm -f fat32file_fuser *.o 