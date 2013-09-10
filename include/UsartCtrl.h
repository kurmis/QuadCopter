#ifndef __USART_H
#define __USART_H

const int USART_SPEED= 115200;
bool RecievedData(int RX_Count) { return (RX_Count>0); };

class UsartCtrl
{
public:
	
	UsartCtrl(int inten);
	int GetCommand(volatile char * buffer, int size);

private:
	
	int Identify(volatile char* buffer, int size, int start, int stop);
	void Clear_Buffer(volatile char* buffer, int size);
	int compStr (char *s1, char *s2, int sz);
	void Init_Commands(void);
};

#endif
