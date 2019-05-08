/*
End year project, 11th grade.

Project: Forward and Reverse Kinematics
Date: 20.06.2016
Author: Dima Claudiu
Grade: TBD
Time Taken: ~6 hours over 3 days

External dependencies: Graphics module of SFML 2.3.2 for Visual C++ 14 (2015) - 32-bit.
*/
#include <SFML/Graphics.hpp>
#include <cmath>


using namespace sf;
using namespace std;

#define width 1000
#define height 800

#define pi 3.141592653589793238


RenderWindow window(VideoMode(width, height), "");


class Arm
{
protected:
	float x, y, length, angle;
	Vertex line[2];

public:
	Arm(float x = 0, float y = 0, float length = 0, float angle = 0) //Basic constructor.
	{
		this->x = x;
		this->y = y;
		this->length = length;
		this->angle = angle;

	}

	void create(float x = 0, float y = 0, float length = 0, float angle = 0) //To be called when creating individual arms for a system.
	{
		this->x = x;
		this->y = y;
		this->length = length;
		this->angle = angle;

	}

	//Basic get functions.
	float getX() const
	{
		return this->x;
	}

	float getY() const
	{
		return this->y;
	}

	float getAngle() const
	{
		return this->angle;
	}

	//Since we have it's initial x, length and angle, we can easily calculate it's end x with this formula.
	float getEndX() const
	{
		return this->x + cos(this->angle) * this->length;
	}

	//Same for y.
	float getEndY() const
	{
		return this->y + sin(this->angle) * this->length;
	}

	void setPosition(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	void rotate(float angle)
	{
		this->angle += angle;
	}

	//We can find the angle between two points relative to the window by applying atan of the ratio between the legs of the right triangle formed with the distance between them as hypotenuse.
	void pointAt(Vector2f p)
	{
		this->angle = atan2(p.y - this->y, p.x - this->x);
	}

	/*
	In order to drag the arm first we need to rotate it(point at it's destination).
	Then we want it's endX and endY to be the targets x and y, so in order to do that we have to calculate the arms x and y.
	It's x relative to the target is cos(angle) * the arms length, wich we subtract from the targets position to calculate the absolute value.
	Same with y but sin(angle).
	*/
	virtual void drag(Vector2f p)
	{
		pointAt(p);
		this->x = p.x - cos(this->angle) * this->length;
		this->y = p.y - sin(this->angle) * this->length;
	}

	//Simply drawing a line between A(x,y) and B(endX,endY)
	virtual void render() 
	{
		line[0].position = Vector2f(this->x, this->y);
		line[1].position = Vector2f(getEndX(), getEndY());
		
		window.draw(line, 2, Lines);
	}
};


/*
This class represents a vector of arms that is inherited from Arm in order to keep and use some methods, working on the principle of forward and inverse kinematics.
*/
class ArmSystem : public Arm
{
private:
	unsigned int n;
	Arm v[100];

public:
	ArmSystem(float x = 0, float y = 0, float length = 0, float angle = 0, float n = 0) : Arm(x, y, length, angle) //Basic constructor.
	{
		//Creating the first one alone in order to create the others based on it.
		v[0].create(500, 0, length, angle);
		this->n = n;

		//Creating each arms in the array based on the previous ones values.
		for (int i = 1; i < n; i++)
		{
			v[i].create(v[i - 1].getX(), v[i - 1].getY(), length, angle);
		}
	}

	virtual void drag(Vector2f p) //Here we use inverse kinematics, we start at the end and work our way to the begining.
	{
		//We drag the last arm to the target.
		v[n].drag(p);

		//And then adjust the position of the rest based on it.
		for (int i = n - 1; i >= 0; i--)
			v[i].drag(Vector2f(v[i + 1].getX(), v[i + 1].getY()));

	}

	virtual void reach(float x, float y, Vector2f p) //In order to have a fixed position, but get as close to the target as possible,
	{
		//We fist drag it to the end, ignoring the pivot.
		drag(p);

		//And then we place the first arm at the pivot location, and adjust the remaning arms based on it's position.
		for (int i = 1; i < n; i++)
		{
			v[0].setPosition(x, y);
			v[i].setPosition(v[i - 1].getEndX(), v[i - 1].getEndY());
		}
	}

	virtual void render() //We cann render for each element in the array.
	{
		
		for (int i = 0; i < n; i++)
			v[i].render();
	}

};

//Basic bouncing ball object, boring.
class Ball
{
private:
	Vector2f position;
	CircleShape ball;

	float xVel = 4;
	float yVel = 5;
	float gravity = 1;

public:
	Ball(float x, float y)
	{
		position.x = x;
		position.y = y;

		ball.setRadius(14);
		ball.setOrigin(ball.getRadius()/2, ball.getRadius()/2);
		ball.setPosition(position);
	}

	Vector2f getPosition() const
	{
		return position;
	}

	void update()
	{
		if (position.y < height)
			position.y += gravity;

		if (position.x <= 0)
			xVel *= -1;
		else if (position.x + ball.getRadius()  >= width)
			xVel *= -1;
		else if (position.y <= 0)
			yVel *= -1;
		else if (position.y + ball.getRadius()  >= height)
			yVel *= -1;

		position.x += xVel;
		position.y += yVel;

		ball.setPosition(position);
	}

	void render()
	{
		window.draw(ball);
	}

};

int main()
{
	window.setFramerateLimit(60); //Pure laziness and bad programming practice, never do this kids.


	ArmSystem ShortArm(0, 0, 100, 0, 4);
	ArmSystem ShortArm2(0, 0, 85,0, 5);
	ArmSystem LongArm(0, 0, 8, 0, 80);

	Ball ball(200, 200);

	short Case = 0; // For showcasing.

	while (window.isOpen())
	{
		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || Keyboard::isKeyPressed(Keyboard::Escape))
				window.close();

			//Examples:
			     if (Keyboard::isKeyPressed(Keyboard::Num1))
				Case = 1;
			else if (Keyboard::isKeyPressed(Keyboard::Num2))
				Case = 2;
			else if (Keyboard::isKeyPressed(Keyboard::Num3))
				Case = 3;
			else if (Keyboard::isKeyPressed(Keyboard::Num4))
				Case = 4;

		}


		window.clear();


		if (Case == 1) //One draggable short Arm System to showcase the principle. (Forward Kinematics)
		{
			ShortArm.drag(Vector2f(Mouse::getPosition(window)));

			ShortArm.render();
		}

		else if (Case == 2) //Showing what can be done when you play with the values, creating a string. (Forward Kinematics)
		{
			LongArm.drag(Vector2f(Mouse::getPosition(window)));

			LongArm.render();
		}

		else if (Case == 3) //Showcasing a short Arm System with a pivot. (Reverse Kinematics)
		{
			ShortArm.reach(width / 2, height / 2, Vector2f(Mouse::getPosition(window)));

			ShortArm.render();
		}

		else if (Case == 4) //Some fun with multiple Arm Systems and a ball. (Reverse Kinematics).
		{
			ball.update();
			ShortArm.reach(width / 2 - 350, height, ball.getPosition());
			ShortArm2.reach(width / 2 + 350, height, ball.getPosition());

			ShortArm.render();
			ShortArm2.render();
			ball.render();
		}
		

		window.display();

	
	}

}