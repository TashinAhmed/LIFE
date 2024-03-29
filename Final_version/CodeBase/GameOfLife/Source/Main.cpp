#include <GLFW/glfw3.h>
//#include "GameBoard.h"

//Create a class that handles everything related to the game of life
class GameBoard {
private:
	bool** mBoard;							//A 2d array of bools representing whether a given cell is currently alive or dead
	bool** mBoard2;							//Used temporarily in the calculation for the next frame. Similar to double buffering
	int mScreenWidth;						//The size of the screens' width
	int mScreenHeight;						//The size of the screens' height
	int mBoardColumns;						//The number of column in the board
	int mBoardRows;							//The number of rows in the board
	int mCellSize;							//The width/height in pixels of a cell

	int WrappingModulo(int, int);			//Modulo operator that enforces the answer to be positive
	int SurroundingCells(int, int);			//Returns the number of enabled cells surrounding a given cell

public:
	GameBoard(int, int, int);				//The constructor which takes the screenwidth, screenheight, and cellSize
	void CalculateNextFrame();				//Calculates the next frame given the current frame
	void ChangedWindowSize(int, int);		//Changes the size of the gameboard to fit to the new resolution
	void SetBoardPosition(int, int, bool);	//Sets the board value at a given index
	void DrawGrid();						//Draws a grid onto the window depending on the boards width and height
	void DrawBoard();						//Draws the cells of the board onto the main screen
	void DrawMouseHover(double, double);	//Draws the square over the cell that the player is hovering over
	void MouseClick(double, double, bool);	//Enables/disables the cell the player clicked on
	void ResetBoard();						//Resets all the cells on the board to false
	void LoadPreconfiguration(int);			//Loads a premade sequence
};


//Constants
#define _SCREEN_WIDTH_MAX 1600
#define _SCREEN_HEIGHT_MAX 900
#define _SCREEN_WIDTH_MIN 1050
#define _SCREEN_HEIGHT_MIN 500
#define _CELL_SIZE 20
#define _INITIAL_FRAMESPEED 0.1

//Global variables
GameBoard board(_SCREEN_WIDTH_MAX, _SCREEN_HEIGHT_MAX, _CELL_SIZE);
bool leftClickDown = false;
bool rightClickDown = false;
bool paused = false;
bool hasQuit = false; //Set to true when the player presses 'q' or 'esc'
float frameSpeed = _INITIAL_FRAMESPEED;

//If the left or right click has been pressed
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//If the left click has been pressed
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
			leftClickDown = true;
		else if (action == GLFW_RELEASE)
			leftClickDown = false;
	}
	
	//If the right click has been pressed
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			rightClickDown = true;
		else if (action == GLFW_RELEASE)
			rightClickDown = false;
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//'R' key resets the board
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		board.ResetBoard();

	//'Space' key pauses/unpauses the board
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		paused = !paused;

	//'+' key increases the board speed
	if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		frameSpeed /= 2;
		if(frameSpeed < _INITIAL_FRAMESPEED / 16)
			frameSpeed = _INITIAL_FRAMESPEED / 16;
	}

	//'-' key increases the board speed
	if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		frameSpeed *= 2;
		if (frameSpeed > _INITIAL_FRAMESPEED * 4)
			frameSpeed = _INITIAL_FRAMESPEED * 4;
	}

	//'esc' or 'q' key closes the program
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) 
	{
		hasQuit = true;
		glfwTerminate();
	}

	//'1' to '5' load a premade sequence
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
		board.LoadPreconfiguration(1);
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		board.LoadPreconfiguration(2);
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		board.LoadPreconfiguration(3);
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
		board.LoadPreconfiguration(4);
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
		board.LoadPreconfiguration(5);
}

int main(void)
{
	//-----------------Initialize openGL window------------------------------//
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	//Create the window
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(_SCREEN_WIDTH_MAX, _SCREEN_HEIGHT_MAX, "Game of Life", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	//Set the min and max sizes for the window
	glfwSetWindowSizeLimits(window, _SCREEN_WIDTH_MIN, _SCREEN_HEIGHT_MIN, _SCREEN_WIDTH_MAX, _SCREEN_HEIGHT_MAX);

	//Make the new window appear in front of all other windows
	glfwMakeContextCurrent(window);

	//Event handlers
	glfwSetMouseButtonCallback(window, mouse_button_callback); //Mouseclick event
	glfwSetKeyCallback(window, key_callback); //Keyboard event

	//-----------------Initialize Game of Life varianbles--------------------//
	double xpos, ypos; //The x and y of the mouse
	int width = _SCREEN_WIDTH_MAX, height = _SCREEN_HEIGHT_MAX; //The current width and height of the window

	//Give the board an initial pattern
	board.LoadPreconfiguration(2);

	//Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		//Get the position of the mouse
		glfwGetCursorPos(window, &xpos, &ypos);

		//Enable/disable the cells that the player left/right clicks on
		if (leftClickDown) 
		{
			board.MouseClick(xpos, ypos, true);
		}
		else if (rightClickDown)
		{
			board.MouseClick(xpos, ypos, false);
		}

		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1, 1, 1, 1); //Set the background color to all white
		
		//Drawing
		board.DrawBoard(); //Draw the board
		board.DrawGrid(); //Draw the gridlines

		//Draw the cell outline we're hovering over
		board.DrawMouseHover(xpos, ypos);

		//Calculate the next frame if a certain amount of time has elapsed
		if (glfwGetTime() > frameSpeed && !paused)
		{
			//Check if the window has been resized
			int newWidth, newHeight;
			glfwGetFramebufferSize(window, &newWidth, &newHeight);
			
			if (newWidth != width || newHeight != height) 
			{
				board.ChangedWindowSize(newWidth, newHeight);

				width = newWidth;
				height = newHeight;
			}

			board.CalculateNextFrame();
			glfwSetTime(0);
		}

		//Double buffer
		glfwSwapBuffers(window);

		//Poll for events
		glfwPollEvents();

		if (hasQuit)
			return 0;
	}

	glfwTerminate();
	return 0;
}