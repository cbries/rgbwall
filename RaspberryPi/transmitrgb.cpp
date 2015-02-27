/*
 * Copyright (C) 2015 Christian Benjamin Ries
 * Website: www.christianbenjaminries.de
 * Source: https://github.com/cbries/rgbwall
 * License: MIT
 */

/*
 * Build: $ g++ -O3 -o transmitrgb transmitrgb.cpp -std=c++0x
 * Usage: $ transmitrgb /dev/ttyACM0 -x 00 -y 00 -r 23 -g 25 -b 65 [-ts 0 -tms 500]
 *
 *  IMPORTANT:
 * ###################
 * In case this tool is used in combination with an Arduino UNO keep in mind
 * that any new connection will reset/reboot the Microcontroller (i.e. Setup() is called).
 * A more detailed explanation about this behaviour can be found on the official
 * Arduino website: http://arduino.cc/en/Main/arduinoBoardUno (Paragraph: Automatic (Software) Reset).
 * For avoiding this some tricks/hacks are tried by people in the web. In my case I tried
 * several ways but only one works.
 * [Cite, http://forum.arduino.cc/index.php?topic=22974.msg173615#msg173615]
 *   [..] a 47 ohm resistor between 3.3v and reset seems to prevent serial resets.  
 *   It's a more common value and leaves the 5v pin open.  But if you hit the reset switch it it will 
 *   draw 70 ma from the ftdi 3.3V port while the switch is closed, and the datasheet lists 50ma max.
 *   I tapped my reset button a few times with this configuration and the magic smoke stayed put, and 
 *   upload still worked when I pulled out the resistor, but I can't really recommend the 47 ohm if 
 *   you are planning on using the reset switch.
 * [/Cite]
 * ###################
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
 *
 * for-Loop for X:
 *  $ for x in `seq 0 19` ; do  ./transmitrgb /dev/ttyACM0 -x $x -y 5 -r 255 -g 000 -b 255; done
 */

#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>
#include <limits.h>

struct Data
{
	Data() {
		devname = "/dev/ttyACM0";
		x = 0; y = 0;
		r = 0; g = 0; b = 0; 
		timeoutSecs = 1;
		timeoutMsecs = 0;
		endless = false;
		dobrightness = false;
		brightness = 15;
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
	uint8_t timeoutSecs;
	uint8_t timeoutMsecs;
	bool endless;
	bool dobrightness;
	int brightness;
};

bool parseOptions(int argc, char **argv, Data & data)
{
	data.devname = argv[1];	

	for(int i=2; i < argc; ++i)
	{
		if(!strcmp(argv[i], "-x"))        { data.x = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-y"))   { data.y = atoi(argv[i+1]); } 
		else if(!strcmp(argv[i], "-r"))   { data.r = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-g"))   { data.g = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-b"))   { data.b = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-ts"))  { data.timeoutSecs = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-tms")) { data.timeoutMsecs = atoi(argv[i+1]); }
		else if(!strcmp(argv[i], "-t"))   { data.endless = true; }
		else if(!strcmp(argv[i], "-br"))  { data.dobrightness = true; data.brightness = atoi(argv[i+1]); }
		else { }
	}
	return true;
}

void showUsage()
{
	std::cout << "Usage: transmitgrb DEVNAME -x INT -y INT -r INT -g INT -b INT" << std::endl;
	std::cout << " Optional: -ts UINT8 -tms UINT8  (for setting a timeout)" << std::endl; 
	std::cout << "           -t  (endless loop for setting the value)" << std::endl;

	std::cout << "Usage: transmitrgb DEVNAME -br 0-255" << std::endl;
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

void set_dtr(int fd, bool state)
{
	int res = 0;
	int iFlags;
	if(state)
	{
		iFlags = TIOCM_DTR;
		res = ioctl(fd, TIOCMBIS, &iFlags);
	}
	else
	{
		iFlags = TIOCM_DTR;
		res = ioctl(fd, TIOCMBIC, &iFlags);
	}
	if(res == -1)
	{
		perror("ioctl() for dtr failed");
	}
}

bool get_dtr(int fd)
{
	int serial;
	ioctl(fd, TIOCMGET, &serial);
	return (serial & TIOCM_DTR);
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

/** \see http://www.cplusplus.com/faq/sequences/strings/trim/
 */
namespace Extern
{
std::string& trimRight(
  std::string&       s,
  const std::string& delimiters = " \f\n\r\t\v" )
{
  return s.erase( s.find_last_not_of( delimiters ) + 1 );
}

std::string& trimLeft(
  std::string&       s,
  const std::string& delimiters = " \f\n\r\t\v" )
{
  return s.erase( 0, s.find_first_not_of( delimiters ) );
}

std::string& trim(
  std::string&       s,
  const std::string& delimiters = " \f\n\r\t\v" )
{
  return trimLeft( trimRight( s, delimiters ), delimiters );
}
} // namespace Extern

/**
 *
 */
int main(int argc, char **argv)
{
	if(argc != 4)
	{
		if(argc < 12)
		{
			showUsage();
			return 1;
		}
	}

	Data data;
	bool res = parseOptions(argc, argv, data); 
	if(res == false) {
		showUsage();
		return 1;
	}

	int fd = open(data.devname.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if(fd == -1) { perror("open() failed"); return 2; }	

	Extern::set_interface_attribs(fd, B115200, 0);
	Extern::set_blocking(fd, 0);

	std::cout << "Device opened: " << data.devname.c_str() << std::endl;

	fd_set rfds;
	struct timeval tv;

	bool ok = false;
	int maxTries = (data.endless ? 1024 : 50);
	int tryCounter = 0;
	while(!ok && tryCounter < maxTries) 
	{
		#define MAXLEN 128
		char buf[MAXLEN] = { '\0' };

		if(data.dobrightness)
		{
			snprintf(buf, MAXLEN, "b%03d\n", data.brightness);
		}
		else
		{
			snprintf(buf, MAXLEN, "%02d%02d%03d%03d%03d\n", 
				data.x, data.y, data.r, data.g, data.b);
		}

		ssize_t n = write(fd, buf, strlen(buf));
		if(buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';
		std::cout << tryCounter << " -> Trying: " << buf;
		usleep ((n + 25) * 100);
		std::cout << ", Bytes: " << n << std::endl;
		if(n < 0) { perror("write()"); continue; }
		++tryCounter;

		FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

		tv.tv_sec = data.timeoutSecs;
		tv.tv_usec = data.timeoutMsecs;
	
		int retval = select(fd+1, &rfds, NULL, NULL, &tv);
		if (retval == -1) perror("select() for reading");
		else if(retval)
		{
			buf[0] = '\0';
			n = read(fd, buf, MAXLEN);
			buf[n] = '\0';
			std::string sbuf(buf);
			Extern::trim(sbuf);
			if(n != 0)
			{
				std::size_t found = sbuf.find("OK");
				ok = found != std::string::npos;
			}
			if(ok)
			{
				buf[n-2] = '\0';
				std::cout << "Result: " << buf << std::endl;
			}
		}
		else
		{
			//std::cout << "We should try it again!" << std::endl;
		}
	}

	if(!data.dobrightness) {
		std::cout << "Data transmitted: " << data.toString() << std::endl;
	}

	return 0;
}

