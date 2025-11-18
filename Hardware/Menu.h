#ifndef __MENU_H_
#define __MENU_H_

struct option_class{
	char Name[16];
	void (*func)(void);
};

void Main_Menu(void);
void LED_Control(void);
void PID(void);
void Image(void);
void Angle(void);

#endif
