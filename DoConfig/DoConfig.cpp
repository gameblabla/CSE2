/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The F*** You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */

#include "FL/Fl.H"
#include "FL/Fl_Check_Button.H"
#include "FL/Fl_Choice.H"
#include "FL/Fl_Radio_Round_Button.H"
#include "FL/Fl_Window.H"
#include <FL/Enumerations.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Round_Button.H>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

#define MAGIC "CSE2E   20191222"
#define FONT "Courier New"

struct data
{
	char magic[32];
	char font[64];
	unsigned char move[4];
	unsigned char attack[4];
	unsigned char okay[4];
	unsigned char display[4];
	unsigned char useJoy[4];
	unsigned char buttons[8][4];
	unsigned char fps[4];
	unsigned char vsync[4];
};

class RadioRow
{
public:
	RadioRow(char offset);
	int value();
	void value(int input);

private:
	Fl_Group *group;
	Fl_Radio_Round_Button *buttons[6];
};

static char config_path[FILENAME_MAX];

static data config = {MAGIC, FONT};

static unsigned long CharsToLong(unsigned char *chars)
{
	return (chars[3] << 24) | (chars[2] << 16) | (chars[1] << 8) | chars[0];
}

static void LongToChars(unsigned long long_var, unsigned char *chars)
{
	chars[0] = long_var & 0xFF;
	chars[1] = (long_var >> 8) & 0xFF;
	chars[2] = (long_var >> 16) & 0xFF;
	chars[3] = (long_var >> 24) & 0xFF;
}

RadioRow::RadioRow(char offset)
{
	char *temp = new char[2];
	temp[0] = '1' + offset;
	temp[1] = '\0';
	this->group = new Fl_Group(140 + offset * 30, 180, 30, 180);
	this->group->label(temp);
	this->group->align(FL_ALIGN_TOP_LEFT);
	for (int i = 0; i < 6; i++)
		this->buttons[i] = new Fl_Radio_Round_Button(140 + offset * 30, 180 + 30 * i, 30, 30);
	this->group->end();
}

int RadioRow::value()
{
	for (int i = 0; i < 6; i++)
		if (this->buttons[i]->value())
			return i;
	return 0;
}

void RadioRow::value(int input)
{
	this->buttons[input]->setonly();
}

Fl_Round_Button *movear;
Fl_Round_Button *movegt;

Fl_Round_Button *buttonxz;
Fl_Round_Button *buttonzx;

Fl_Round_Button *okayjump;
Fl_Round_Button *okayattack;

Fl_Choice *displaychoice;
Fl_Choice *frameratechoice;
Fl_Check_Button *vsyncchoice;
Fl_Check_Button *joychoice;

Fl_Group *joystuffcontainer;
RadioRow *joyRows[8];

void quit(Fl_Widget *, void *)
{
	std::exit(0);
}

void activatejoy(Fl_Widget *, void *)
{
	if (joystuffcontainer->active())
		joystuffcontainer->deactivate();
	else
		joystuffcontainer->activate();
}

void read_Config()
{
	std::fstream fd;
	fd.open(config_path, std::ios::in | std::ios::binary);
	fd.read((char *)&config, sizeof(config));
	fd.close();
	strcpy(config.magic, MAGIC);
	CharsToLong(config.move) ? movegt->setonly() : movear->setonly();
	CharsToLong(config.attack) ? buttonzx->setonly() : buttonxz->setonly();
	CharsToLong(config.okay) ? okayattack->setonly() : okayjump->setonly();

	displaychoice->value(CharsToLong(config.display));
	frameratechoice->value(CharsToLong(config.fps));
	vsyncchoice->value(CharsToLong(config.vsync));
	joychoice->value(CharsToLong(config.useJoy));

	if (!CharsToLong(config.useJoy))
		joystuffcontainer->deactivate();

	for (int i = 0; i < 8; i++)
	{
		const unsigned long button = CharsToLong(config.buttons[i]);
		if (button < 6 && button > 0)
		{
			const unsigned int button_lookup[6] = {0, 1, 2, 4, 5, 3};
			joyRows[i]->value(button_lookup[button - 1]);
		}
	}
}

void write_Config(Fl_Widget *, void *)
{
	LongToChars(movegt->value(), config.move);
	LongToChars(buttonzx->value(), config.attack);
	LongToChars(okayattack->value(), config.okay);

	LongToChars(displaychoice->value(), config.display);
	LongToChars(frameratechoice->value(), config.fps);
	LongToChars(vsyncchoice->value(), config.vsync);
	LongToChars(joychoice->value(), config.useJoy);
	for (int i = 0; i < 8; i++)
	{
		const unsigned int button_lookup[6] = {0, 1, 2, 5, 3, 4};
		LongToChars(button_lookup[joyRows[i]->value()] + 1, config.buttons[i]);
	}
	std::fstream fd;
	fd.open(config_path, std::ios::out | std::ios::binary);
	fd.write((char *)&config, sizeof(config));
	fd.close();
	exit(0);
}

int main(int argc, char *argv[])
{
	strcpy(config_path, argv[0]);

	for (size_t i = strlen(config_path);; --i)
	{
		if (i == 0)
		{
			strcpy(config_path, "Config.dat");
			break;
		}
		else if (config_path[i] == '\\' || config_path[i] == '/')
		{
			config_path[i] = '\0';
			strcat(config_path, "/Config.dat");
			break;
		}
	}

	Fl_Window *mainw = new Fl_Window(400, 410, "DoConfig - Doukutsu Monogatari Settings");

	Fl_Group *movegroup = new Fl_Group(10, 10, 185, 50);
	movegroup->box(FL_THIN_DOWN_BOX);
	movear = new Fl_Radio_Round_Button(10, 10, 185, 20, "Arrows for Movement");
	movear->setonly();
	movegt = new Fl_Radio_Round_Button(10, 40, 185, 20, "<>? for Movement");
	movegroup->end();

	Fl_Group *buttongroup = new Fl_Group(10, 70, 185, 50);
	buttongroup->box(FL_THIN_DOWN_BOX);
	buttonxz = new Fl_Radio_Round_Button(10, 70, 185, 20, "Z=Jump; X=Attack");
	buttonxz->setonly();
	buttonzx = new Fl_Radio_Round_Button(10, 100, 185, 20, "X=Jump; Z=Attack");
	buttongroup->end();

	Fl_Group *okaygroup = new Fl_Group(205, 10, 185, 50);
	okaygroup->box(FL_THIN_DOWN_BOX);
	okayjump = new Fl_Radio_Round_Button(205, 10, 185, 20, "Jump=Okay");
	okayjump->setonly();
	okayattack = new Fl_Radio_Round_Button(205, 40, 185, 20, "Attack=Okay");
	okaygroup->end();

	displaychoice = new Fl_Choice(205, 70, 185, 20);
	Fl_Menu_Item screens[] = {
		{"Fullscreen"},
		{"Windowed 426x240"},
		{"Windowed 852x480"},
		{"Windowed 1278x720"},
		{"Windowed 1704x960"},
		{0}};
	displaychoice->menu(screens);

	frameratechoice = new Fl_Choice(205, 100, 185, 20);
	Fl_Menu_Item screens2[] = {
		{"50FPS"},
		{"60FPS"},
		{0}};
	frameratechoice->menu(screens2);

	vsyncchoice = new Fl_Check_Button(205, 130, 185, 20, "V-sync");

	joychoice = new Fl_Check_Button(10, 130, 185, 20, "Use Joypad");
	joychoice->callback(&activatejoy);

	joystuffcontainer = new Fl_Group(10, 160, 380, 200);
	joystuffcontainer->box(FL_THIN_DOWN_BOX);
	for (int i = 0; i < 8; i++)
	{
		joyRows[i] = new RadioRow(i);
	}

	// There's no Label class alright? I'll switch it as soon as one is introduced.
	Fl_Group *labeljump = new Fl_Group(10, 180, 10, 20);
	labeljump->label("Jump:");
	labeljump->align(FL_ALIGN_RIGHT);
	labeljump->end();

	Fl_Group *labelattack = new Fl_Group(10, 210, 10, 20);
	labelattack->label("Attack:");
	labelattack->align(FL_ALIGN_RIGHT);
	labelattack->end();

	Fl_Group *labelweaponup = new Fl_Group(10, 240, 10, 20);
	labelweaponup->label("Weapon+:");
	labelweaponup->align(FL_ALIGN_RIGHT);
	labelweaponup->end();

	Fl_Group *labelweapondown = new Fl_Group(10, 270, 10, 20);
	labelweapondown->label("Weapon-:");
	labelweapondown->align(FL_ALIGN_RIGHT);
	labelweapondown->end();

	Fl_Group *labelitem = new Fl_Group(10, 300, 10, 20);
	labelitem->label("Items:");
	labelitem->align(FL_ALIGN_RIGHT);
	labelitem->end();

	Fl_Group *labelmap = new Fl_Group(10, 330, 10, 20);
	labelmap->label("Map:");
	labelmap->align(FL_ALIGN_RIGHT);
	labelmap->end();

	joystuffcontainer->end();

	Fl_Button *okaybutton = new Fl_Button(10, 370, 185, 30, "Okay");
	okaybutton->callback(&write_Config);

	Fl_Button *cancelbutton = new Fl_Button(205, 370, 185, 30, "Cancel");
	cancelbutton->callback(&quit);

	mainw->end();
	mainw->show(argc, argv);

	read_Config();
	Fl::option(Fl::OPTION_VISIBLE_FOCUS, false);
	return Fl::run();
}
