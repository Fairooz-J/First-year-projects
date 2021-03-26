// Fairooz Zahin Jalal

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <fstream>
using namespace std;

const int TEXT_SIZE             = 14; // stores the size of the text file array
const int SEARCH_SIZE           = 19; // stores the size of the search file array
const int TEXT_STRING_SIZE      = 12; // stores the size of the texts string array
const int SEARCH_STRING_SIZE    = 5;  // stores the size of the SEARCH string array
const int TEXT_ROWS             = 4;  // stores number of rows in 2d text array
const int TEXT_COLUMNS          = 14; // stores number of columns in 2D text array
const int SEARCH_SIZE_2D        = 63; // stores size of 2D search array
const int SEARCH_STRING_SIZE_2D = 16; // stores size of 2D search string array

void ReadTextFile(char array[], int size);
// checks if text1.txt is present or not and then reads each character from the file into a character array

void ReadSearchFile(char arr[], int size);
// checks if search1.txt is present or not and then reads each character from the file into a character array

void CombineTextCharacters(char textArr[], string arr[] , int size);
// combines three characters at a time from the character array and reads them into a string array

void CombineSearchCharacters(char searchArr[], string arr[], int size);
// combines three characters at a time from the character array and reads them into a string array

void SearchWords( string searchArr[], string textArr[], int searchSize, int textSize);
// compares each string in search against text and prints out words that match along with position, removing them
// and printing words that don't match in another for loop

void Read2DTextFile( int row, int column, char txtArr[TEXT_ROWS][TEXT_COLUMNS]);
// checks if text2D.txt is present or not and then reads each character from the file into a 2D character array

void Read2DSearchFile(char Arr[], int size);
// checks if search2D.txt is present or not and then reads each character from the file into a character array

void Combine2DSearchCharacters(char searchArr[], string arr[], int size);
// combines three characters at a time from the character array and reads them into a string array

void ForwardSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS] , string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// checks all 3 consecutive characters from left to right and matches against search array and prints out 
// words that match along with line number and location

void BackwardSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// checks all 3 consecutive characters from right to left and matches against search array and prints out 
// words that match along with line number and location

void VerticalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// checks all 3 consecutive characters from up to do and down to up and matches against search array and prints out 
// words that match along with line number and location

void DiagonalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// checks all 3 consecutive characters diagonal down right and diagonal down left and matches against search array and prints out 
// words that match along with line number and location

void OrthogonalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// checks all 3 consecutive characters down right, down left, right down and left down and matches against search array and prints out 
// words that match along with line number and location

void Write2DFilteredText(char filteredtxt[TEXT_ROWS][TEXT_COLUMNS]);
// uses ofstream to write final filtered text into text2Dfiltered.txt

int main()
{
	char textFileArr[TEXT_SIZE];
	ReadTextFile(textFileArr, TEXT_SIZE);
	// reads text file into an array using ifstream 

	char searchFileArr[SEARCH_SIZE]; 
	ReadSearchFile(searchFileArr, SEARCH_SIZE);
	// reads search file into an array usinh ifstream
	
	string textStringArr[TEXT_STRING_SIZE];
	CombineTextCharacters(textFileArr, textStringArr, TEXT_SIZE);
	// combines three characters from character array and stores in string array for comparison

	string searchStringArr[SEARCH_STRING_SIZE];
	CombineSearchCharacters(searchFileArr, searchStringArr, SEARCH_SIZE);
	// combines three characters from character array and stores in string array for comparison

	char text2DFileArr[TEXT_ROWS][TEXT_COLUMNS];
	Read2DTextFile(TEXT_ROWS, TEXT_COLUMNS, text2DFileArr);
	// reads 2D text file into an array using ifstream 

	char search2DFileArr[SEARCH_SIZE_2D];
	Read2DSearchFile( search2DFileArr ,SEARCH_SIZE_2D );
	// reads 2D search file into an array usinh ifstream

	string searchStringArr2D[SEARCH_STRING_SIZE_2D];
	Combine2DSearchCharacters(search2DFileArr, searchStringArr2D, SEARCH_SIZE_2D);
	// combines three characters from character array and stores in string array for comparison

	char filteredText[TEXT_ROWS][TEXT_COLUMNS];
	Read2DTextFile(TEXT_ROWS, TEXT_COLUMNS, filteredText);
	// creates an array of 2D text where all filtered words from 2d text is stored and written into file

	int userInput; // stores and checks number pressed by user
	char response = 'y';

	cout << "                   Crossword Puzzle         " << endl << endl;
	cout << "Choose from the following numbers to view words found, 6 to view part 1 and 7 to view full Part 2" << endl << endl;
	cout        << "1) Forward Search" 
		<< endl << "2) Backward Search" 
		<< endl << "3) Vertical Search"
		<< endl << "4) Diagonal Search" 
		<< endl << "5) Orthogonal Search"
		<< endl << "6) View words found in Part 1" 
		<< endl << "7) View full Part 2 and write into filtered text file" <<  endl << endl;

	do // prompts user to choose a number while response is to view respected search algorithms
	{
		cout << "Please choose a number: ";
		cin >> userInput;
		cout << endl;

		if (userInput == 1) // checks user input and calls forward search algorithm function
		{
			ForwardSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
		}
		else if (userInput == 2) // checks user input and calls backward search algorithm function
		{
			BackwardSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
		}
		else if (userInput == 3) // checks user input and calls vertical search algorithm function
		{
			VerticalSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
		}
		else if (userInput == 4) // checks user input and calls diagonal search algorithm function
		{
			DiagonalSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
		}
		else if (userInput == 5) // checks user input and calls orthogonal search algorithm function
		{
			OrthogonalSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
		}
		else if (userInput == 6) // checks user input and calls search word algorithm function for part 1
		{
			for (int i = 0; i < TEXT_SIZE; i++)
			{
				cout << textFileArr[i];
			}
			cout << endl << endl;
			SearchWords(searchStringArr, textStringArr, SEARCH_STRING_SIZE, TEXT_STRING_SIZE);
			cout << endl;
		}
		else if (userInput == 7) // checks user input and displays 2d text, all algorithm functions and filtered text
		{
			cout << "Unfiltered text: " << endl << endl;
			for (int i = 0; i < TEXT_ROWS; i++)
			{
				for (int j = 0; j < TEXT_COLUMNS; j++)
				{
					cout << text2DFileArr[i][j];
				}
				cout << endl;
			}
			cout << endl << endl;
			ForwardSearch(text2DFileArr, searchStringArr2D, filteredText);
			BackwardSearch(text2DFileArr, searchStringArr2D, filteredText);
			VerticalSearch(text2DFileArr, searchStringArr2D, filteredText);
			DiagonalSearch(text2DFileArr, searchStringArr2D, filteredText);
			OrthogonalSearch(text2DFileArr, searchStringArr2D, filteredText);
			cout << endl;
			cout << "Filtered text: " << endl << endl;
			for (int i = 0; i < TEXT_ROWS; i++)
			{
				for (int j = 0; j < TEXT_COLUMNS; j++)
				{
					cout << filteredText[i][j];
				}
				cout << endl;
			}
			Write2DFilteredText(filteredText);
			cout << endl << endl;
			cout << "Do you want to start again (y / n)?" << endl; // prompts user to start crossword again
			cin >> response;
		}

	} while (response == 'y'); // exits do - while loop if user types 'n'.

	system("pause");
}

void ReadTextFile(char array[], int size)
{
	ifstream infile;
	infile.open("text1.txt");
	int tCount = 0;
	char ch;
	if (!infile) // checks if file is present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else // reads each character into an array using counter until end of file
	{
		while (tCount < size && !infile.eof())
		{
			infile >> noskipws;
			infile >> ch;
			if (!infile.eof())
			{
				array[tCount] = ch;
				tCount++;
			}
		}
	}
	infile.close();
}

void ReadSearchFile(char arr[], int size)
{
	ifstream infile;
	infile.open("search1.txt");
	int sCount = 0;
	char ch;
	if (!infile) // checks if file is present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else // reads each character into an array using counter until end of file
	{
		while (sCount < size && !infile.eof())
		{
			infile >> noskipws;
			infile >> ch;
			if (!infile.eof())
			{
				arr[sCount] = ch;
				sCount++;
			}
		}
	}
	infile.close();
}

void CombineTextCharacters(char textArr[], string arr[] , int size) // combines 3 text characters into a string array
{
	int count = 0;
	for (int i = 0; i < size - 2; i++)
	{
		string words = string() + textArr[i] + textArr[i + 1] + textArr[i + 2];
		arr[count] = words;
		count++;
	}
}

void CombineSearchCharacters(char searchArr[], string arr[], int size) // combines 3 search characters into a string array
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		string words = string() + searchArr[i] + searchArr[i + 1] + searchArr[i + 2];
		arr[count] = words;
		count++;
		i += 3;
	}
}

void SearchWords(string searchArr[], string textArr[], int searchSize, int textSize) 
{
	ofstream outfile;
	outfile.open("text1Filtered.txt");
	if (!outfile) // checks if file is present. Shows error if not present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else // compares text string and search string and writes into file
	{
		for (int i = 0; i < textSize; i++)
		{
			for (int j = 0; j < searchSize; j++)
			{
				if (searchArr[j] == textArr[i])
				{
					cout << textArr[i] << ",Found,location " << i << endl;
					searchArr[j].erase();
					outfile << textArr[i] << endl;
				}
			}
		}
	}
	for (int i = 0; i < searchSize; i++)// outputs words not found in the text file
	{
		if (searchArr[i].empty() == false)
		{
			cout << searchArr[i] << ",Not found" << endl;
		}
	}
	cout << endl;
}

void Read2DTextFile(int row, int column, char txtArr[TEXT_ROWS][TEXT_COLUMNS] )
{
	ifstream infile;
	infile.open("text2D.txt");
	if (!infile) // checks if file is present. Shows error if not present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else // reads each character in rows and columns into character array until end of file
	{
		while (!infile.eof())
		{
			if (!infile.eof())
			{
				for (int i = 0; i < row; i++)
				{
					for (int j = 0; j < column; j++)
					{
						infile >> txtArr[i][j];
					}
				}
			}
		}
	}
	infile.close();
}

void Read2DSearchFile(char Arr[], int size)
{
	ifstream infile;
	infile.open("search2D.txt");
	int sCount = 0;
	char ch;
	if (!infile) // checks if file is present. Shows error if not present
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else // reads each character into an array using counter until end of file
	{
		while (sCount < size && !infile.eof())
		{
			infile >> noskipws;
			infile >> ch;
			if (!infile.eof())
			{
				Arr[sCount] = ch;
				sCount++;
			}
		}
	}
	infile.close();
}

void Combine2DSearchCharacters(char searchArr[], string arr[], int size)
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
		string words = string() + searchArr[i] + searchArr[i + 1] + searchArr[i + 2];
		arr[count] = words;
		count++;
		i += 3;
	}
}

void ForwardSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	for (int i = 0; i < TEXT_ROWS; i++)
	{
		for (int j = 0; j < TEXT_COLUMNS - 2; j++)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i][j + 1] + textArr2D[i][j + 2];

			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i][j + 1] = '.';
					filteredtxt[i][j + 2] = '.';
				}
			}
		}
	}
	
}
void BackwardSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	for (int i = TEXT_ROWS - 1; i >= 0; i--)
	{
		for (int j = TEXT_COLUMNS - 1; j > 0; j--)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i][j - 1] + textArr2D[i][j - 2];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i][j - 1] = '.';
					filteredtxt[i][j - 2] = '.';
				}
			}

		}
	}
}
void VerticalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	for (int j = 0; j < TEXT_COLUMNS - 1; j++)
	{
		for (int i = 0; i < TEXT_ROWS - 2; i++)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i + 1][j] + textArr2D[i + 2][j];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i + 1][j] = '.';
					filteredtxt[i + 2][j] = '.';
				}
			}
		}
	}

	for (int j = TEXT_COLUMNS - 1; j >= 0; j--)
	{
		for (int i = TEXT_ROWS - 1; i > 0; i--)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i - 1][j] + textArr2D[i - 2][j];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i - 1][j] = '.';
					filteredtxt[i - 2][j] = '.';
				}
			}
		}
	}
}
void DiagonalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	// Down right
	for (int i = 0; i < TEXT_ROWS - 2; i++)
	{
		for (int j = 0; j < TEXT_COLUMNS - 2; j++)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i + 1][j + 1] + textArr2D[i + 2][j + 2];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i + 1][j + 1] = '.';
					filteredtxt[i + 2][j + 2] = '.';
				}
			}
		}
	}
	// Down left
	for (int i = 0; i < TEXT_ROWS - 2; i++)
	{
		for (int j = TEXT_COLUMNS; j > 0; j--)
		{
			string word = string() + textArr2D[i][j] + textArr2D[i + 1][j - 1] + textArr2D[i + 2][j - 2];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (word == searchArr2D[s])
				{
					cout << word << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i + 1][j - 1] = '.';
					filteredtxt[i + 2][j - 2] = '.';
				}
			}
		}
	}
}

void OrthogonalSearch(char textArr2D[TEXT_ROWS][TEXT_COLUMNS], string searchArr2D[], char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	for (int i = 0; i < TEXT_ROWS - 1; i++)
	{
		for (int j = 0; j < TEXT_COLUMNS - 1; j++)
		{
			string rightDown = string() + textArr2D[i][j] + textArr2D[i][j + 1] + textArr2D[i + 1][j + 1];
			string downRight = string() + textArr2D[i][j] + textArr2D[i + 1][j] + textArr2D[i + 1][j + 1];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (rightDown == searchArr2D[s])
				{
					cout << rightDown << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i][j + 1] = '.';
					filteredtxt[i + 1][j + 1] = '.';
				}
				else if (downRight == searchArr2D[s])
				{
					cout << downRight << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i + 1][j] = '.';
					filteredtxt[i + 1][j + 1] = '.';
				}
			}
		}
	}
	for (int i = 0; i < TEXT_ROWS - 1; i++)
	{
		for (int j = TEXT_COLUMNS - 1; j > 0; j--)
		{
			string leftDown = string() + textArr2D[i][j] + textArr2D[i][j - 1] + textArr2D[i + 1][j - 1];
			string downLeft = string() + textArr2D[i][j] + textArr2D[i + 1][j] + textArr2D[i + 1][j - 1];
			for (int s = 0; s < SEARCH_STRING_SIZE_2D; s++)
			{
				if (leftDown == searchArr2D[s])
				{
					cout << leftDown << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i][j - 1] = '.';
					filteredtxt[i + 1][j - 1] = '.';
				}
				else if (downLeft == searchArr2D[s])
				{
					cout << downLeft << " Found,line " << i << " location " << j << endl;
					filteredtxt[i][j] = '.';
					filteredtxt[i + 1][j] = '.';
					filteredtxt[i + 1][j - 1] = '.';
				}
			}
		}
	}
}
void Write2DFilteredText(char filteredtxt[TEXT_ROWS][TEXT_COLUMNS])
{
	ofstream outfile;
	outfile.open("text2DFiltered.txt");
	if (!outfile)
	{
		cout << "ERROR: ";
		cout << "Can't open input file\n";
	}
	else
	{
		for (int i = 0; i < TEXT_ROWS; i++)
		{
			for (int j = 0; j < TEXT_COLUMNS; j++)
			{
				outfile << filteredtxt[i][j];
			}
		}
	}
}