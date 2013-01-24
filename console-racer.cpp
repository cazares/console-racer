#include <iostream>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()


// use curses.h for windows
//#include <curses.h>

//use ncurses for mac or linux
#include <ncurses.h>


using namespace std;


struct screenData{
  	// create the track
	int screenHeight;  // the number of lines on the display
	int screenWidth;
	
	// set up variables for wall positions within the display strings
	int leftWall;
	int rightWall;
	int prevLeftWall; 
	int prevRightWall;
	
	// set up variables to store the postion of the car
	// the x position is the position in the display string
	// the y position is which string in the track array the car is in
	int currCarPositionX;
	int currCarPositionY;
	int prevCarPositionX;
	int prevCarPositionY;
	
	// The following will be an array of strings that
	// such that each string is one line of the game display
	// treat this variable like it is an array of strings.  you can use the [] operator
	string *track;
	
	
	// an integer indicating what key the user has pressed on the keyboard
	int keyPressed;
};

void updateCarPosition( screenData & screen );

void updateTrack( screenData & screen );

void getNextWallPosition( screenData & screen );

bool checkForCrashes( screenData & screen );

void drawCar( screenData & screen, bool crashOccurred );

void printTrack( screenData & screen );


void createRocks( screenData &screen );

int main()
{	
	
	/******
	 The following code sets up the terminal to accept keyboard input
	 and display our game screen
	 *******/
	initscr();			/* Start curses mode !!!		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */
	halfdelay(1);		/*  delay of 1 before screen is refreshed */
	/**********************/
	
	// create the struct that keeps track of everything on the screen
	screenData screen;
	
	/********* initialize the screen.  ********/
	// The variables LINES and COLS are provided by the curses library
	// and tell how many lines high our screen is and how many characters wide it is
	screen.screenHeight = LINES;
	screen.screenWidth = COLS;
	string track[LINES];
	screen.track = track;
	
	// initialize variables for wall positions
	screen.leftWall = 5;
	screen.rightWall = 20;
	screen.prevLeftWall = screen.leftWall; 
	screen.prevRightWall = screen.leftWall;
	
	// create the initial track
	for( int i = 0; i < screen.screenHeight; i++ )
	{
		
		screen.track[i] = "";
		for(  int j = 0; j < screen.leftWall; j ++ ) 
                    screen.track[i] += " ";
		screen.track[i] += "|" ;
		for( int j= screen.leftWall+1; j < screen.rightWall; j ++ ) 
                    screen.track[i] += " ";
		screen.track[i] += "|";
		for( int j = screen.rightWall+1; j < screen.screenWidth; j++ )
			screen.track[i] += " ";		
	}
	
	
	// initialize variables for the position of the car
	screen.currCarPositionX = 10;
	screen.currCarPositionY = screen.screenHeight-1;
	screen.prevCarPositionX = screen.currCarPositionX;
	screen.prevCarPositionY= screen.currCarPositionY;
	
	/********* finished initializing the screen *********/
	srand(time(0));  // Initialize random number generator.
	
	bool continueGame = true; // keep the game running while this is true
	
	while( continueGame )
	{
		screen.keyPressed = getch(); // get  input from the keyboard
		
	    updateCarPosition( screen ); // update the position where the car should be displayed
		
		
		
		// update the track walls
		getNextWallPosition( screen);
		
		// shift the track down by one line
		updateTrack( screen );
		
		// maybe make a rock
		createRocks( screen );
		
		// check for a crash
		continueGame = checkForCrashes( screen );
		
		// draw the car to the track array
		drawCar( screen, !continueGame );
		
		// actually print the track to the screen
		printTrack( screen );
		
		// curses function to refresh the display
		refresh();			/* Print it on to the screen */
	}
	
	/********** the following code makes the user hit a key other 
	 than an arrow key to exit the game
	 *************/
	screen.keyPressed = getch();
	while( screen.keyPressed == ERR ||  screen.keyPressed == KEY_UP 
		  ||  screen.keyPressed == KEY_DOWN ||  screen.keyPressed == KEY_LEFT
		  ||  screen.keyPressed == KEY_RIGHT  )
		screen.keyPressed = getch();
	/********************/
	
	
	endwin();			/* End curses mode.  this is important!	  */
	
	return 0;
}




// This function updates the track array by drawing the car in the correct.
// position in the track array.
// The car should be a ^ symbol.  If a crash has occurred,
// draw a * symbol.
void drawCar( screenData &screen, bool crashOccurred )
{
   int carX = screen.currCarPositionX;
   int carY = screen.currCarPositionY;
   string currentTrack = screen.track[carY];

   if(!crashOccurred)
   {
      currentTrack[carX] = '^';
   }
   else
   {
      currentTrack[carX] = '*';
   }

   screen.track[carY] = currentTrack;	
}


// This function returns true if no crash has occurred and the game should 
// continue.
// it returns false if a crash has occurred.  
// Crashes can occur by hitting walls and rocks
bool checkForCrashes( screenData &screen)
{
   int carX = screen.currCarPositionX;
   int carY = screen.currCarPositionY;
   string currentTrack = screen.track[carY];
   char currChar = currentTrack[carX];

   if(currChar == '#' || currChar == '\\' || currChar == '/' ||
      currChar == '|')
   {
      return false;
   }
   else
   {
      return true;
   }
}

// this function uses the random number generation technique used below to randomly generate
// rocks.  rocks should only be generated on the track (between the walls)
// rocks will be represented by a # symbol. 
// this function should update the track array to include the new rock.
// Only generate 0 or 1 rocks when this function is called.
void createRocks( screenData &screen )
{
   int prob = rand() % 10;  // a random number, tells us to draw a rock or not
   int screenWidth = screen.rightWall - screen.leftWall - 1; // space between walls
   int position;
   int offset = screen.leftWall;
   string rockTrack = screen.track[0];
	
   if(prob == 0 && screenWidth > 2)
   {
      position = rand() % screenWidth;
      rockTrack[position+offset+1] = '#';
      screen.track[0] = rockTrack;
   }  
}


// Update the track. Shift every line in the tracks array down by 1.
// the last line can be overwritten.  Draw the walls for the new 
// track line at track[0].  Remember to erase the old car position.
void updateTrack( screenData &screen )
{
   int prevCarX = screen.prevCarPositionX;
   int prevCarY = screen.prevCarPositionY;
   int carX = screen.currCarPositionX;
   int carY = screen.currCarPositionY;
   string prevCarTrack = screen.track[prevCarY];
   string carTrack = screen.track[carY];

   prevCarTrack[prevCarX] = ' ';
   screen.track[prevCarY] = prevCarTrack;

      // shifts everything down 1
    for(int i = screen.screenHeight-1; i > 0; i--)
    {
        screen.track[i] = screen.track[i-1];
    }

      // initialize next track line
    screen.track[0] = "";
    for(int j = 0; j < screen.leftWall; j++)
    {
        screen.track[0] += " ";
    }
    if(screen.prevLeftWall < screen.leftWall)
    {
        screen.track[0] += "/";
    }
    else if(screen.prevLeftWall == screen.leftWall)
    {
        screen.track[0] += "|";
    }
    else if(screen.prevLeftWall > screen.leftWall)
    {
        screen.track[0] += "\\";
    }

    for(int j = screen.leftWall+1; j < screen.rightWall; j++)
    {
        screen.track[0] += " ";
    }
    
    if(screen.prevRightWall < screen.rightWall)
    {
        screen.track[0] += "/";
    }
    else if(screen.rightWall == screen.rightWall)
    {
        screen.track[0] += "|";
    }
    else if(screen.prevRightWall > screen.rightWall)
    {
        screen.track[0] += "\\";
    }
    for(int j = screen.rightWall+1; j < screen.screenWidth; j++)
    {
        screen.track[0] += " ";
    }
}



/*****  everything below this point is implemented for you .  Use it for reference****/
void getNextWallPosition( screenData &screen )
{
	int r = rand() % 3; // a random number
	
	screen.prevLeftWall = screen.leftWall;
	screen.prevRightWall = screen.rightWall;
	
	if( r == 0 ) // move left wall left
	{
		screen.leftWall = (screen.leftWall > 1 ) ? screen.leftWall-1 : screen.leftWall;  
	}
	if( r == 1 ) // move left wall right
	{
		screen.leftWall = (screen.leftWall < screen.rightWall-3 ) ? screen.leftWall+1 : screen.leftWall;  
	}
	
	r = rand() % 3;
	if( r == 0 ) // move right wall left
	{
		screen.rightWall = (screen.rightWall > screen.leftWall + 3 ) ? screen.rightWall-1 : screen.rightWall;  
	}
	if( r == 1 ) // move left wall right
	{
		screen.rightWall = (screen.rightWall < screen.screenWidth ) ? screen.rightWall+1 : screen.rightWall;  
	}
	
}

void printTrack( screenData &screen )
{
	
	for( int i = 0; i < screen.screenHeight; i++ )
		mvprintw( i, 0, screen.track[i].c_str() );
}

void updateCarPosition( screenData &screen )
{
	screen.prevCarPositionX = screen.currCarPositionX;
	screen.prevCarPositionY = screen.currCarPositionY;
	
	
	if(screen.keyPressed == KEY_LEFT)
	{
		screen.currCarPositionX--;
	}
	else if( screen.keyPressed == KEY_RIGHT )
	{
		screen.currCarPositionX++;
	} else if( screen.keyPressed == KEY_UP )
	{
		screen.currCarPositionY = screen.currCarPositionY > 0 ? screen.currCarPositionY-1 : screen.currCarPositionY;
	} else if( screen.keyPressed == KEY_DOWN )
	{
		screen.currCarPositionY = screen.currCarPositionY < screen.screenHeight-1? screen.currCarPositionY+1 : screen.currCarPositionY;
	}
}





