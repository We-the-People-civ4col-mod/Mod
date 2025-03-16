#include <string>

class GUI_python_interface;

struct GUI_Position
{
	GUI_Position();
	
	int x;
	int y;
	int width;
	int height;

	class GUI_Widget_base* parent;
};

class GUI_Widget_base
{
public:
	GUI_Widget_base(std::string name, GUI_python_interface& interface);
	virtual ~GUI_Widget_base();
	
	void show();
	void hide();

	virtual void draw() = 0;

	const std::string name();

protected:
	GUI_python_interface& m_interface;

private:
	std::string m_name;

	bool m_bShown;
	bool m_bDrawn;
	/*
	enum WidgetTypes m_eWidgetType;
	int m_iData1;
	int m_iData2;
	*/
};

class GUI_Panel : public GUI_Widget_base
{
public:
	GUI_Panel(std::string name, GUI_python_interface& interface);

	void draw();

private:
	int m_x;
	int m_y;
	int m_width;
	int m_height;
};

class GUI_ImageButton : public GUI_Widget_base
{
public:
	GUI_ImageButton(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Minimap : public GUI_Widget_base
{
public:
	GUI_Minimap(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Table : public GUI_Widget_base
{
public:
	GUI_Table(std::string name, GUI_python_interface& interface);

	void draw();

};

class GUI_Multilist : public GUI_Widget_base
{
public:
	GUI_Multilist(std::string name, GUI_python_interface& interface);

	void draw();

};

class GUI_MultiLineText : public GUI_Widget_base
{
public:
	GUI_MultiLineText(std::string name, GUI_python_interface& interface);

	void draw();

};

class GUI_Text : public GUI_Widget_base
{
public:
	GUI_Text(std::string name, GUI_python_interface& interface);

	void draw();

};

class GUI_Label : public GUI_Widget_base
{
public:
	GUI_Label(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Bar : public GUI_Widget_base
{
public:
	GUI_Bar(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Checkbox : public GUI_Widget_base
{
public:
	GUI_Checkbox(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Editbox : public GUI_Widget_base
{
public:
	GUI_Editbox(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Listbox : public GUI_Widget_base
{
public:
	GUI_Listbox(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Dropdownbox : public GUI_Widget_base
{
public:
	GUI_Dropdownbox(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_DDSGFC : public GUI_Widget_base
{
public:
	GUI_DDSGFC(std::string name, GUI_python_interface& interface);

	void draw();

};


class GUI_Line : public GUI_Widget_base
{
public:
	GUI_Line(std::string name, GUI_python_interface& interface);

	void draw();

};
