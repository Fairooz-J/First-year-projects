// Fairooz Zahin Jalal  G-20808943
// Hovercar .cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <iomanip>
using namespace std;
using namespace tle;

// stores and switches between states depending on state of player and keyboard input
enum Gamestate { waiting, counting , playing, paused, over };

// creates different states within playing state for player to switch between
enum RaceState { check , displayCross ,increase, checkLap, increaseLap };

// defines axis of collision for detecting box sides
enum CollisionAxis {none, xAxis, zAxis};

// defines x and z components of a 2D vector
struct Vector2d
{
	float x;
	float z;
};

// Stores flare models as well as movement components for particle system
struct particle
{
	IModel* model;
	float moveVector[3];
};

// Calls isle model to the program along with their X and Z positions
struct Isle
{
	IModel* Model;
	float IxPos, IzPos;
};

// Calls wall models from text file to program with their X and Z positons
struct Wall
{
	IModel* Model;
	float WxPos, WzPos;
};

// Calls checkpoit models to the program as stores pole positions as 2D vectors
struct Checkpoint
{
	IModel* Model;
	IModel* Cross;
	float CxPos, CzPos;;
	Vector2d pole1, pole2;
};

// Stores and calls players and non-players to the program 
struct Hovercars
{
	IModel* Model; // Control movement and collisions
	float speed; // change speed
	float health; // control health loss
};

// Calls corner and sunken tanks to the program with X and Z positions
struct Smalltank
{
	IModel* Model;
	float x, z;
	float radius = 1.5f;
};

// Calls dummy models to the program for non-player movement and lap checks
struct Dummy
{
	IModel* Model;
	float DxPos, DzPos;
};

// Scalar multiplication of 2D vector 
Vector2d Scalar(float s, Vector2d v)
{
	return { s * v.x , s * v.z };
}

// Sum vectors to calculate momentum for player movement
Vector2d Sum3Vectors(Vector2d v1, Vector2d v2, Vector2d v3)
{
	return { v1.x + v2.x + v3.x , v1.z + v2.z + v3.z };
}

// Calculate vector length of momentum using pythagorus theorem for speed readout
float VectorLength(Vector2d v)
{
	float speed;
	speed = (v.x * v.x) + (v.z * v.z);
	speed = sqrt(speed);
	return {speed};
}

void CreateScene(IMesh* isle, vector <Isle>& Isles,
	IMesh* wall, vector<Wall>& Walls,
	IMesh* checkpoint, vector<Checkpoint>& Checkpoints,
	IMesh* walkway,
	IMesh* smalltank2, vector <Smalltank>& Cornertanks,
	IMesh* dummy, vector <Dummy>& Dummies,
	IMesh* cross, vector <Dummy>& Laps,
	IMesh* smalltank1, vector <Smalltank>& SunkenTanks, IMesh* garageMesh);

// intialize function to detect collision between player and lap points
bool Point2Box(float sxPos, float szPos, float bwidth, float bdepth,
	           float bxPos, float bzPos);

// intializes funtion to calculate boundaries of box and return box sides for collision resolution
CollisionAxis Sphere2box(float sOldX, float sOldZ, float sxPos, float szPos, float sRad,
	float bxPos, float bzPos, float bwidth, float bdepth);

// initializes function that detects collision between player and struts, tanks 
bool sphere2sphere(float s1xPos, float s1zPos, float s1Rad, float s2xPos, float s2zPos, float s2Rad);

// Displays stage number after passing through respective checkpoint
void DisplayStage(stringstream& text, int number, IFont* font);

// Displays circuit number after successful completion of all stages 
void DisplayLap(stringstream& text, int number, IFont* font);

// Displays exhaust flames from player during race
void DisplayFlare(float gravity, float newV, float time, particle flare[], Hovercars player, float pX, float pY, float pZ);

// Displays race time on the bottom left of the screen
void DisplayTime(stringstream& text, float time, IFont* font);

// Function called within game loop that controls non-player movement
void MoveNonPlayer(Hovercars nonplayers, vector <Dummy>& Dummies,
	               int& dummyCount, float carRadius, float time, bool& collision, float speed, float nonPlayerX);

// generates a random number by passing max and min range
float random(int rangeMin, int rangeMax)
{
	float result = (float)rand() / (float)(RAND_MAX + 1);
	result *= (float)(rangeMax - rangeMin);
	result += rangeMin;

	return result;
}

const float kplayerZPos              = -30.0f; // player position behind the first checkpoint
const float kplayerXPos              = -7.0f;  // player position behind the first checkpoint
const float kplayerYPos              = 3.0f;   // sets player position on the y axis
const float kcameraYPos              = 12.0f;  // camera position behind the player
const float kcameraZPos              = -30.0f; // camera position behind the player
const float kcameraZShift            = 24.0f;  // camera Z position shift for first person
const float kcameraYShift            = 2.5f;   // camera Y position shift for first person
const float kdummyCameraYPos         = 3.0f;   // sets position of dummy camera behind player
const float kdummyCameraZPos         = -50.0f; // sets position of dummy camera behind player
const float kplayerHealth            = 100.0f; // max health of player. gamer over once finished
const float kGameSpeed               = 200.0f; // game speed used for the hovercars
const float kmaxBoostCount           = 3.0f;   // disables boost once reached
const float kmaxCoolDownCount        = 5.0f;   // enables boost once count down is over
const float kStageFontX              = 500.0f; // x position where stage is displayed
const float kStageFontY              = 300.0f; // y position where stage is displayed
const float kLapFontX                = 550.0f; // x position where lap number is displayed
const float kLapFontY                = 250.0f; // y position where lapnumber is displayed
const float crossYPosition           = -3.0f;  // y position for the intial placement of cross
const float kphConversion            = 3.6f;   // converts meters per second to kilometers per hour
const float collisionAwarenessRadius = 2.0f;   // increments dummy value for non-player movement
const float kTimeFontX               = 100.0f; // displays time 100 units in the x direction
const float kTimeFontY               = 500.0f; // displays time 500 units in the y direction
const float spaceObjectScale         = 1.5f;   // scale space objects to a reasonable size
const float spaceObejectsRotation    = 90.0f;  // initial rotation for space objects
const float walkwayScale             = 4.0f;
const float sunkenTankYPos           = -4.0f;
const float crossScaleFactor         = 0.3f;

const int quadSize      = 5; // max quad size
const int maxNonPlayers = 3; // stores max number of non-players
const int scale         = 2; // converts TL units to meters per second 
const int fontSize      = 40;// font size for text streams displayed on screen
int currentQuad         = 0; // quad that shoots out

const string kstarMeshString         = "Stars.x";        // loads text string for stars mesh
const string kisleMeshString         = "IsleStraight.x"; // loads text string for isle mesh
const string kwallMeshString         = "Wall.x";         // loads text string for wall mesh
const string kcheckpointMeshString   = "Checkpoint.x";   // loads text string for checkpoint mesh
const string kgroundMeshString       = "ground.x";       // loads text string for ground mesh
const string khoverMeshString        = "race2.x";        // loads text string for hovercar mesh
const string kwalkwayMeshString      = "Walkway.x";      // loads text string for walkway mesh
const string ksmallTank2MeshString   = "TankSmall2.x";   // loads text string for small tank
const string ksmallTank1MeshString   = "TankSmall1.x";   // loads text string for small tank
const string kisleCornerMeshString   = "IsleCorner.x";   // loads text string for isle corner
const string kflareMeshString        = "Flare.x";        // loads text string for fare mesh
const string kdummyMeshString        = "Dummy.x";        // loads text tsring for dummy mesh
const string kcrossMeshString        = "Cross.x";        // loads text string for cross mesh
const string kinterstellarMeshString = "Interstellar.x"; // loads text string for interstellar mesh 
const string kplanetMeshString       = "Planet.x";       // loads text string for planet mesh
const string kgarageMeshString       = "GarageLarge.x";  // loads text string for garage mesh
const string knonPlayerSkin          = "RedGlow.jpg";    // sets skin for non-players
const string kbackdropSkin           = "ui_backdrop.jpg";// skin used for backdrops
const string fontStyle               = "Comic Sans MS";  // font style for text streams displayed on screen

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("./media 2");

	/**** Set up your scene here ****/
	IMesh* starsMesh        = myEngine->LoadMesh(kstarMeshString);         // loads mesh to the program and displays on screen
	IMesh* isleMesh         = myEngine->LoadMesh(kisleMeshString);         // loads mesh to the program and displays on screen
	IMesh* wallMesh         = myEngine->LoadMesh(kwallMeshString);         // loads mesh to the program and displays on screen
	IMesh* checkpointMesh   = myEngine->LoadMesh(kcheckpointMeshString);   // loads mesh to the program and displays on screen
	IMesh* groundMesh       = myEngine->LoadMesh(kgroundMeshString);       // loads mesh to the program and displays on screen
	IMesh* hoverMesh        = myEngine->LoadMesh(khoverMeshString);        // loads mesh to the program and displays on screen
	IMesh* walkwayMesh      = myEngine->LoadMesh(kwalkwayMeshString);      // loads mesh to the program and displays on screen
	IMesh* smallTank2Mesh   = myEngine->LoadMesh(ksmallTank2MeshString);   // loads mesh to the program and displays on screen
	IMesh* smallTank1Mesh   = myEngine->LoadMesh(ksmallTank1MeshString);   // loads mesh to the program and displays on screen
	IMesh* isleCornerMesh   = myEngine->LoadMesh(kisleCornerMeshString);   // loads mesh to the program and displays on screen
	IMesh* flareMesh        = myEngine->LoadMesh(kflareMeshString);        // loads mesh to the program and displays on screen
	IMesh* dummyMesh        = myEngine->LoadMesh(kdummyMeshString);        // loads mesh to the program and displays on screen
	IMesh* crossMesh        = myEngine->LoadMesh(kcrossMeshString);        // loads mesh to the program and displays on screen
	IMesh* interstellarMesh = myEngine->LoadMesh(kinterstellarMeshString); // loads mesh to the program and displays on screen
	IMesh* garageMesh       = myEngine->LoadMesh(kgarageMeshString);       // loads mesh to the program and displays on screen

	IModel* sky    = starsMesh->CreateModel();  // creates model using mesh and display om screen
	IModel* ground = groundMesh->CreateModel(); // creates model using mesh and display om screen
	IMesh* planetMesh = myEngine->LoadMesh(kplanetMeshString);

	//////////--------- Space objects---------//////////////////////////
	const int  spaceObjects = 3; // creates set number of additional space objects
	// Planets
	IModel* Planets[spaceObjects]; // stores planets to be loaded on screen
	float planetXs[spaceObjects] = { -20.0f , 400.0f, -500.0f }; // loads planets at their initial X positions
	float planetYs[spaceObjects] = { 70.0f , 35.0f , 400.0f }; // loads planets at their intial Y positions
	float planetZs[spaceObjects] = { 500.0f , 700.0f , 400.0f }; // loads planets at their initial Z positions
	float planetScale[spaceObjects] = { 110.0f,  50.0f , 80.0f }; // loads planets with their individual scales
	string planetSkin[spaceObjects] = { "Sun.jpg" , "", "Saturn.jpg" }; // loads planets with their individual skins

	// creates planets and loads them onto the screen 
	for (int i = 0; i < spaceObjects; i++)
	{
		Planets[i] = planetMesh->CreateModel(planetXs[i], planetYs[i], planetZs[i]);
		Planets[i]->Scale(planetScale[i]); // set scale
		Planets[i]->SetSkin(planetSkin[i]); // set skin
	}

	IModel* Interstellars[spaceObjects]; // loads interstellar models in an array
	float InterstellarYs[spaceObjects] = { 70.0f , -30.0f , -30.0f }; // loads intial Y positions of interstellars
	float InterstellarZs[spaceObjects] = { 100.0f , -130.0f , -130.0f }; // loads intial Z positions of interstellars
	IModel* IntDummies[spaceObjects]; // creates an array of interstellar dummies for rotation
	float IntDummyXs[spaceObjects] = { -20.0f ,-50.0f ,200.0f }; // loads X positions od dummies into an array
	float IntDummyYs[spaceObjects] = { 200.0f ,100.0f, 100.0f }; // loads Y positions od dummies into an array
	float IntDummyZs[spaceObjects] = { 500.0f ,100.0f ,-66.0f }; // loads Z positions od dummies into an array
	float IntDummySpeed[spaceObjects] = {20.0f, -20.0f, -15.0f}; // gives each dummy their own speed of rotation

	// creates interstellars and attches them to dummy models
	for (int i = 0; i < spaceObjects; i++)
	{
		IntDummies[i] = dummyMesh->CreateModel(IntDummyXs[i], IntDummyYs[i], IntDummyZs[i]);
		Interstellars[i] = interstellarMesh->CreateModel(0.0f, InterstellarYs[i], InterstellarZs[i]);
		Interstellars[i]->Scale(spaceObjectScale);
		Interstellars[i]->RotateY(spaceObejectsRotation);
		Interstellars[i]->AttachToParent(IntDummies[i]);
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float playerSpeed = 20.0f; // speed for rotation and movement
	Hovercars player; // creates player using struct properties
	player.Model = hoverMesh->CreateModel(kplayerXPos, kplayerYPos, kplayerZPos); // loads player model on the program
	player.speed = playerSpeed; // sets player speed to be controlled
	player.health = kplayerHealth; // sets player health to collision resolution

	Dummy cameraDummy; // creates camera dummy using dummy struct properties
	cameraDummy.Model= dummyMesh->CreateModel(0.0f, kdummyCameraYPos, kdummyCameraZPos); // loads camera dummy onto the program

	float nonPlayerXs[maxNonPlayers] = { 10.0f, 5.0f, 15.0f }; // loads non-players at their initial X positions
	float nonPlayerZs[maxNonPlayers] = { -35.0, -40.0f, -45.0f }; // loads non-players at their initial Z positions
	float nonPlayerSpeed[maxNonPlayers] = { 25.0f, 21.0, 16.0f }; // gives each non-player their own speed
	bool dummyCollision[maxNonPlayers] = { false, false, false }; // detect dummy collision for each non-player
	int dummyCount[maxNonPlayers] = { 0,0,0 }; // increment dummy values for each non-player
    Hovercars nonPlayers[maxNonPlayers]; // creates non-players usring Hovercar struct properties
	for (int i = 0; i < maxNonPlayers; i++)
	{
		nonPlayers[i].Model = hoverMesh->CreateModel(nonPlayerXs[i], 0.0f, nonPlayerZs[i]); // loads model with X and Z positions
		nonPlayers[i].Model->SetSkin(knonPlayerSkin); // Sets different skin from player
		nonPlayers[i].speed = nonPlayerSpeed[i]; // sets speed for movement
	}
	
	particle flare[quadSize]; // creates fare particele using struct properties
	for (int i = 0; i < quadSize; i++)
	{
		flare[i].model = flareMesh->CreateModel(player.Model->GetX(), player.Model->GetY(), player.Model->GetZ());
		// loads model with player positions on the screen
	}

	vector <Isle> Isles;             // stores isle models for collision detection and resolution
	vector <Wall> Walls;             // stores walls to create wall/isle  section for collision detection and resolution
	vector <Checkpoint> Checkpoints; // used in the race circuit to signify a stage 
	vector <Smalltank> CornerTanks;  // placed alongside track to suggest corners
	vector <Smalltank> SunkenTanks;  // placed on walkways as obstacle to be avoided
	vector <bool> RaceStage;         // checks if checkpoints have beeen passed in order
	vector <Dummy> Dummies;          // creates point for non-players to look at and move towards
	vector <Dummy> Laps;             // creates multiple circuits of race track
	vector <bool> LapStage;          // checks if player finishes all stages of a lap in order
	CreateScene(isleMesh, Isles, 
		        wallMesh, Walls, 
		        checkpointMesh, Checkpoints, 
		        walkwayMesh, smallTank2Mesh, CornerTanks,
		        dummyMesh, Dummies,
		        crossMesh, Laps, 
		        smallTank1Mesh,SunkenTanks, garageMesh); 
	// calls function that reads objects from a text file and outputs on screen
	// takes meshes and vectors as parameters for storing scene objects

	// checks bool values against number of checkpoints so multiple checkpoints can be added
	for (int i = 0; i < Checkpoints.size(); i++)
	{
		RaceStage.push_back(false); 
	}

	// checks bool values against number of laps making number of laps dynamic
	for (int i = 0; i < Laps.size(); i++)
	{
		LapStage.push_back(false);
	}

	// creates game camera which is attached to a dummy to allow rotation for player
	ICamera* Camera = myEngine->CreateCamera(kManual, 0.0f, kcameraYPos, kcameraZPos);
	Camera->AttachToParent(cameraDummy.Model);

	// loads font style and size to be displayed on the screen
	IFont* myFont = myEngine->LoadFont(fontStyle, fontSize);

	Vector2d momentum{ 0.0f, 0.0f }; // controls players momentum 
	Vector2d thrust{ 0.0f, 0.0f };   // used to increase player speed steadily depending on keyboard input
	Vector2d drag{ 0.0f, 0.0f };     // used to decrease player speed steadily depending on keyboard input

	float matrix[4][4];                    // retrieves matrix of player for momentum
	float thrustFactor         = 0.102f ;  // provides forward and backward movement of player
    float dragCoefficient      = -0.095f;  // works against thrust to slow down player
	const float kboostPower    = 0.5f;     // increases thrust causing player to accelerate
	const float koverheatPower = 200.0f;   // reduces thrust to 0 causing player to decelerate
	const float kcameraSpeed   = 10.0f;    // controls speed for panning camera
	float gravity              = -2000.0f; // provides downward force for particle system
	float newVelocity          = 0.0f;     // moves particles using gravity and the y component of vector

	const EKeyCode kForwardKey        = Key_Up;     // pans camera forward
	const EKeyCode kBackwardKey       = Key_Down;   // pans camera backward
	const EKeyCode kLeftKey           = Key_Left;   // pans camera ro the left  
	const EKeyCode kRightKey          = Key_Right;  // pans camera to the right
	const EKeyCode kResetKey          = Key_1;      // resets camera position to be behind player
	const EKeyCode kFirstPersonKey    = Key_2;      // enables first person view
  	const EKeyCode kPauseKey          = Key_P;      // stores key code that pauses game on input
	const EKeyCode kEscapeKey         = Key_Escape; // stores key code that exits game on input
	const EKeyCode kForwardThrustKey  = Key_W;      // accelerates player forward
	const EKeyCode kBackwardThrustKey = Key_S;      // decelerates player backward
	const EKeyCode kLeftRotateKey     = Key_A;      // rotates camera and player to the left
	const EKeyCode KRightRotateKey    = Key_D;      // rotates camera and player to the right
	const EKeyCode kInitiateKey       = Key_Space;  // intiates countdown as well boost during gameplay

	const float strutWidth          = 4.0f;   // used to create a bounding box to check for collision between player
	const float strutDepth          = 4.0f;   // used to create a bounding box to check for collision between player
	const float strutRadius         = 0.5f;   // used to check collisions between player and checkpoint strut
	const float carCollisionRadius  = 5.0f;   // used to check collisions with checkpoints, walls, tanks etc
	const float wallWidth           = 2.5f;   // used to create a bounding box to check for collision between player
	const float wallDepth           = 11.0f;  // used to create a bounding box to check for collision between player
	const float lapCollisionWidth   = 20.0f;  // used to create a bounding box to check for collision between player
	const float lapCollisionDepth   = 10.0f;  // used to create a bounding box to check for collision between player
	const float kmaxRotationX       = 45.0f;  // stops camera rotation right beyond this point
	const float kminRotationX       = -45.0f; // stops camera rotation left beyond this point
	const float kmaxRotationY       = 85.0f;  // stops camera rotation upward after this point
	const float kminRotationY       = -85.0f; // stops camera rotation downward betong this point
	const float koverheatCount      = 2.0f;   // displays warning once count is reached
	const float kminBoostHealth     = 30.0f;  // de-activates boost once health goes under limit
	const float kminHealthCount     = -2.0f;  // slows down health reduction during continuous collision
	const float kmaxCrossCount      = 3.0f;   // checkpoint disappears once max count reached
	const float kmaxVerticalHeight  = 3.0f;   // players moves downward once height is reached
	const float kminVerticalHeight  = 1.0f;   // player moves upwards once height is reached
	const float kminPlayerZRotation = -15.0f; // leans player to the right
	const float kmaxPlayerZRotation = 15.0f;  // leans player to the left
	const float kmaxPlayerXRotation = 15.0f;  // leans player forwards
	const float crossYShift         = 7.0f;   // displays cross above checkpoint

	Gamestate currentGameState = waiting;
	RaceState currentRaceState = check;

	int checkpointNumber = 0; // checks collision with each checkpoint
	int stageNumber      = 0; // checks which stage is complete and displays output
	int startCount       = 0; // counts down during waiting stage
	int lapNumber        = 0; // checks number of laps completed
	int circuitNumber    = 0; // displays lap number 

	float gameTimer               = 0.0f; // records race time and diplays on program
	float boostCount              = 3.0f; // records time for boost 
	float coolDownCount           = 5.0f; // allows booster to cool down disabling boost
	float healthCount             = 0.0f; // controls health loss during continuous collision
	float crossCount              = 0.0f; // displays cross above checkpoint for set time

	float cameraRotationX         = 0.0f;  // controls the angle of camera rotation on the X axis
	float cameraRotationY         = 0.0f;  // controls the angle of camera rotation on the y axis
	float playerRotationZ         = 0.0f;  // controls angle of rotation for player's local Z
	float playerRotationX         = 0.0f;  // controls angle or rotation for player's local X
	float verticalRiseFactor      = 0.004; // moves player upwards with respect to gravity
	float verticalSinkFactor      = 0.005; // moves player downward faster due to effects of gravity
	float wallCollisonAbsorption  = 3.0f;  // reduces momentum of player after bounce
	float CollisionAbsorption     = 1.0f;  // reduces momentum of player after bounce with struts or non-players

	bool checkpointCollision = false; // checks collision with each checkpoint
	bool pause               = false; // chenges between game pause state and play state
	bool boost               = true;  // controls enabling and disabling of boost
	bool up                  = true;  // controls floating movement of player
	bool health              = false; // control health loss during continuous collision
	bool lapCollision        = false; // check collisions with laps
	bool nonPlayerMovement   = false; // controls non player movement laps
						      
	const int khealthXFont        = 900;  // displays health 900 units across
	const int khealthYFont        = 50;   // displays health 50 units down
	const int kspeedXFont         = 900;  // displays speed 900 units across
	const int kspeedYFont         = 500;  // displays speed 500 units down
	const int kstartXFont         = 500;  // displays start texr 500 units across
	const int kstartYFont         = 200;  // displays start text 200 units down
	const int kcountdownXFont     = 625;  // displays countdown 625 units across
	const int kcountdownYFont     = 200;  // displays countdown 200 units down
	const int kboostXFont         = 525;  // displays boost activated text 525 units across
	const int kboostYFont         = 100;  // displays boost activated text 100 units down
	const int kwarningXFont       = 325;  // displays warning text 325 units across
	const int kwarningYFont       = 150;  // displays warning text 150 units down
	const int kboostInactiveXFont = 325;  // displays when boost is inactive 325 units across
	const int kboostInactiveYFont = 100;  // displays when boost is inactive 100 units down
	const int framecount          = 400;  // sets intial count up frame
	const int frameincrement      = 200;  // adds on to initial frame count to display countdown

	const string khealthString        = "Health remaining: ";                 // displays string for health remaining
	const string kspeedString         = "Speed: ";                            // displays string for speed 
	const string kspeedUnitString     = " KPH";                               // displays string for speed unit
	const string kstartString         = "Hit Space to start";                 // displays string for starting game
	const string kboostString         = "Boost activated";                    // displays string for boost activat5ed
	const string kwarningString       = "1 second before booster overheats!"; // displays warning string
	const string kboostInactiveString = "You cannot use boost anymore";       // displays string when boost is inactive
	const string kcountdown3String    = "3";                                  // displays countdown on screen
	const string kcountdown2String    = "2";                                  // displays countdown on screen
	const string kcountdown1String    = "1";                                  // displays countdown on screen
	const string kgoString            = "GO!";                                // displays GO! text on screen 

	// loads sprite on screen in position of health
	ISprite* healthBackdrop = myEngine->CreateSprite(kbackdropSkin, khealthXFont, khealthYFont);

	myEngine->Timer();
	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/
		float frameTime = myEngine->Timer(); // stores time taken to render previous frame
		float speed = VectorLength(momentum); // calculates length of momentum for speed readout 

		// rotation of each dummy with their associated speed
		for (int i = 0; i < spaceObjects; i++)
		{
			IntDummies[i]->RotateLocalY(IntDummySpeed[i] * frameTime);
		}

		// intialise text streams
		stringstream startText;           // displays text stream for countdown
		stringstream stageText;           // displays text stream for number of stages completed
		stringstream lapText;             // displays text stream for number of laps taken
		stringstream healthDisplay;       // displays text stream for player lifespan
		stringstream boostActiveText;     // displays text stream for when boost is inactive
		stringstream boostInactiveText;   // displays text stream for when boost is active
		stringstream overheatWarningText; // displays text stream for overheating of booster 
		stringstream speedText;           // displays text stream for speed
		stringstream timeText;            // displays text stream for race time
		
		// displays health of player using string stream on the top right 
		healthDisplay << khealthString << player.health;
		myFont->Draw(healthDisplay.str(), khealthXFont, khealthYFont, kRed);
		healthDisplay.str("");

		// displays and updates current speed of player on the bottom right
		speedText << fixed << setprecision(2);
		speedText << kspeedString << int(fabs(speed * scale * kphConversion)) << kspeedUnitString;
		myFont->Draw(speedText.str(), kspeedXFont, kspeedYFont, kLightGrey);
		speedText.str("");

		// Calls function that displays race time on the bottom left
		DisplayTime(timeText, gameTimer, myFont);

		// displays number of laps and laps finishes at the centre of the screen
		DisplayLap(lapText, circuitNumber, myFont);

		// store current positions of player
		float playerCurrentX = player.Model->GetX();
		float playerCurrentZ = player.Model->GetZ();
		float playerCurrentY = player.Model->GetY();

		switch (currentGameState)
		{
			case waiting:
			{
				// displays text at thecentre of the screen to invoke game initiation
				startText << kstartString;
				myFont->Draw(startText.str(), kstartXFont, kstartYFont);
				startText.str("");

				// swtiches state to countdown with keyboard input
				if (myEngine->KeyHit(kInitiateKey)) currentGameState = counting;
				break;
			}
			case counting:
			{
				// starts down up of frames to display countdown from 3
				startCount++;
				
				// switches states to playing once countdown is over
				if (startCount >= framecount + frameincrement + frameincrement + frameincrement + frameincrement)
				{
					currentGameState = playing;
				}
				else if (startCount >= framecount + frameincrement + frameincrement + frameincrement ) // checks start count
				{
					// displays countdown on the centre of the screen
					startText << kgoString;
					myFont->Draw(startText.str(), kcountdownXFont, kcountdownYFont);
					startText.str(""); // clears text stream
				}
				else if (startCount >= framecount + frameincrement + frameincrement) // checks start count
				{
					// displays countdown on the centre of the screen
					startText << kcountdown1String;
					myFont->Draw(startText.str(), kcountdownXFont, kcountdownYFont);
					startText.str(""); // clears text stream
				}
				else if (startCount >= framecount + frameincrement) // checks start count
				{
					// displays countdown on the centre of the screen
					startText << kcountdown2String;
					myFont->Draw(startText.str(), kcountdownXFont, kcountdownYFont);
					startText.str(""); // clears text stream
				}
				else if (startCount >= framecount) // checks start count
				{
					// displays countdown on the centre of the screen
					startText << kcountdown3String;
					myFont->Draw(startText.str(), kcountdownXFont, kcountdownYFont);
					startText.str(""); // clears text stream
				}

				break;
			}
			case playing:
			{
				// Start timer
				gameTimer += frameTime;

				// Camera movement
				if (myEngine->KeyHeld(kForwardKey)) Camera->MoveLocalZ(kcameraSpeed * frameTime); // moves camera forward
				if (myEngine->KeyHeld(kBackwardKey)) Camera->MoveLocalZ(-kcameraSpeed * frameTime); // moves camera backward
			
				if (myEngine->KeyHeld(kRightKey)) Camera->MoveLocalX(kcameraSpeed * frameTime); // rotates camera to the right
				if (myEngine->KeyHeld(kLeftKey)) Camera->MoveLocalX(-kcameraSpeed * frameTime); // rotates camera to the left

				// Camera rotation
				int mouseMoveX = myEngine->GetMouseMovementX(); // stores mouse input in the X axis
				int mouseMoveY = myEngine->GetMouseMovementY(); // stores mouse input in the Y axis

				float xRotation = (mouseMoveX * kcameraSpeed)  * frameTime; // stores mouse movement and speed
				float yRotation = (mouseMoveY * kcameraSpeed)  * frameTime; // stores mouse movement and speed

				myEngine->StartMouseCapture(); // removes cursor pointer from screen 
				Camera->ResetOrientation(); // resets camera angles each frame to avoid tilting

				// checks if mouse is being moved in the x direction and is less that upper limit
				// stores angle of rotation each frame
				if (yRotation > 0)      {if (cameraRotationX <  kmaxRotationX)  cameraRotationX += yRotation;}
				// checks if mouse is being moved in the x direction and is more that lower limit
				// stores angle of rotation each frame
				else if (yRotation < 0) {if (cameraRotationX >  kminRotationX)  cameraRotationX += yRotation;}

				// checks if mouse is being moved in the y direction and is less that upper limit
				// stores angle of rotation each frame
				if (xRotation > 0)      {if (cameraRotationY < kmaxRotationY)  cameraRotationY += xRotation;}
				// checks if mouse is being moved in the y direction and is more that lower limit
				// stores angle of rotation each frame
				else if (xRotation < 0) {if (cameraRotationY > kminRotationY)  cameraRotationY += xRotation;}

				Camera->RotateX(cameraRotationX); // rotates camera in x axis using angle of rotation
				Camera->RotateY(cameraRotationY); // rotates camera in y axis using angle of rotation

				// Camera reset
				if (myEngine->KeyHit(kResetKey)) // reads keyboard input
				{
					Camera->DetachFromParent(); // detaches from camera dummy
					Camera->ResetOrientation(); // resets angles in all directions
					Camera->SetPosition(0.0f, kcameraYPos, kcameraZPos); // sets position behind player
					Camera->AttachToParent(cameraDummy.Model); // attaches to camera dummy again

					cameraRotationY = 0.0f; // camera angles are reset to avoid tilting
					cameraRotationX = 0.0f; // camera angles are reset to avoid tilting
				}

				// Camera first person
				if (myEngine->KeyHit(kFirstPersonKey)) // reads keyboard input
				{ 
					Camera->DetachFromParent(); // detaches from camera dummy
					Camera->ResetOrientation(); // resets angles in all directions
					Camera->SetPosition(0.0f, kcameraYPos - kcameraYShift, kcameraZPos + kcameraZShift);// place camera centre of player
					Camera->AttachToParent(cameraDummy.Model); // attaches to camera dummy again

					cameraRotationY = 0.0f; // camera angles are reset to avoid tilting
					cameraRotationX = 0.0f; // camera angles are reset to avoid tilting
				}

				// store players Local Z axis rotation
				float LocalZRotation = player.speed * frameTime; 

				// Hovercar rotation
				if (myEngine->KeyHeld(kLeftRotateKey)) // reads keyboard input
				{
					player.Model->RotateY(-player.speed * frameTime); // rotates player to the left
					if (LocalZRotation > 0.0f && playerRotationZ < kmaxPlayerZRotation) // checks if angle of rotation is within limits
					{
						// increments angle of rotation each frame
						playerRotationZ += LocalZRotation;
						player.Model->RotateLocalZ(LocalZRotation); // tilts player to the left
					}
					cameraDummy.Model->RotateY(-player.speed * frameTime); // rotates camera dummy to the left
				}
				else // resets player tilting when key held is released
				{
					if ( playerRotationZ > 0.0f) // checks if angle of rotation is within limits
					{
						// decrements angle of rotation from previous frame
						playerRotationZ -= LocalZRotation;
						player.Model->RotateLocalZ(-LocalZRotation); // tilts player to right until angle of rotation is zero
					}
				}

				if (myEngine->KeyHeld(KRightRotateKey)) // reads keyboard input
				{
					player.Model->RotateY(player.speed * frameTime); // rotates player to the left
					if (LocalZRotation > 0.0f && playerRotationZ > kminPlayerZRotation) // checks if angle of rotation is within limits
					{
						// decrements angle of rotation from previous frame
						playerRotationZ -= LocalZRotation;
						player.Model->RotateLocalZ(-LocalZRotation); // tilts player to the right
					}
					cameraDummy.Model->RotateY(player.speed * frameTime); // rotates camera dummy to the right
				}
				else // resets player tilting when key held is released
				{
					if (playerRotationZ < 0.0f) // checks if angle of rotation is within limits
					{
						// increments angle of rotation from previous frame
						playerRotationZ += LocalZRotation;
						player.Model->RotateLocalZ(LocalZRotation); // tilts player to left until angle of rotation is zero
					}
				}

				// Get face vector (local z axis)
				player.Model->GetMatrix(&matrix[0][0]);
				Vector2d facingVector = { matrix[2][0], matrix[2][2] };

				// store players local X axis rotation
				float LocalXRotation = player.speed * frameTime;

				// calculate thrust (based on keyboard input)
				if (myEngine->KeyHeld(kForwardThrustKey)) // reads keyboard input
				{
					// multiplies facing vector with positive thrust factor
					thrust = Scalar( thrustFactor * frameTime, facingVector );
					if (LocalXRotation > 0.0f && playerRotationX < kmaxPlayerXRotation) // checks if angle of rotation is within limits
					{
						// increments angle of rotation from previous frame
						playerRotationX += LocalXRotation;
						player.Model->RotateLocalX(LocalXRotation); // tilts player slightly forward
					}
				}
				else if (myEngine->KeyHeld(kBackwardThrustKey)) // reads keyboard input
				{
					// multiplies facing vector with negative thrust factor
					thrust = Scalar(-thrustFactor * frameTime, facingVector ); 
				}
				else
				{
					thrust = { 0.0f, 0.0f }; // no player movement 
					if (LocalXRotation > 0.0f && playerRotationX > 0.0f)// checks if angle of rotation is within limits
					{
						// decrements angle of rotation from previous frame
						playerRotationX -= LocalXRotation;
						player.Model->RotateLocalX(-LocalXRotation); // tilts player backward until angle is zero
					}
				}

				// calculate drag (based on previous momentum)
				drag = Scalar(dragCoefficient * frameTime, momentum );

				// check if spacebar is held and enable boost 
				if (myEngine->KeyHeld(kInitiateKey) && boost == true)
				{
					boostActiveText << kboostString; // displays when boost is active at the center of the screen
					myFont->Draw(boostActiveText.str(), kboostXFont, kboostYFont);
					boostActiveText.str("");
					boostCount -= frameTime; // starts boost countdown 
					thrust = Scalar((thrustFactor + kboostPower) * frameTime, facingVector); 
					// multiplies facing vector with thrust factpr and additional boost factor
				}
				if (boostCount <= 0.0f) // checks if countdown has reached zero
				{
					boost = false; // turns off boosters
					drag = Scalar((dragCoefficient - koverheatPower) * frameTime, momentum); // increases drag to slow down player
					boostCount = kmaxBoostCount; // resets boost count
					coolDownCount = kmaxCoolDownCount; // resets cool down count
				}
				else if (boostCount <= koverheatCount) // checks if boost count reached 1
				{
					// displays warning at the center of the screen
					overheatWarningText << kwarningString; 
					myFont->Draw(overheatWarningText.str(), kwarningXFont, kwarningYFont);
					overheatWarningText.str("");
				}
				if (coolDownCount > 0.0f) coolDownCount -= frameTime; // starts cool down count for boosters
				if (coolDownCount <= 0.0f) boost = true; // enables boost once count is over
				if (player.health < kminBoostHealth) // checks if player health is less than minimum boost health
				{
					boost = false; // deactivates boost function
					boostInactiveText << kboostInactiveString; // displays string saying boost is no longer available
					myFont->Draw(boostInactiveText.str(), kboostInactiveXFont, kboostInactiveYFont);
					boostInactiveText.str("");
				}

				// calculate momentum (based on thrust , drag and previous momentum)
				momentum = Sum3Vectors(momentum, thrust, drag);

				// calculate new position of player
				float playerNewX = playerCurrentX + momentum.x ;
				float playerNewZ = playerCurrentZ + momentum.z ;

				bool wall_isle_Collision = false; // resets wall collision to zero each frame

				// check for collision with wall/isle section
				for (int i = 0; i < Walls.size(); i++)
				{
					// checks for collision and collision axis
 					CollisionAxis collisionResponse = Sphere2box(playerCurrentX, playerCurrentZ, playerNewX, playerNewZ, carCollisionRadius,
						                                         Walls[i].WxPos, Walls[i].WzPos, wallWidth, wallDepth);

					// collision resolution
					if (collisionResponse == xAxis) // checks which axis player collided with
					{
						momentum.x = -momentum.x / wallCollisonAbsorption; // negates momentum with some absorption
						if (player.health > 0.0f & !health) player.health--; // reduce player health
						health = true;
						wall_isle_Collision = true; // sets bool to true due to collision
						break;
					}
					else if (collisionResponse == zAxis) // checks which axis player collided with
					{
						
						momentum.z = -momentum.z / wallCollisonAbsorption; // negates momentum with some absorption
						if (player.health > 0.0f && !health) player.health--; // reduce player health
						health = true;
						wall_isle_Collision = true; // sets bool to true due to collision
						break;
					}
				}
				if (health) // checks if bool is true 
				{
					healthCount -= frameTime; // starts health timer to control loss of player health
				}
				if (healthCount <= kminHealthCount)
				{
					health = false;
					healthCount = 0.0f; // resets health count to zero
				}
				if (!wall_isle_Collision)
				{
					// sets player to new position if there is no collision
					player.Model->SetPosition(playerNewX, 0.0f, playerNewZ); 
					// sets dummy camera to new position if there is no collision
					cameraDummy.Model->SetPosition(playerNewX, 0.0f, playerNewZ);
				}

				// Move Hover car
				//player.Model->Move((momentum.x * kGameSpeed) * frameTime, 0.0f, (momentum.z* kGameSpeed)* frameTime);

				// Calls function that displays exhaust flames from player engine
				DisplayFlare( gravity,  newVelocity, frameTime, flare, player,
					          playerCurrentX, playerCurrentY, playerCurrentZ);

				//Hover car floating 
				if (up == true) // checks if player is going up
				{
					playerCurrentY += verticalRiseFactor; // adds height to player's vertical position 
					player.Model->SetY(playerCurrentY); // sets player to new position
					if (player.Model->GetY() > kmaxVerticalHeight) // checks if max height is reached
					{
						up = false; // moves player down
					}
				}
				if (up == false) // checks if player is goind down
				{ 
					playerCurrentY -= verticalSinkFactor; // subtracts height to player's vertical position 
					player.Model->SetY(playerCurrentY); // sets player to new position
					if (player.Model->GetY() < kminVerticalHeight) // checks if min height is reached
					{
						up = true; // moves player up
					}
				}

				// Non-player movement
				nonPlayerMovement = true;
				if (nonPlayerMovement) // checks if non-players should move
				{
					for (int i = 0; i < maxNonPlayers; i++)
					{ 
						// calls function for each non-player for individual movement 
						// passing model and their own dummy count and collision bool and positions as parameters
						MoveNonPlayer(nonPlayers[i], Dummies,
							dummyCount[i], carCollisionRadius, frameTime, dummyCollision[i], nonPlayerSpeed[i], nonPlayerXs[i]);
					}
				}

				// Non-player collision
				for (int i = 0; i < maxNonPlayers; i++)
				{
					// checks collision which each non-players using current positions
					if (sphere2sphere(player.Model->GetX(), player.Model->GetZ(), carCollisionRadius,
						nonPlayers[i].Model->GetX(), nonPlayers[i].Model->GetZ(), carCollisionRadius))
					{
						momentum.x = -momentum.x / CollisionAbsorption; // negates momentum with absorption
						momentum.z = -momentum.z / CollisionAbsorption; // negates momentum with absorption
						if (player.health > 0.0f) player.health--; // reduce player health
					}
				}

				// Corner tank collisions
				for (int i = 0; i < CornerTanks.size(); i++)
				{
					// checks collision which each non-players using current positions
					if (sphere2sphere(player.Model->GetX(), player.Model->GetZ(), carCollisionRadius,
						CornerTanks[i].Model->GetX(), CornerTanks[i].Model->GetZ(), CornerTanks[i].radius))
					{
						momentum.x = -momentum.x / CollisionAbsorption; // negates momentum with absorption
						momentum.z = -momentum.z / CollisionAbsorption; // negates momentum with absorption
						if (player.health > 0.0f) player.health--; // reduce player health
					}
				}

				// Sunken tank collisions
				for (int i = 0; i < SunkenTanks.size(); i++)
				{
					// checks collision which each non-players using current positions
					if (sphere2sphere(player.Model->GetX(), player.Model->GetZ(), carCollisionRadius,
						SunkenTanks[i].Model->GetX(), SunkenTanks[i].Model->GetZ(), SunkenTanks[i].radius))
					{
						momentum.x = -momentum.x / CollisionAbsorption; // negates momentum with absorption
						momentum.z = -momentum.z / CollisionAbsorption; // negates momentum with absorption
						if (player.health > 0.0f) player.health--; // reduce player health
					}
				}

				// check collision with each checkpoint strut
				for (int i = 0; i < Checkpoints.size(); i++)
				{
					// checks collision with left poles 
					if (sphere2sphere(player.Model->GetX(), player.Model->GetZ(), carCollisionRadius,
						Checkpoints[i].pole1.x, Checkpoints[i].pole1.z, strutRadius))
					{
						momentum.x = -momentum.x / CollisionAbsorption; // negates momentum with absorption
						momentum.z = -momentum.z / CollisionAbsorption; // negates momentum with absorption
						if (player.health > 0.0f) player.health--; // reduces player health
					}
					// checks collision with right poles 
					if (sphere2sphere(player.Model->GetX(), player.Model->GetZ(), carCollisionRadius,
						Checkpoints[i].pole2.x, Checkpoints[i].pole2.z, strutRadius))
					{
						momentum.x = -momentum.x / CollisionAbsorption; // negates momentum with absorption
						momentum.z = -momentum.z / CollisionAbsorption;	// negates momentum with absorption
						if (player.health > 0.0f) player.health--; // reduces player health
					}
				}
				
				// Race stages and checkpoint collisions
				switch (currentRaceState)
				{
				    case check:
					{
						DisplayStage(stageText, stageNumber, myFont); // displays stage number completed

						// checks if player collided with checkpoint
						if (Point2Box(player.Model->GetX(), player.Model->GetZ(), strutWidth, strutDepth,
							Checkpoints[checkpointNumber].Model->GetX(), Checkpoints[checkpointNumber].Model->GetZ()))
						{
							checkpointCollision = true; // bool turns if collision occurs
							RaceStage[checkpointNumber] = true; // race stage for respective checkpoint turns true
							currentRaceState = displayCross; // switches state to display cross 
							stageNumber++; // increments stage number to be displayed
						}
						if (RaceStage[checkpointNumber] == true)
						{
							DisplayStage( stageText , stageNumber, myFont); // displays stage number completed
						}
						break;
					}
					case displayCross:
					{
						if (RaceStage[checkpointNumber] == true)
						{
							DisplayStage(stageText, stageNumber, myFont); // displays stage number completed
							Checkpoints[checkpointNumber].Cross->SetY(crossYShift); // sets cross on appropriate position
							crossCount += frameTime; // starts count up of how long cross is displayed
						}
						if (crossCount >= kmaxCrossCount) // checks if max is reached
						{
							Checkpoints[checkpointNumber].Cross->SetY(crossYPosition); // sets cross below ground
							crossCount = 0.0f; // resets cross count for next cross
							currentRaceState = increase; // switches state to next stage
						}
						break;
					}
					case increase:
					{
						if (RaceStage[checkpointNumber] == true)
						{
							DisplayStage( stageText , stageNumber, myFont); // displays stage number completed
						}
						// checks if current checkpoint number is less that total number of checkpoints
						if (checkpointNumber < Checkpoints.size() - 1)
						{
							checkpointNumber++; // increments checkpoint to checkpoint to check collision with the next one
							currentRaceState = check; // switches state to check to check for collisions with next checkpoint
						}
						else // checkpoint number is greater that total number of checkpoints
						currentRaceState = checkLap; // switches state to check lap collision
					}
					case checkLap:
					{
						// checks collision with dummy points before first checkpoint for lap change
						if (Point2Box(player.Model->GetX(), player.Model->GetZ(), lapCollisionWidth, lapCollisionDepth,
							Laps[lapNumber].DxPos, Laps[lapNumber].DzPos))
						{
							lapCollision = true; // turns true if collisio nhas occured
							LapStage[lapNumber] = true; // lap stage bool turns true for respective lap completion
							currentRaceState = increaseLap; // switches states to increase lap
							circuitNumber++; // incremenst lap number to be displayed
						}
						if (LapStage[lapNumber] == true) // checks if lap is complete
						{
							checkpointNumber = 0; // resets current checkpoint number
							stageNumber = 0; // resets current stage number
							crossCount = 0.0f; // resets cross count for each checkpoint

							// resets all non players movements to restart again each lap
							for (int i = 0; i < maxNonPlayers; i++)
							{
								dummyCount[i] = 0;
								dummyCollision[i] = false;
							}
							nonPlayerMovement = false; // turns true when new lap starts

							// resets all bool values for new circuit
							for (int i = 0; i < Checkpoints.size(); i++)
							{
								RaceStage[i]= false;
							}
						}
						break;
					}
					case increaseLap:
					{
						// checks if current lap is less that total laps
						if (lapNumber < Laps.size() - 1)
						{
							lapNumber++; // increments lap number to a new lap
							currentRaceState = check; // switches state to check to check collisions with checkpoints
							currentGameState = playing;
						}
						else // if current lap is greater that total lap number
						currentGameState = over; // game state switches to over
						break;
					}
				}
				// checks if health is below zero
				if (player.health <= 0.0f) currentGameState = over; // switches game state to over if true
				break;
			} // end of playing state 
			case paused:
			{
				break;
			}
			case over:
			{
				break;
			}

		}// end of game states

		if (myEngine->KeyHit(kPauseKey)) // checks keyboard input
		{
			pause = !pause;
			if (pause)
			{
				currentGameState = paused; // pauses game if true
			}
			else
			{
				currentGameState = playing; // returns to playing if false
			}
		}
		if (myEngine->KeyHit(kEscapeKey)) // reads keybpard input
		{
			myEngine->Stop(); // stop program
		}
	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
bool Point2Box(float sxPos, float szPos, float bwidth, float bdepth,
	           float bxPos, float bzPos)
{
	float minX = bxPos - bwidth;
	float maxX = bxPos + bwidth;
	float minZ = bzPos - bdepth;
	float maxZ = bzPos + bdepth;

	// return true if player position is within the bounding box limits
	return((sxPos > minX && sxPos< maxX) && (szPos > minZ && szPos < maxZ));
}

CollisionAxis Sphere2box(float sOldX, float sOldZ, float sxPos, float szPos, float sRad,
	                      float bxPos, float bzPos, float bwidth, float bdepth)
{
	float minX = bxPos - bwidth/2  - sRad;
	float maxX = bxPos + bwidth/2  + sRad;
	float minZ = bzPos - bdepth/2  - sRad;
	float maxZ = bzPos + bdepth/2  + sRad;

	if (sxPos > minX && sxPos< maxX && szPos > minZ && szPos < maxZ)
	{
		// testing for axis of collision
		if (sOldX < minX || sOldX > maxX)
			return xAxis; // colliding parallel to the x axis
		else
			return zAxis; // colliding parallel to the z axis
	}
	else return none;
}

bool sphere2sphere(float s1xPos, float s1zPos, float s1Rad, float s2xPos, float s2zPos, float s2Rad)
{
	float distX = s2xPos - s1xPos; // calculates distance in the X direction
	float distZ = s2zPos - s1zPos; // calculates distance in the z direction
	float distance = sqrt(distX * distX + distZ * distZ); //calculates distance between two spheres

	// returns true if distance is less than sum of radius
	return (distance < (s1Rad + s2Rad));
}

void DisplayStage( stringstream& text , int number, IFont* font)
{
	text << "Stage: " << number << " complete";
	font->Draw(text.str(), kStageFontX, kStageFontY, kWhite);
	text.str("");
}

void DisplayLap(stringstream& text, int number, IFont* font)
{
	text << "Lap: " << number << "/5";
	font->Draw(text.str(), kLapFontX, kLapFontY, kWhite);
	text.str("");
}

void DisplayTime(stringstream& text, float time , IFont* font)
{
	int seconds, minutes, hours;

	seconds = (int)time;
	minutes = seconds / 60;
	seconds = seconds % 60;
	hours   = minutes / 60;
	minutes = minutes % 60;

	text <<"Time elapsed: "<< hours << ":" << minutes << ":" << seconds;
	font->Draw(text.str(), kTimeFontX, kTimeFontY, kLightGrey);
	text.str("");
}

void DisplayFlare( float gravity, float newV, float time, particle flare[], Hovercars player,
	               float pX, float pY, float pZ)
{
	for (int i = 0; i < quadSize; i++)
	{
		newV = flare[i].moveVector[1] + (gravity * time); // finding new velocity using gravity
		flare[i].moveVector[1] = newV; // setting y component to the value of new velocity

		flare[i].model->MoveY(flare[i].moveVector[1] * time); // move quad in y component
		flare[i].model->MoveX(flare[i].moveVector[0] * time); // move quad in x component 
		flare[i].model->MoveZ(flare[i].moveVector[2] * time); // move quad in z component


		if (flare[i].model->GetY() < 0.0f) // checks if y position is less than zero
		{
			flare[i].moveVector[1] = 0.0f; // resets vector in y component 
			flare[i].model->SetX(pX);
			flare[i].model->SetY(pY);
			flare[i].model->SetZ(pZ);
		} // reset values to player positions
	}

	flare[currentQuad].moveVector[1] = 0.0f;
	flare[currentQuad].moveVector[0] = random(-50, 50);
	flare[currentQuad].moveVector[2] = random(-50, 50);

	currentQuad++;
	if (currentQuad >= quadSize)
	{
		currentQuad = 0;
	}
}
void MoveNonPlayer(Hovercars nonplayers, vector <Dummy>& Dummies,
	int& dummyCount, float carRadius, float time, bool& collision, float speed, float nonPlayerX)
{
	if (collision == false) // checks if player is near dummy
	{
		nonplayers.Model->LookAt(Dummies[dummyCount].Model); // looks at current dummy
		nonplayers.Model->MoveLocalZ(speed * time); // moves towards the dummy
	}
	// checks collision with current dummy
	if (sphere2sphere(nonplayers.Model->GetX(), nonplayers.Model->GetZ(), carRadius,
		Dummies[dummyCount].DxPos, Dummies[dummyCount].DzPos, collisionAwarenessRadius))
	{
		collision = true; // bools changes if collision has occured
	}
	if (collision == true)
	{
		if (dummyCount < Dummies.size() - 1)
		{
			dummyCount++; // increments current dummy to next dummy position
			collision = false; // resets bool for next collision
		}
		else
		{ 
			// sets non- player to intial position once at the end of stage
			nonplayers.Model->SetX(nonPlayerX);
		}
	}
}

void CreateScene(IMesh* isle, vector <Isle>& Isles,
	IMesh* wall, vector<Wall>& Walls,
	IMesh* checkpoint, vector<Checkpoint>& Checkpoints,
	IMesh* walkway,
	IMesh* smalltank2, vector <Smalltank>& Cornertanks,
	IMesh* dummy, vector <Dummy>& Dummies,
	IMesh* cross, vector <Dummy>& Laps,
	IMesh* smalltank1, vector <Smalltank>& SunkenTanks, IMesh* garageMesh)
{
	ifstream infile;
	infile.open("scene.txt"); // opens text file
	string name, x, z, rotation; // declares strings for each object
	int count = 0;
	if (!infile) // checks if file is present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else
	{
		while (!infile.eof())
		{
			infile >> name; // reads in object name
			infile >> x;    // reads x position of object
			infile >> z;    // reads z position of object
			infile >> rotation; // reads angle of rotationof object
			
			// converts strings to floats
			float X = stof(x); 
			float Z = stof(z);
			float R = stof(rotation);

			if (name == "Isle") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Isle temp;
				temp.Model = isle->CreateModel(X, 0, Z);
				temp.Model->RotateY(R);
				temp.IxPos = X;
				temp.IzPos = Z;
				Isles.push_back(temp);
			}
			else if (name == "Wall") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Wall temp;
				temp.Model = wall->CreateModel(X, 0, Z);
				temp.Model->RotateY(R);
				temp.WxPos = X;
				temp.WzPos = Z;
				Walls.push_back(temp);
			}
			else if (name == "Checkpoint") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Checkpoint temp;
				temp.Model = checkpoint->CreateModel(X, 0, Z);
				temp.Model->RotateLocalY(R);
				temp.Cross = cross->CreateModel(X, crossYPosition, Z);
				temp.Cross->Scale(crossScaleFactor);
				temp.Cross->RotateLocalY(R);
				temp.Model->SetSkin(kbackdropSkin);
				temp.CxPos = X;
				temp.CzPos = Z;

				float xOffset = abs(R) == 0.0f;
				float zOffset = abs(R) == 90.0f;

				temp.pole1.x = X - 10.0f * xOffset;
				temp.pole1.z = Z - 10.0f * zOffset;

				temp.pole2.x = X + 10.0f * xOffset;
				temp.pole2.z = Z + 10.0f * zOffset;

				Checkpoints.push_back(temp);
			}
			else if (name == "Walkway") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				IModel* path = walkway->CreateModel(X, 0, Z);
				path->Scale(walkwayScale);
				path->RotateY(R);
			}
			else if (name == "Cornertank") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Smalltank temp;
				temp.Model = smalltank2->CreateModel(X, 0, Z);
				Cornertanks.push_back(temp);
			}
			else if (name == "Sunkentank")
			{
				// creates temporary model and stores it in model vector
				Smalltank temp;
				temp.Model = smalltank1->CreateModel(X, sunkenTankYPos, Z);
				temp.Model->RotateZ(R);
				SunkenTanks.push_back(temp);
			}
			else if (name == "Dummy") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Dummy temp;
				temp.Model = dummy->CreateModel(X, 0, Z);
				temp.DxPos = X;
				temp.DzPos = Z;
				Dummies.push_back(temp);
			}
			else if (name == "Lap") // checks if string is present in file
			{
				// creates temporary model and stores it in model vector
				Dummy temp;
				temp.Model = dummy->CreateModel(X, 0, Z);
				temp.DxPos = X;
				temp.DzPos = Z;
				Laps.push_back(temp);
			}
			else if (name == "Garage") // checks if string is present in file
			{
				IModel* garage = garageMesh->CreateModel(X, 0, Z);
				garage->Scale(spaceObjectScale);
			}
			//count++;
		}

	}
}