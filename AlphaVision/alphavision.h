#ifndef ALPHAVISION_H
#define ALPHAVISION_H

#include <QObject>
#include <QDebug>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

class AlphaVision : public QObject
{
    Q_OBJECT
public:
    explicit AlphaVision(QObject *parent = 0);
	void openSerial(QString port);
	void sendTextToDisplay(QString text);

private:
	QByteArray generateHeader();
	void parseForSpecialChars(QByteArray *message);
	void sendChar(char c);
	void sendMessageOutSerial(QByteArray message);
	struct termios tio;
	struct termios stdio;
	int tty_fd;
	fd_set rdset;

signals:

public slots:

};

#endif // ALPHAVISION_H
