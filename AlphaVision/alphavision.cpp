#include "alphavision.h"

AlphaVision::AlphaVision(QObject *parent) :
    QObject(parent)
{

}

void AlphaVision::openSerial(QString port) {
	stdio.c_iflag=0;
	stdio.c_oflag=0;
	stdio.c_cflag=0;
	stdio.c_lflag=0;
	stdio.c_cc[VMIN]=1;
	stdio.c_cc[VTIME]=0;
	tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
//	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	this->tty_fd = open(port.toAscii(), O_RDWR | O_NONBLOCK);
	cfsetospeed(&tio, B9600);
	cfsetispeed(&tio, B9600);
	tcsetattr(this->tty_fd, TCSANOW, &tio);
}

void AlphaVision::sendTextToDisplay(QString text) {
	QByteArray message = generateHeader();
	message.append(text);
	parseForSpecialChars(&message);
	sendMessageOutSerial(message);
}

void AlphaVision::parseForSpecialChars(QByteArray *message) {
	message->replace("{C_RED}", QByteArray::fromHex("1C31"));	// RED
	message->replace("{C_GREEN}", QByteArray::fromHex("1C32"));	// GREEN
	message->replace("{C_AMBER}", QByteArray::fromHex("1C33"));	// AMBER
	message->replace("{C_RB1}", QByteArray::fromHex("1C39"));	// RAINBOW 1
	message->replace("{C_RB2}", QByteArray::fromHex("1C42"));	// RAINBOW 2

	message->replace("\n", "\r");

	message->replace("{BANNER}", QByteArray::fromHex("1B6161"));	// Scroll Right to Left
	message->replace("{HOLD}", QByteArray::fromHex("1B6262"));		// Hold in place
	message->replace("{FLASH}", QByteArray::fromHex("1B6363"));		// Flash Message
	message->replace("{ROLLUP}", QByteArray::fromHex("1B6565"));	// Roll up
	message->replace("{ROTATE}", QByteArray::fromHex("1B7474"));	// Real Rotate
}

QByteArray AlphaVision::generateHeader() {
	QByteArray header;
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("00"));
	header.append(QByteArray::fromHex("01"));	// Start Of header
	header.append(QByteArray::fromHex("3F"));	// All Signs
	header.append(QByteArray::fromHex("3F"));	// All Signs
	header.append(QByteArray::fromHex("30"));	// All Signs
	header.append(QByteArray::fromHex("30"));	// All Signs
	header.append(QByteArray::fromHex("02"));	// Start Of Text
	header.append(QByteArray::fromHex("41"));	// Write
	header.append(QByteArray::fromHex("41"));	// File A
	header.append(QByteArray::fromHex("1B"));	// Location
	header.append("0b");						// Center And Hold
	return header;
}

void AlphaVision::sendMessageOutSerial(QByteArray message) {
	message.append(QByteArray::fromHex("04"));
	int i;
	for (i=0; i<message.size(); i++) {
		sendChar(message.at(i));
		qDebug() << message.at(i);
		usleep(4000);
	}
}

void AlphaVision::sendChar(char c) {
	write(tty_fd, &c, 1);
}
