/*
 * Copyright (C) 2015 Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * Source: https://github.com/cbries/rgbwall
 * License: MIT
 */

/*
 * Build: $ g++ -O3 -o transmitrgb transmitrgb.cpp -std=c++0x
 * Usage: $ transmitrgb /dev/ttyACM0 -x 00 -y 00 -r 23 -g 25 -b 65
 *
 * [Output]
 *  pi@raspbmc:~/development$ ./transmitrgb /dev/ttyACM0 -x 00 -y 00 -r 23 -g 45 -b 56
 *  Device opened: /dev/ttyACM0
 *  Trying: 0000023045056
 *  Transmitted bytes: 14
 *  Trying: 0000023045056
 *  Transmitted bytes: 14
 *  Trying: 0000023045056
 *  Transmitted bytes: 14
 *  Result: OKData transmitted: x: 00, y: 00, RGB(023, 045, 056)
 *  pi@raspbmc:~/development$
 * [/Output]
 */

#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

struct Data
{
	Data() {
		devname = "/dev/ttyACM0";
		x = 0; y = 0;
		r = 0; g = 0; b = 0; 
	}
	std::string devname;
	uint8_t x, y;
	uint8_t r, g, b;
	std::string toString() {
		char buf[64] = { '\0' };
		snprintf(buf, 64, "x: %02d, y: %02d, RGB(%03d, %03d, %03d)"
		   , x, y, r, g, b);
		return std::string(buf);
	}
};

bool parseOptions(int argc, char **argv, Data & data)
{
	data.devname = argv[1];	

	for(int i=2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-x"))      { data.x = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-y")) { data.y = atoi(argv[i+1]); } 
		else if(!strcmp(argv[i], "-r")) { data.r = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-g")) { data.g = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-b")) { data.b = atoi(argv[i+1]); }
		else { }
	}
	return true;
}

void showUsage()
{
	std::cout << "Usage: transmitgrb DEVNAME -x INT -y INT -r INT -g INT -b INT" << std::endl;
}

/**
 * \see http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 */
namespace Extern
{
int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                std::cerr << "Error from tcgetattr: " << errno << std::endl;
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                std::cerr << "Error from tcsetattr: " << errno << std::endl;
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                std::cerr << "Error from tggetattr: " << errno << std::endl;
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                std::cerr << "Error setting term attributes: " << errno << std::endl;
}
} // namespace Extern

/**
 *
 */
int main(int argc, char **argv)
{
	if(argc != 12)
	{
		showUsage();
		return 1;
	} 

	Data data;
	bool res = parseOptions(argc, argv, data); 
	if(res == false) {
		showUsage();
		return 1;
	}

	int fd = open(data.devname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if(fd == -1) { return 2; }	

	Extern::set_interface_attribs(fd, B115200, 0);
	Extern::set_blocking (fd, 0);

	std::cout << "Device opened: " << data.devname.c_str() << std::endl;

	fd_set rfds;
	struct timeval tv;

	bool ok = false;
	int tryCounter = 0;
	while(!ok && tryCounter < 10) 
	{
		char buf[32] = { '\0' };
		snprintf(buf, 32, "%02d%02d%03d%03d%03d\n", 
			data.x, data.y, data.r, data.g, data.b);
		++tryCounter;
		std::cout << "Trying: " << buf;
		ssize_t n = write(fd, buf, strlen(buf));
		usleep ((n + 25) * 100);
		std::cout << "Transmitted bytes: " << n << std::endl;
		if(n < 0) { continue; }

		FD_ZERO(&rfds);
        	FD_SET(fd, &rfds);

		tv.tv_sec = 1;
           	tv.tv_usec = 0;
	
		int retval = select(fd+1, &rfds, NULL, NULL, &tv);
		if (retval == -1) perror("select() for reading");
		else if(retval)
		{
			n = read(fd, buf, 32);
			buf[n] = '\0';
			ok = !strcmp(buf, "OK") || !strcmp(buf, "OK\n");
			if(ok)
			{
				std::cout << "Result: " << buf;
			}
		}
		else
		{
			std::cout << "We should try it again!" << std::endl;
		}
	}

	std::cout << "Data transmitted: " << data.toString() << std::endl;

	return 0;
}

