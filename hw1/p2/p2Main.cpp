/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <string>
#include <cstring>
#include "p2Json.h"

using namespace std;

string split_cmd(string);

/*class Commands
{
	PRINT,
	ADD,
	SUM,
	AVE,
	MAX,
	MIN

};*/

int main()
{
	Json json;

	// Read in the csv file. Do NOT change this part of code.
	string jsonFile;
	cout << "Please enter the file name: ";
	cin >> jsonFile;
	if (json.read(jsonFile))
		cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
	else
	{
		cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
		exit(-1); // jsonFile does not exist.
	}

	if (json.isEmpty())
	{
		cout << "Error: No element found!!" << endl;
		exit(-1);
	}

	// TODO read and execute commands
	// cout << "Enter command: ";

	getchar(); //!!! drop the previous '\n' char !!!(Very important)
	while (true)
	{
		cout << "Enter command: ";
		string input;
		getline(cin, input);

		//**********DEBUG**********
		//cout << "Input:" << input << endl;
		//*************************

		string cmd = split_cmd(input); //Retract commands from user input

		if (cmd == "PRINT")
		{
			json.print();
		}
		else if (cmd == "ADD")
		{
			json.add(input);
		}
		else if (cmd == "SUM")
		{
			cout << "The summation of the values is: " << json.get_sum() << "." << endl;
		}
		else if (cmd == "AVE")
		{
			cout << "The average of the values is: " << fixed << setprecision(1) << json.get_ave() << "." << endl;
		}
		else if (cmd == "MAX")
		{
			//cout << json.get_max() << endl;
			cout << "The maximum element is: { " << json.get_max_element() << " }." << endl;
		}
		else if (cmd == "MIN")
		{
			//cout << json.get_min() << endl;
			cout << "The minimum element is: { " << json.get_min_element() << " }." << endl;
		}
		/*else if (cmd == "DEL")
		{

		}
		else if (cmd == "WRITE")
		{

		}*/
		else if (cmd == "EXIT")
		{
			break;
		}
		else
		{
			cout << "Error: Wrong command!" << endl;
		}
		/*switch (cmd)
		{
		case PRINT:
			json.print();
			break;
		case ADD:

			break;
		case SUM:
			cout << json.get_sum() << endl;
			break;
		case AVE:
			cout << json.get_ave() << endl;
			break;
		case MAX:
			cout << json.get_max() << endl;
			break;
		case MIN:
			cout << json.get_min() << endl;
			break;
		default:
			cout << "Error command!" << endl;
			break;
		}*/
	}
}

string split_cmd(string input)
{
	string cmd;
	char cmd_s[100];
	strcpy(cmd_s, input.c_str());
	cmd = strtok(cmd_s, " ");
	return cmd;
}
