// Fun with Frogs.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
using namespace tle;
#include <sstream>
using namespace std;
#include <math.h>


// stores and switches between states depending on state of frogs
enum gamesStates {playing, paused ,over};

// creates different states for the frogs to switch between within the playing state
enum frogStates {waiting, crossing , safe, dead, sinking};

// intialises the states for movement of frogs
enum frogMovement {resting, moveForward, moveBackward, moveLeft, moveRight};

// intialises the states for forward movemeent , rising and sinking of transits
enum transitState {moveTransit, sinkTransit, riseTransit};

// intialises the states for forward movemeent , rising and sinking of range rovers
enum rangeRoverState { moveRangeRover, sinkRangeRover, riseRangeRover };

// intialises the states for forward movemeent , rising and sinking of rovers
enum roverState { moveRover, sinkRover, riseRover };

// intialises the states for forward movemeent , rising and sinking of pickups
enum pickUpState { movePickUp, sinkPickUp, risePickUp };

// intiliases the states for the random appearance and movement of enemy spider
enum spiderStates {delay, moveSpider, sink};

// intiliases function to detect collision between frogs and vehicles
bool sphere2box(float sxPos, float szPos, float sRad,
	            float bxPos, float bzPos, float bwidth, float bdepth);

// intilises function to detect collision between frogs and plants and spider
bool sphere2sphere(float s1xPos, float s1zPos, float s1Rad, float s2xPos, float s2zPos, float s2Rad);

const float kskyYPos     = -1000.0f;                    // sets the vertical position of the skybox
const float ksurfaceYPos = -5.0f;                       // sets the vertical position of the surface 
const float kislandYPos  = -5.0f;                       // sets the z position of island 2
const float kislandZPos  = 115.0f;                      // sets the vertical position of island 2
const float kcameraYPos  = 40.0f;                       // camera positioned behind frog to be rotated 
const float kcameraZPos  = -60.0f;                      // camera positioned relative to frog to be rotated
const float kscaleFactor = 10.0f;                       // increases size of tyres and spider to make it visible
const float ktyreYPos    = -2.0f;                       // elevates vertical position of tyres when created 
const float kplantZPos   = 115.0f;                      // sets position of plants on island 2

const float ktyreLane1ZPos = 75.0f;                     // sets position of lane 1 tyres for collision detection between frogs
const float ktyreLane2ZPos = 85.0f;                     // sets position of lane 2 tyres for collision detection between frogs
const float ktyreLane3ZPos = 95.0f;                     // sets position of lane 3 tyres for collision detection between frogs
const float ktyreLane4ZPos = 105.0f;                    // sets position of lane 4 tyres for collision detection between frogs

const float kcameraAngle      = 20.0f;                  // rotates camera making it look down on island
const float krotationFactor   = 90.0f;                  // rotates vehicles to point left and right respectively

const float kGameSpeed         = 1.5f;                  // Constant speed of the game that'll be used as a baseline to multiply by
const float ktransitSpeed      = 8.0f;                  // Constant speed of transit that gets multiplied by kgamespeed to increase or decrease
const float krangeRoverSpeed   = ktransitSpeed * 1.5f;  // Constant speed of range rover as a result of transit speed multiplied by 1.5
const float kroverSpeed        = ktransitSpeed / 2.0f;  // Constant speed of rover as a result of transit speed divided by 2
const float kpickUpSpeed       = ktransitSpeed * 2.0f;  // Constant speed of pick up as a result of transit multiplied by 2
const float kVerticalSpeed     = 5.0f;                  // used to sink and rise vehicles until limits are reached
const float kfrogSpeed         = 0.05f;                 // constant speed used to move frog in all four directions
const float krotationSpeed     = 8.05f;                 // allows forward and backward movement of camera between limits
const float kmaxRotation       = 70.0f;                 // stops camera rotation forward beyong this point
const float kminRotation       = 5.0f;                  // stops camera rotation backward below this point
const float ksquashFactor      = 0.1f;                  // used to scale down vertical position of frog in dead state
const float ktyreSpeed         = 10.0f;                 // used for the movement of tyres between bounds

const float kfrogRadius      = 5.7f;                    // used to check collisions between tyres and vehicles causing state change
const float ktransitWidth    = 3.0f;                    // used to create a bounding box to check for collision between current frog
const float ktransitDepth    = 6.0f;                    // used to create a bounding box to check for collision between current frog
const float krangeRoverWidth = 2.6f;                    // used to create a bounding box to check for collision between current frog
const float krangeRoverDepth = 5.5f;                    // used to create a bounding box to check for collision between current frog
const float kroverWidth      = 3.0f;                    // used to create a bounding box to check for collision between current frog
const float kroverDepth      = 6.3f;                    // used to create a bounding box to check for collision between current frog
const float kpickUpWidth     = 2.8f;                    // used to create a bounding box to check for collision between current frog
const float kpickUpDepth     = 6.2f;                    // used to create a bounding box to check for collision between current frog
const float ktyreWidth       = 35.0f;                   // used to create a bounding box to check for collision between current frog
const float ktyreDepth       = 30.0f;                   // used to create a bounding box to check for collision between current frog
const float kplantRadius     = 1.5f;                    // used to create a bounding radius to check for collision between current frog
const float spiderRadius     = 4.0f;                    // used to create a bounding radius to check for collision between current frog

const float kfontX     = 500.0f;                        // sets position of score text 500 units across
const float kfontY     = 40.0f;                         // sets position of score text 40 units down
const float kfontTimeX = 400.0f;                        // sets position of time remaining text 400 units across
const float kfontTimeY = 60.0f;                         // sets position of time remaining text 60 units down
const float kfontOverX = 500.0f;                        // sets position of game over text 500 units across
const float kfontOverY = 300.0f;                        // sets posiiton of game over text 300 units down

const float kLane1ZPos   = 70.0f;                       // switches frog state to dead if within limit and not in collision with lane 1 tyre
const float kLane2ZPos   = 80.0f;                       // switches frog state to dead if within limit and not in collision with lane 2 tyre
const float kLane3ZPos   = 90.0f;                       // switches frog state to dead if within limit and not in collision with lane 3 tyre
const float kLane4ZPos   = 100.0f;                      // switches frog state to dead if within limit and not in collision with lane 4 tyre
const float kIsland2ZPos = 110.0f;                      // switches frog state to dead if within limit and not in collision with island 2

const int kLeftLimit        = -61;                      // maximum distance vehicles can go in left direction before sinking
const int kRightLimit       = 61;                       // maximum distance vehicles can go in right direction before sinking
const int kUpperLimit       = 120;                      // sets current frogs position to current position if it goes beyond this limit
const int kLowerLimit       = 15;                       // sets current frogs position to current position if it goes beyond this limit
const int ksafeZone         = 115;                      // switches state of current frog to safe once crossed
const int ksinkLimit        = -12;                      // maximum distance vehicles and spider can sink before rising in opposite side
const int kislandRightLimit =  55;                      // sets position of frogs to this limit if gone above
const int kislandLeftLimit  = -55;                      // sets position of frogs to this limit if gone below 

const int kNumFrogs         = 3;                        // Constant number of frogs within the simulation 
const int kNumIslands       = 5;                        // Constant number of islands within the simulation
const int kNumTyres         = 3;                        // Constant number of tyres in each lane within the simulation that detects collision with frogs
const int kNumVehicles      = 2;                        // Constant number of vehicles in each lane within the simulation that detects collision with frogs
const int kNumPlants        = 6;                        // Constant number of plants within the simulation that detects collision with frogs

const int kstep      = 10;                              // Constant integer that adds or subtracts from the frogs current position moving it in four directions
const int kscoreStep = 10;                              // Constant integer that adds to the current score each time the frog moves forward
const int kfontSize  = 36;                              // Constant integer that loads the size of texts

const EKeyCode kPauseKey      = Key_P;                  // Declares constants for the keyboard controls - pausing of game
const EKeyCode kLeftKey       = Key_Z;                  // Declares constants for the keyboard controls for frogs - moving the frogs left
const EKeyCode kRightKey      = Key_X;                  // Declares constants for the keyboard controls for frogs - moving the frogs right
const EKeyCode kForwardKey    = EKeyCode(192);          // Declares constants for the keyboard controls for frogs - moving the frogs forward
const EKeyCode kBackwardKey   = EKeyCode(191);          // Declares constants for the keyboard controls for frogs - moving the frogs backward
const EKeyCode kEscapeKey     = Key_Escape;             // Declares constants for the keyboard controls - quits the program
const EKeyCode kUpKey         = Key_Up;                 // Declares constants for the keyboard controls for camera - tilting the camera forward
const EKeyCode kDownKey		  = Key_Down;               // Declares constants for the keyboard controls for camera - tilting the camera backward
const EKeyCode kresetCamera   = Key_C;                  // Declares constants for the keyboard controls for camera - reset camera position relative to current frog

const string kstrSky        = "skybox.x";               // Loads the .x file for sky to the program
const string kstrSurface    = "surface.x";              // Loads the .x file for surface to the program
const string kstrIsland1    = "island1.x";              // Loads the .x file for island 1 to the program
const string kstrFrog       = "frog.x";                 // Loads the .x file for frog to the program
const string kstrTransit    = "transit.x";              // Loads the .x file for transit to the program
const string kstrRangeRover = "rangeRover.x";           // Loads the .x file for range rover to the program
const string kstrDummy      = "dummy.x";                // Loads the .x file for dummy to the program
const string kstrIsland2    = "island2.x";              // Loads the .x file for dummy to the program
const string kstrTyre       = "tyre.x";                 // Loads the .x file for tyre to the program
const string kstrRover      = "rover.x";                // Loads the .x file for rover to the program
const string kstrPickUp     = "pickUp.x";               // Loads the .x file for sky to the program
const string kstrPlant      = "plant.x";                // Loads the .x file for plant to the program
const string kstrSpider     = "spider.x";               // Loads the .x file for spider to the program
const string kstrFont       = "Comic Sans MS";          // Loads the font type for text streams
const string kstrFrogAlive  = "frog.jpg";               // Assigns skin to all frogs alive
const string kstrFrogSafe   = "frog_blue.jpg";          // Assigns skin to all frogs in the safe state
const string kstrFrogDead   = "frog_red.jpg";           // Assigns skin to all frogs in the dead state
const string kstrScore      = "Score: ";                // Displays text that shows the current score
const string kstrTime       = "Time remaining: ";       // Displays text that shows the time remaining
const string kstrGameOver   = "Game over!";             // Displays text that shows that the game is over

const float kPi   = 3.1415926f;
const float kEdge = 2 * kPi;

// Stores properties of frogs
struct frog
{
	IModel*   model;                                    // loads the frog models to the program which can be moved 
	IModel*   dummy;                                    // loads the dummy model that attaches to the frog
	float     speed             = kfrogSpeed;           // stores kfrogspeed which is multiples by kgamespeed and frametime
	frogStates currentFrogState = waiting;              // sets initial state  of current frog within playing state
};

// Stores properties of transits
struct transit
{
	IModel* model;                                      // loads transit models to the program which moves right to left , sinks and rises
	transitState transitMovement = moveTransit;         // sets initial state of transits which changes after hitting boundaries
};

// Stores properties of range rovers
struct rangeRover                                       
{
	IModel* model;                                       // loads range rovers models to the program which moves left to right , sinks and rises
	rangeRoverState rangeRoverMovement = moveRangeRover; // sets initial state of range rovers which changes after hitting boundaries
};

// Stores properties of rovers
struct rover
{
	IModel* model;                                      // loads rovers models to the program which moves left to right , sinks and rises
	roverState roverMovement = moveRover;				// sets initial state of rovers which changes after hitting boundaries
};

// Stores properties of pick ups
struct pickup
{
	IModel* model;                                      // loads pick up models to the program which moves right to left , sinks and rises
	pickUpState pickUpMovement = movePickUp;			// sets initial state of pick ups which changes after hitting boundaries
};

// Stores properties of tyres
struct tyres
{
	IModel* Lane1;                                      // displays tyres of three on lane 1 on the screen, making them bounce and detect collision
	IModel* Lane2;                                      // displays tyres of three on lane 2 on the screen, making them bounce and detect collision
	IModel* Lane3;                                      // displays tyres of three on lane 3 on the screen, making them bounce and detect collision
	IModel* Lane4;                                      // displays tyres of three on lane 4 on the screen, making them bounce and detect collision
};

// Stores properties of spider
struct spider
{
	IModel* model;                                      // displayys spider on the screen and kills current frog when collides        
	float speed;                                        // stores spider speed which determines how fast it moves when multiplied with kgamespeed
	int count = 0;                                      // causes spider to appear at previous safe zone when reaches a max count
	int MaxCount;                                       // assigned to a random generated number making spider appear
	spiderStates currentSpiderState = delay;            // sets initial state of spider which changes after hitting boundaries
};
void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media2");


	/**** Set up your scene here ****/

	IFont* myFont = myEngine->LoadFont(kstrFont, kfontSize); // loads font type and size on the screen
	// Load "Comic Sans MS" font at 36 points

	IMesh* skyMesh     = myEngine->LoadMesh(kstrSky);        // skymesh is called on the program
	IMesh* surfaceMesh = myEngine->LoadMesh(kstrSurface);    // surfacemesh is called on the program
	IMesh* islandMesh  = myEngine->LoadMesh(kstrIsland1);    // island1mesh is called on the program
	IMesh* frogMesh    = myEngine->LoadMesh(kstrFrog);       // frogmesh is called on the program
	IMesh* transitMesh = myEngine->LoadMesh(kstrTransit);    // transitmesh is called on the program
	IMesh* rangeRMesh  = myEngine->LoadMesh(kstrRangeRover); // rangerovermesh is called on the program
	IMesh* dummyMesh   = myEngine->LoadMesh(kstrDummy);      // dummymesh is called on the program
	IMesh* island2Mesh = myEngine->LoadMesh(kstrIsland2);    // island2mesh is called on the program
	IMesh* tyreMesh    = myEngine->LoadMesh(kstrTyre);       // tyremesh is called on the program
	IMesh* roverMesh   = myEngine->LoadMesh(kstrRover);      // rovermesh is called on the program
	IMesh* pickupMesh  = myEngine->LoadMesh(kstrPickUp);     // pickupmesh is called on the program
	IMesh* plantMesh   = myEngine->LoadMesh(kstrPlant);      // plantmesh is called on the program
	IMesh* spiderMesh  = myEngine->LoadMesh(kstrSpider);     // spidermesh is called on the program

	IModel* sky        = skyMesh->CreateModel(0.0f, kskyYPos, 0.0f);               // loads sky on the program
	IModel* surface    = surfaceMesh->CreateModel(0.0f, ksurfaceYPos, 0.0f);       // loads surface on the program
	IModel* island2    = island2Mesh->CreateModel(0.0f, kislandYPos, kislandZPos); // loads island 2 on the program

	spider spider;                                               
	spider.model = spiderMesh->CreateModel(0.0f, kskyYPos, 0.0f);// spider is called and displayed on the screen
	spider.MaxCount = (rand() % 10000 + 5000);                   // max count set to a random generated number 
	spider.model->Scale(kscaleFactor);                           // spider scaled to a visible size for collision detection
	spider.model->RotateY(-krotationFactor);                     // rotated to face the left

	// islands are displayed ont the program using associated X and Z values stored in arrays
	float islandXs[kNumIslands] = { 0.0f, 0.0f, 0.0f , -500.0f, 500.0f }; 
	float islandZs[kNumIslands] = { 40.0f,-200.0f ,1500.0f ,40.0f,40.0f};
	IModel* islands[kNumIslands];
	for (int i = 0; i < kNumIslands; i++)
	{
		islands[i] = islandMesh->CreateModel(islandXs[i], kislandYPos, islandZs[i]);
	}

	// frogs and dummies are displayed on the screen using associated X and Z values stored in arrays
	float frogXs[kNumFrogs] = { -10.0f,0.0f,10.0f };
	float frogZs[kNumFrogs] = { 15.0f,15.0f,15.0f };
	frog myFrogs[kNumFrogs];
	for (int i = 0; i < kNumFrogs; i++)
	{
		myFrogs[i].model = frogMesh->CreateModel();
		myFrogs[i].dummy = dummyMesh->CreateModel(frogXs[i], 0.0f, frogZs[i]);
		myFrogs[i].model->AttachToParent(myFrogs[i].dummy); // each frog is attached to its own dummy following dummy movements                 
		myFrogs[i].model->SetSkin(kstrFrogAlive);           // skins are assigned to all frogs in the crossing state
	}

	// transit X and Z values are called in the program
	float transitXs[kNumVehicles] = { 60.0f , 20.0f};
	float transitZs[kNumVehicles] = { 35.0f, 35.0f};
	transit myTransits[kNumVehicles];
	
	// range rover X and Z values are called in the program
	float rangeRoverXs[kNumVehicles] = { -50.0f, -20.0f};
	float rangeRoverZs[kNumVehicles] = { 45.0f, 45.0f };
	rangeRover myRangeRovers[kNumVehicles];

	// rover X and Z values are called in the program
	float roverXs[kNumVehicles] = { -60.0f, -10.0f };
	float roverZs[kNumVehicles] = { 25.0f, 25.0f };
	rover myRovers[kNumVehicles];

	// pick up X and Z values are called in the program
	float pickUpXs[kNumVehicles] = { 70.0f, 0.0f };
	float pickUpZs[kNumVehicles] = { 55.0f, 55.0f };
	pickup myPickUps[kNumVehicles];

	for (int i = 0; i < kNumVehicles; i++)
	{
		myTransits[i].model = transitMesh->CreateModel(transitXs[i], 0.0f, transitZs[i]); // transits are displayed in the program using associated X and Z values
		myTransits[i].model->RotateY(-krotationFactor); // transits are rotated to face left
		myRangeRovers[i].model = rangeRMesh->CreateModel(rangeRoverXs[i], 0.0f, rangeRoverZs[i]); // range rover are displayed in the program using associated X and Z values
		myRangeRovers[i].model->RotateY(krotationFactor); // range rovers are rotated to face right
		myRovers[i].model = roverMesh->CreateModel(roverXs[i], 0.0f, roverZs[i]); // rover are displayed in the program using associated X and Z values
		myRovers[i].model->RotateY(krotationFactor); // rovers are rotated to face right
		myPickUps[i].model = pickupMesh->CreateModel(pickUpXs[i], 0.0f, pickUpZs[i]); // pick up are displayed in the program using associated X and Z values
		myPickUps[i].model->RotateY(-krotationFactor); // pick ups are rotated to face left
	}

	// lane 1 and 3 tyres' X and Z values are called in the program
	float tyreLeftXs[kNumTyres] = { -50, -41,-32 };
	// lane 2 and 4 tyres' X and Z values are called in the program
	float tyreRightXs[kNumTyres] = { 50,  41, 32 };
	tyres myTyres[kNumTyres];
	for (int i = 0; i < kNumTyres; i++)
	{
		myTyres[i].Lane1 = tyreMesh->CreateModel(tyreLeftXs[i], ktyreYPos, ktyreLane1ZPos);  // displays lane 1 tyres using associated X, Y and Z values
		myTyres[i].Lane2 = tyreMesh->CreateModel(tyreRightXs[i], ktyreYPos, ktyreLane2ZPos); // displays lane 2 tyres using associated X, Y and Z values
		myTyres[i].Lane3 = tyreMesh->CreateModel(tyreLeftXs[i], ktyreYPos, ktyreLane3ZPos);  // displays lane 3 tyres using associated X, Y and Z values
		myTyres[i].Lane4 = tyreMesh->CreateModel(tyreRightXs[i], ktyreYPos, ktyreLane4ZPos); // displays lane 4 tyres using associated X, Y and Z values

		myTyres[i].Lane1->Scale(kscaleFactor); // scales lane 1 tyres to a visible size
		myTyres[i].Lane2->Scale(kscaleFactor); // scales lane 2 tyres to a visible size
		myTyres[i].Lane3->Scale(kscaleFactor); // scales lane 3 tyres to a visible size
		myTyres[i].Lane4->Scale(kscaleFactor); // scales lane 4 tyres to a visible size
	}
	
	// plants are for the program to display it on the screen suing associated X and Z values
	float plantXs[kNumPlants] = { -50.0f, -30.0f, -10.0f, 10.0f, 30.0f, 50.0f };
	IModel* plants[kNumPlants];
	for (int i = 0; i < kNumPlants; i++)
	{
		plants[i] = plantMesh->CreateModel(plantXs[i], 0.0f, kplantZPos);
	}
	
	// Camera is called for the program to display it on the screen
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0.0f, kcameraYPos, kcameraZPos);
	myCamera->RotateX(kcameraAngle); // rotates camera to look down on the island

	gamesStates currentState = playing;    // sets current state of game to playing where all movements and collisions occur
	frogMovement currentMovement= resting; // sets current movement of frog to resting awaiting key press

	int currentFrog = 0; // increments as current frog is either in safe or dead state
	int Score       = 0; // increments each time currentfrog moves forward

	float countdown = 20.0f; // decrements until it reaches zero after which current frog dies
	float sinValue;

	bool pause     = false; // Boolean to determine if the simulation is halted or playing
	bool isMoving  = false; // Boolean to determine whether tyres are hitting boundaries or not
	bool collision = false; // Boolean to detect whether collision has occurred or not
	 
	myCamera->AttachToParent(myFrogs[currentFrog].dummy);                    // Follows dummy without interfering with frog movements
	myFrogs[currentFrog].model->AttachToParent(myFrogs[currentFrog].dummy);  // Makes frog model copy movements and direction of dummy

	myEngine->Timer(); // Establishes first time base

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		// Save the old positions of current frog
		float frogOldX = myFrogs[currentFrog].model->GetX();
		float frogOldZ = myFrogs[currentFrog].model->GetZ();

		/**** Update your scene each frame here ****/
		stringstream textScore;
		stringstream textCount;

		textScore << kstrScore << Score;  // Used to display score variable in the form of a text
		myFont->Draw(textScore.str(), kfontX, kfontY); // displays text on screen using assigned X and Y values

		float frameTime = myEngine->Timer();    // time taken for each frame to be rendered
		textCount << kstrTime << int(countdown); // Used to display countdown variable in the form of a text
		myFont->Draw(textCount.str(), kfontTimeX, kfontTimeY); // displays text on screen using assigned X and Y values

		switch (currentState)
		{
		   case playing:
		   {
			   countdown -= frameTime; // decrements until reaches zero which kills current frog
			   
			   for (int i = 0; i < kNumVehicles; i++)
			   {
				  // switch statement that moves transit from right to left, cause it to sink and return to intial position
				   switch (myTransits[i].transitMovement)
				   {
				      case moveTransit:
					  {
						  if (myTransits[i].model->GetX() > kLeftLimit) // moves transit to the left only if within limits
						  {
							  myTransits[i].model->MoveX((-ktransitSpeed * kGameSpeed) * frameTime);
						  }
						  else
						  {
							  myTransits[i].transitMovement = sinkTransit; // changes state to sinking once beyond limit
						  }
						  break;
					  }
					  case sinkTransit:
					  {
						  if (myTransits[i].model->GetY() >= ksinkLimit) // moves tranit down only if within sink limit
						  {
							  myTransits[i].model->MoveY((-kVerticalSpeed * kGameSpeed) * frameTime);
						  }
						  else
						  {
							  myTransits[i].model->SetX(kRightLimit); // sets it back to initial position
							  myTransits[i].transitMovement = riseTransit; // switches state to rise
						  }
						  break;
					  }
					  case riseTransit:
					  {
						  if (myTransits[i].model->GetY() <= 0.0f) // moves transit up only if within limits
						  {
							  myTransits[i].model->MoveY((kVerticalSpeed * kGameSpeed) * frameTime);
						  }
						  else
						  {
							  myTransits[i].transitMovement = moveTransit; // switches state to move to left again
						  }
						  break;
					  }
				   }// end of transit movement

				   // switch statement that moves range rovers from left to right, cause it to sink and return to intial position
				   switch (myRangeRovers[i].rangeRoverMovement)
				   {
				       case moveRangeRover:
				     {
					   if (myRangeRovers[i].model->GetX() < kRightLimit) // moves range rover to the right only if within limits
					   {
						   myRangeRovers[i].model->MoveX((krangeRoverSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRangeRovers[i].rangeRoverMovement = sinkRangeRover; // changes state to sinking once beyond limit
					   }
					   break;
				     }
				       case sinkRangeRover:
				     {
					   if (myRangeRovers[i].model->GetY() >= ksinkLimit) // moves range rover down only if within sink limit
					   {
						   myRangeRovers[i].model->MoveY((-kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRangeRovers[i].model->SetX(kLeftLimit); // sets it back to initial position
						   myRangeRovers[i].rangeRoverMovement = riseRangeRover; // switches state to rise
					   }
					   break;
				     }
				     case riseRangeRover:
				     {
					   if (myRangeRovers[i].model->GetY() <= 0.0f) // moves range rover up only if within limits
					   {
						   myRangeRovers[i].model->MoveY((kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRangeRovers[i].rangeRoverMovement = moveRangeRover; // switches state to move to right again
					   }
					   break;
				     }
				   }// end of range rover movement

				   // switch statement that moves rovers from left to right, cause it to sink and return to intial position
				   switch (myRovers[i].roverMovement)
				   {
				     case moveRover:
				     {
					   if (myRovers[i].model->GetX() < kRightLimit) // moves rover to the right only if within limits
					   {
						   myRovers[i].model->MoveX((kroverSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRovers[i].roverMovement = sinkRover; // changes state to sinking once beyond limit
					   }
					   break;
				     }
				     case sinkRover:
				     {
					   if (myRovers[i].model->GetY() >= ksinkLimit) // moves rover down only if within sink limit
					   {
						   myRovers[i].model->MoveY((-kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRovers[i].model->SetX(kLeftLimit); // sets it back to initial position
						   myRovers[i].roverMovement = riseRover; // switches state to rise
					   }
					   break;
				     }
				     case riseRover:
				     {
					   if (myRovers[i].model->GetY() <= 0.0f) // moves rover up only if within limits
					   {
						   myRovers[i].model->MoveY((kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myRovers[i].roverMovement = moveRover; // switches state to move to right again
					   }
					   break;
				     }
				   }// end of rover movement

				   // switch statement that moves pick up from right to left, cause it to sink and return to intial position
				   switch (myPickUps[i].pickUpMovement)
				   {
				     case movePickUp:
				     {
					   if (myPickUps[i].model->GetX() > kLeftLimit) // moves pickup to the left only if within limits
					   {
						   myPickUps[i].model->MoveX((-kpickUpSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myPickUps[i].pickUpMovement = sinkPickUp; // changes state to sinking once beyond limit
					   }
					   break;
				     }
				     case sinkPickUp:
				     {
					   if (myPickUps[i].model->GetY() >= ksinkLimit) // moves pickup down only if within sink limit
					   {
						   myPickUps[i].model->MoveY((-kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myPickUps[i].model->SetX(kRightLimit); // sets it back to initial position
						   myPickUps[i].pickUpMovement = risePickUp; // switches state to rise
					   }
					   break;
				     }
				     case risePickUp:
				     {
					   if (myPickUps[i].model->GetY() <= 0.0f) // moves pickup up only if within limits
					   {
						   myPickUps[i].model->MoveY((kVerticalSpeed * kGameSpeed) * frameTime);
					   }
					   else
					   {
						   myPickUps[i].pickUpMovement = movePickUp; // switches state to move to left again
					   }
					   break;
				     }
				   }// end of pick up movement
			   }

			   // Moves each tyre on lane 1 and lane 3 between -50 and 50
			   for (int i = 0; i < kNumTyres ; i++)
			   {
			       if (myTyres[i].Lane1->GetX() >= kislandRightLimit && myTyres[i].Lane3->GetX() >= kislandRightLimit) // turns isMoving true if tyres greater than right limit
			       {
				      isMoving = true;
			       }
			       if (myTyres[i].Lane1->GetX() <= kislandLeftLimit && myTyres[i].Lane3->GetX() <= kislandLeftLimit) // turns isMoving false if tyres lower than left limit
			       {
				      isMoving = false;
			       }
			       if (isMoving)
			       {
					  // Moves lane 1 and lane 3 tyres to the left if true
				      myTyres[i].Lane1->MoveX((-ktyreSpeed * kGameSpeed) * frameTime);
					  myTyres[i].Lane3->MoveX((-ktyreSpeed * kGameSpeed) * frameTime);
			       }
			       else
			       {
					   // Moves lane 1 and lane 3 tyres to the right if false
				      myTyres[i].Lane1->MoveX((ktyreSpeed * kGameSpeed) * frameTime);
				      myTyres[i].Lane3->MoveX((ktyreSpeed * kGameSpeed) * frameTime);
			       }
			   }
			   // Moves each tyre on lane 2 and lane 4 between 50 and -50
			   for (int i = 0; i < kNumTyres; i++)
			   {
				   if (myTyres[i].Lane2->GetX() <= kislandLeftLimit && myTyres[i].Lane4->GetX() <= kislandLeftLimit) // turns isMoving true if tyres lower than left limit
				   {
					   isMoving = true;
				   }
				   if (myTyres[i].Lane2->GetX() >= kislandRightLimit && myTyres[i].Lane4->GetX() >= kislandRightLimit) // turns isMoving false if tyres greater than right limit
				   {
					   isMoving = false;
				   }
				   if (isMoving)
				   {
					   // Moves lane 2 and lane 4 tyres to the right if true
					   myTyres[i].Lane2->MoveX((ktyreSpeed * kGameSpeed) * frameTime);
					   myTyres[i].Lane4->MoveX((ktyreSpeed * kGameSpeed) * frameTime);
				   }
				   else
				   {
					   // Moves lane 2 and lane 4 tyres to the left if false
					   myTyres[i].Lane2->MoveX((-ktyreSpeed * kGameSpeed) * frameTime);
					   myTyres[i].Lane4->MoveX((-ktyreSpeed * kGameSpeed) * frameTime);
				   }
			   }

			   for (int i = 0; i < kNumFrogs; i++)
			   {
				   // stores current X and Z values of frog model
				   frogXs[i] = myFrogs[i].model->GetX();
				   frogZs[i] = myFrogs[i].model->GetZ();

				   if (frogZs[i]> kUpperLimit) // checks if current position of frogs crosses upper limit
				   {
					   // sets position to limit when crossed
					   myFrogs[i].model->SetZ(kUpperLimit);
					   myFrogs[i].dummy->SetZ(kUpperLimit);
				   }
				   if (frogZs[i] < kLowerLimit) // checks if current position of frogs crosses lower limit
				   {
					   // sets position to limit when crossed
					   myFrogs[i].model->SetZ(kLowerLimit);
					   myFrogs[i].dummy->SetZ(kLowerLimit);
				   }
				   if (frogXs[i] > kislandRightLimit) // checks if current position of frogs crosses right limit
				   {
					   // sets position to limit when crossed
					   myFrogs[i].model->SetX(kislandRightLimit);
					   myFrogs[i].dummy->SetX(kislandRightLimit);
				   }
				   if (frogXs[i] < kislandLeftLimit) // checks if current position of frogs crosses left limit
				   {
					   // sets position to limit when crossed
					   myFrogs[i].model->SetX(kislandLeftLimit);
					   myFrogs[i].dummy->SetX(kislandLeftLimit);
				   }
			   }

			   // rotates camera forward when up key is pressed and max rotation has not been reached
			   if (myEngine->KeyHeld(kUpKey)) 
			   {
				   if (myCamera->GetY() < kmaxRotation) 
				   {
					   myCamera->SetX(myFrogs[currentFrog].model->GetX());
					   myCamera->LookAt(myFrogs[currentFrog].model);
					   myCamera->MoveLocalY((krotationSpeed * kGameSpeed) * frameTime);
				   }
			   }

			   // rotate camera backward when down key is pressed and min rotation has not been reached
			   if (myEngine->KeyHeld(kDownKey))
			   {
				   if (myCamera->GetY() > kminRotation)
				   {
					   myCamera->SetX(myFrogs[currentFrog].model->GetX());
					   myCamera->LookAt(myFrogs[currentFrog].model);
					   myCamera->MoveLocalY((-krotationSpeed * kGameSpeed) * frameTime);
				   }
			   }

			   // reset camera position relative to current frog
			   if (myEngine->KeyHit(kresetCamera))
			   {
				   myCamera->SetPosition(myFrogs[currentFrog].model->GetX(), kcameraYPos, kcameraZPos);
				   myCamera->LookAt(myFrogs[currentFrog].model);
			   }

			   switch (myFrogs[currentFrog].currentFrogState)
			   {
			     case waiting:
			     {
					 // sets current frog state to crossing allowing movement and collision
					myFrogs[currentFrog].currentFrogState = crossing;
			        break;
			     }
			     case crossing:
			     {
					 if (myEngine->KeyHit(kLeftKey)) // switches frogs movement to moveleft when left key is pressed
					 {
						 currentMovement = moveLeft;
					 }
					 if (myEngine->KeyHit(kRightKey)) // switches frogs movement to moveright when right key is pressed
					 {
						 currentMovement = moveRight;
					 }
					 if (myEngine->KeyHit(kForwardKey)) // switches frogs movement to moveforward when forward key is pressed
					 {
						 currentMovement = moveForward;
					 }
					 if (myEngine->KeyHit(kBackwardKey)) // switches frogs movement to movebackward when backward key is pressed
					 {
						 currentMovement = moveBackward;
					 }

					 switch (currentMovement)
					 {
					    case moveLeft:
					    {
							if (frogXs[currentFrog] > kislandLeftLimit) // checks if frog is within bounds of island
							{
								// subtracts 10 to current position of frog and sets that as the new position
								float currentXPos = myFrogs[currentFrog].model->GetX();
								currentXPos = currentXPos - kstep;
								myFrogs[currentFrog].model->LookAt(islands[3]);
								myFrogs[currentFrog].dummy->SetX(currentXPos);
								myFrogs[currentFrog].model->SetX(currentXPos);
								currentMovement = resting; // switch to resting until key hit 
							}
							break;
					    }
						case moveRight:
						{
							if (frogXs[currentFrog] < kislandRightLimit) // checks if frog is within bounds of island
							{
								// adds 10 to current position of frog and sets that as the new position
								float currentXPos = myFrogs[currentFrog].model->GetX();
								currentXPos = currentXPos + kstep;
								myFrogs[currentFrog].model->LookAt(islands[4]);
								myFrogs[currentFrog].dummy->SetX(currentXPos);
								myFrogs[currentFrog].model->SetX(currentXPos);
								currentMovement = resting; // switch to resting until key hit 
							}
							break;
						}
						case moveForward:
						{
							if (frogZs[currentFrog] < kUpperLimit) // checks if frog is within bounds of island
							{
								// adds 10 to current position of frog and sets that as the new position
								Score += kscoreStep;
								float currentZPos = myFrogs[currentFrog].model->GetZ();
								currentZPos = currentZPos + kstep;
								myFrogs[currentFrog].model->LookAt(islands[2]);
								myFrogs[currentFrog].dummy->SetZ(currentZPos);
								myFrogs[currentFrog].model->SetZ(currentZPos);
								currentMovement = resting; // switch to resting until key hit 
							}
							break;
						}
						case moveBackward:
						{
							if (frogZs[currentFrog] > kLowerLimit) // checks if frog is within bounds of island
							{
								// subtracts 10 to current position of frog and sets that as the new position
								float currentZPos = myFrogs[currentFrog].model->GetZ();
								currentZPos = currentZPos - kstep;
								myFrogs[currentFrog].model->LookAt(islands[1]);
								myFrogs[currentFrog].dummy->SetZ(currentZPos);
								myFrogs[currentFrog].model->SetZ(currentZPos);
								currentMovement = resting; // switch to resting until key hit
							}
							break;
						}
					 }// end of Frog movement

					 switch (spider.currentSpiderState)
					 {
					    case delay:
						{
							spider.count++; // increments counter each frame
							if (spider.count == spider.MaxCount) // checks if counter is equal to max counter which is a random generated number
							{
								// sets position of spider in no longer safe zone and switches state to move
								spider.model->SetPosition(55.0f, 0.0f, 65.0f);
								spider.currentSpiderState = moveSpider;
							}
							break;
						}
						case moveSpider:
						{
							// moves spider to the left until it has hit left limit
							spider.model->MoveX((-krangeRoverSpeed * kGameSpeed) * frameTime);
							if (spider.model->GetX() < kLeftLimit)
							{
								spider.currentSpiderState = sink; // switches state to make spider go below surface
							}
							break;
						}
						case sink:
						{
							// sinks spider below surface until it reaches sink limit
							spider.model->MoveY((-kVerticalSpeed * kGameSpeed) * frameTime);
							if (spider.model->GetY() <= ksinkLimit)
							{
								// resets spider state and count until next max count is generated
								spider.currentSpiderState = delay;
								spider.count = 0;
								spider.MaxCount = (rand() % 10000 + 5000);
							}
							break;
						}
					 }// end of spider movements

//////////////////////////////////////////// collision detection and resolution begins /////////////////////////////////////////
				   for (int i = 0; i < kNumVehicles; i++)
				   {
					   // saves old positions of transits
					   transitXs[i] = myTransits[i].model->GetX();
					   transitZs[i] = myTransits[i].model->GetZ();

					   // checks for collision with current frog
					   if (sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
						   transitXs[i], transitZs[i], ktransitWidth, ktransitDepth))
					   {
						   // bool returns true and current frog state switches to dead
						   collision = true; 
						   myFrogs[currentFrog].currentFrogState = dead;
					   }
					   
					   // saves old positions of range rovers
					   rangeRoverXs[i] = myRangeRovers[i].model->GetX();
					   rangeRoverZs[i] = myRangeRovers[i].model->GetZ();

					   // checks for collision with current frog
					   if (sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
						   rangeRoverXs[i], rangeRoverZs[i], krangeRoverWidth, krangeRoverDepth))
					   {
						   // bool returns true and current frog state switches to dead
						   collision = true;
						   myFrogs[currentFrog].currentFrogState = dead;
					   }

					   // saves old positions of rovers
					   roverXs[i] = myRovers[i].model->GetX();
					   roverZs[i] = myRovers[i].model->GetZ();

					   // checks for collision with current frog
					   if (sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
						   roverXs[i], roverZs[i], kroverWidth , kroverDepth))
					   {
						   // bool returns true and current frog state switches to dead
						   collision = true;
						   myFrogs[currentFrog].currentFrogState = dead;
					   }

					   // saves old positions of pick ups
					   pickUpXs[i] = myPickUps[i].model->GetX();
					   pickUpZs[i] = myPickUps[i].model->GetZ();

					   // checks for collision with current frog
					   if (sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
						   pickUpXs[i], pickUpZs[i], kpickUpWidth, kpickUpDepth ))
					   {
						   // bool returns true and current frog state switches to dead
						   collision = true;
						   myFrogs[currentFrog].currentFrogState = dead;
					   }
				   }
				   for (int i = 0; i < kNumPlants; i++)
				   {
					   // checks collision with each plant in the entire row
					   if (sphere2sphere(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
						   plantXs[i], kplantZPos, kplantRadius))
					   {
						   // bools returns true and frof and dummy are set to its old positions
						   collision = true;
						   myFrogs[currentFrog].dummy->SetPosition(frogOldX, 0.0f, frogOldZ);
						   myFrogs[currentFrog].model->SetPosition(frogOldX, 0.0f, frogOldZ);
						   break; // stops for loop after successful collision
					   }
				   }
				   for (int i = 0; i < kNumTyres; i++)
				   {
					   if (myFrogs[currentFrog].model->GetZ() > kLane1ZPos && myFrogs[currentFrog].model->GetZ() < kLane2ZPos) // checks if current frog is within bounds of lane 1 and 2
					   {
						   // checks if frog is not collision with each tyre in the row
						   if (!(sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
							               myTyres[i].Lane1->GetX(), ktyreLane1ZPos, ktyreWidth, ktyreDepth)))
						   {
							   // sets position of dummy to current position and causes frogs to sink
							   myFrogs[currentFrog].dummy->SetPosition(myFrogs[currentFrog].model->GetX(), 0.0f, ktyreLane1ZPos);
							   myFrogs[currentFrog].currentFrogState = sinking;
						   }
						   else
						   {
							   // bool returns true and frog and dummy at positions are set on the tyre
							  collision = true;
							  myFrogs[currentFrog].dummy->SetX(myTyres[i].Lane1->GetX());
							  myFrogs[currentFrog].dummy->SetZ(myTyres[i].Lane1->GetZ());
						   }
					   }
					   if (myFrogs[currentFrog].model->GetZ() > kLane2ZPos && myFrogs[currentFrog].model->GetZ() < kLane3ZPos) // checks if current frog is within bounds of lane 2 and 3
					   {
						   // checks if frog is not collision with each tyre in the row
						   if (!(sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
							   myTyres[i].Lane2->GetX(), ktyreLane2ZPos, ktyreWidth, ktyreDepth)))
						   {
							   // sets position of dummy to current position and causes frogs to sink
							   myFrogs[currentFrog].dummy->SetPosition(myFrogs[currentFrog].model->GetX(), 0.0f, ktyreLane2ZPos);
							   myFrogs[currentFrog].currentFrogState = sinking;
						   }
						   else
						   {
							   // bool returns true and frog and dummy at positions are set on the tyre
							   collision = true;
							   myFrogs[currentFrog].dummy->SetX(myTyres[i].Lane2->GetX());
							   myFrogs[currentFrog].dummy->SetZ(myTyres[i].Lane2->GetZ());
						   }
					   }
					   if (myFrogs[currentFrog].model->GetZ() > kLane3ZPos && myFrogs[currentFrog].model->GetZ() < kLane4ZPos)// checks if current frog is within bounds of lane 3 and 4
					   {
						   // checks if frog is not collision with each tyre in the row
						   if (!(sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
							   myTyres[i].Lane3->GetX(), ktyreLane3ZPos, ktyreWidth, ktyreDepth)))
						   {
							   // sets position of dummy to current position and causes frogs to sink
							   myFrogs[currentFrog].dummy->SetPosition(myFrogs[currentFrog].model->GetX(), 0.0f, ktyreLane3ZPos);
							   myFrogs[currentFrog].currentFrogState = sinking;
						   }
						   else
						   {
							   // bool returns true and frog and dummy at positions are set on the tyre
							   collision = true;
							   myFrogs[currentFrog].dummy->SetX(myTyres[i].Lane3->GetX());
							   myFrogs[currentFrog].dummy->SetZ(myTyres[i].Lane3->GetZ());
						   }
					   }
					   if (myFrogs[currentFrog].model->GetZ() > kLane4ZPos && myFrogs[currentFrog].model->GetZ() < kIsland2ZPos) // checks if current frog is within bounds of lane 4 and island 2
					   {
						   // sets position of dummy to current position and causes frogs to sink
						   if (!(sphere2box(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius,
							   myTyres[i].Lane4->GetX(), ktyreLane4ZPos, ktyreWidth, ktyreDepth)))
						   {
							   myFrogs[currentFrog].dummy->SetPosition(myFrogs[currentFrog].model->GetX(), 0.0f, ktyreLane4ZPos);
							   myFrogs[currentFrog].currentFrogState = sinking;
						   }
						   else
						   {
							   // bool returns true and frog and dummy at positions are set on the tyre
							   collision = true;
							   myFrogs[currentFrog].dummy->SetX(myTyres[i].Lane4->GetX());
							   myFrogs[currentFrog].dummy->SetZ(myTyres[i].Lane4->GetZ());
						   }
					   }
				   }
				   // detects collision between current frog and spider
				   if (sphere2sphere(myFrogs[currentFrog].model->GetX(), myFrogs[currentFrog].model->GetZ(), kfrogRadius, 
					                  spider.model->GetX(), spider.model->GetZ(), spiderRadius))
				   {
					   // switches state of current frog to dead when true
					   collision = true;
					   myFrogs[currentFrog].currentFrogState = dead;
				   }
/////////////////////////////////////////////// collision detection and resolution ends /////////////////////////////////////

				   // checks to see if current frog has reached the second island, if so frog state is now safe
				   if (myFrogs[currentFrog].model->GetZ() >= ksafeZone)
				   {
					   myFrogs[currentFrog].currentFrogState = safe;
				   }
				   // if timer goes below zero state of current frog is now dead
				   if (countdown <= 0.0f)
				   {
					   myFrogs[currentFrog].currentFrogState = dead;
				   }
				   break;
			     }

			     case safe:
			     {
				   myFrogs[currentFrog].model->SetSkin(kstrFrogSafe); // Sets skin of current frog to blue

				   // checks to see if current frog within total number of frogs
				   if (currentFrog < kNumFrogs - 1)
				   {
					   // detaches camera from current dummy and attach to new dummy 
					   myCamera->DetachFromParent();
					   currentFrog++;
					   myCamera->AttachToParent(myFrogs[currentFrog].dummy);
					   myFrogs[currentFrog].currentFrogState = waiting;
				   }
				   else
				   {
					   currentState = over; // switches game state if all frogs are safe
				   }
				   myFrogs[currentFrog].currentFrogState = waiting;
				   break;
			     }
			     case dead:
				 {
					 myFrogs[currentFrog].model->SetSkin(kstrFrogDead); // Sets skin of current frog to red
					 myFrogs[currentFrog].model->ScaleY(ksquashFactor); // scales down the vertical height of frog

					 // checks to see if current frog within total number of frogs
					 if (currentFrog < kNumFrogs - 1)
					 {
						 // detaches camera from current dummy and attach to new dummy 
						 myCamera->DetachFromParent();
						 currentFrog++;
						 myCamera->AttachToParent(myFrogs[currentFrog].dummy);
						 countdown = 20.0f;
						 myFrogs[currentFrog].currentFrogState = waiting;
					 }
					 else
					 {
						 currentState = over; // switches game state if all frogs are safe
					 }
					 break;
				 }
				 case sinking:
				 {
					 myFrogs[currentFrog].model->SetSkin(kstrFrogDead); // Sets skin of current frog to red
					 myFrogs[currentFrog].model->MoveY((-kVerticalSpeed * kGameSpeed)* frameTime);

					 if (myFrogs[currentFrog].model->GetY() <= ksinkLimit)// moves current frog down until it hits sink limit after which it dies
					 {
						 myFrogs[currentFrog].currentFrogState = dead;
					 }
				 }
			   }// end of frog states in GameStates
			   break;
		   }// end of playing state in GameStates
		   case paused:
		   {
			   break;
		   }
		   case over:
		   {
			   // displays game over message if all frogs are dead
			   stringstream endText;
			   endText << kstrGameOver;
			   myFont->Draw(endText.str(), kfontOverX, kfontOverY);
			   break;
		   }
		}// end of GameStates

		//Inverts the value of the paused boolean when the p key is pressed
		if (myEngine->KeyHit(kPauseKey))
		{
			pause = !pause;
			// if true game state is paused and all movements are halted or else game keeps running
			if (pause)
			{
				currentState = paused;
			}
			else
			{
				currentState = playing;
			}
		}

		// Exits the game when ESC is pressed
		if (myEngine->KeyHit(kEscapeKey))
		{
			myEngine->Stop();
		}
	
	}
	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
bool sphere2box(float sxPos, float szPos, float sRad,
	            float bxPos, float bzPos, float bwidth, float bdepth)
{
	float minX = bxPos - bwidth / 2 - sRad;
	float maxX = bxPos + bwidth / 2 + sRad;
	float minZ = bzPos - bdepth / 2 - sRad;
	float maxZ = bzPos + bdepth / 2 + sRad;

	return(sxPos > minX && sxPos< maxX && szPos > minZ && szPos < maxZ);
}

bool sphere2sphere(float s1xPos, float s1zPos, float s1Rad, float s2xPos, float s2zPos, float s2Rad)
{
	float distX = s2xPos - s1xPos;
	float distZ = s2zPos - s1zPos;
	float distance = sqrt(distX * distX + distZ * distZ);

	return (distance < (s1Rad + s2Rad));
}
