/*-------------------------------------------------------------------

ev-counter.cpp

Copyright (C) Elizabeth Bergshoeff 2020

*///-----------------------------------------------------------------

/*
read:
	arguments:
		pathin, pathout

		strings that point to files. First string is the path to the ev list file,
		the second string is the path to the output file.
*/
/*
	ev list file format:
		filein:
			[pokemon decl] [ev list]
		pokemon decl:
			[' ' id ' ' name]^n '\n'
		ev decl:
			[ev list]^n
		ev list:
			[id]^n [ev]^n
		ev:
			'a'
			'd'
			'p'
			'q'
			's'
			'h'
		id:
			any character other than ev, ' ', '\n', '!'
		name:
			string of characters other than ' ' and '\n'
		
		Notes:
			the first space in the pokemon decl is needed
			reserved characters include ' ', '\n', ev, '!'
*/

#ifndef WINDOWS
#define bluet_types
typedef unsigned char uchar;
#endif


#define pokeid_default '!'
#define idlist_end '\n'

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>


using namespace std;


struct Pokemon
{
	string name;
	char id = pokeid_default;
	uchar eva = 0;
	uchar evd = 0;
	uchar evp = 0;
	uchar evq = 0;
	uchar evs = 0;
	uchar evh = 0;

	Pokemon(char c, string s) : id(c), name(s) {}
};

struct Error_fin_bad {};
struct Error_empty_evlist {};
struct Error_repeated_id { char id = pokeid_default; Error_repeated_id(char c) : id(c) {} };
struct Error_filein { string path; Error_filein(const char* s) : path(s) {} };

class EV_Counter_App
{
public:
	EV_Counter_App() { read(); }
	void read();

private:
	//called by read. can throw fstream errors
	void pokemon_decl();
	void ev_decl();
	void write_out();
	
private:
	//used by pokemon_decl to check for repeated ids
	const inline bool is_id(char c) 
	{ 
		//find returns end if the given id is not found in idlist. 
		//if find returns end, then c is not an id. otherwise, c is an id
		if ( find(idlist.begin() , idlist.end() , c) == idlist.end() ) return false; 
		return true; 
	}

private:
	const char* filein = "evlist.txt";
	ifstream fin;

	const char* fileout = "evsheet.txt";
	ofstream fout;

	/*
	'idlist' and 'evlist' are related and share special properties
	specifically
	1. evlist and idlist should always be the same size
	2. items in evlist and idlist which share an index are related in some way. 
		In this case, they have the same id and this property is used to find ids in evlist
		in the routines void pokemon_decl() and void ev_decl() in class EV_Counter_App
		
		if somehow they become different sizes debugging becomes difficult:
		use of one or the other may seem reasonable in its context but its complement may not be needed
		and may be 'brought out of order' due to edits to the one (see 1). This can cause problems in other
		parts of the application which may be hard to track down; the issue may not be clear in context
		due to routines like "is_id" which implicitly references only 1 of the below

		TL DR; These two should always be the same size and as such should be appended together;
			respectively they should be encapsulated together in a single object to make the code more readable.
			see the routine void pokemon_decl() and void read() in class EV_Counter_App
	*/
	vector<unique_ptr<Pokemon>> evlist;
	vector<char> idlist;
};


void EV_Counter_App::read()
{
	//clear storage
	idlist.resize(0);
	evlist.resize(0);

	fin.open(filein);
	if (fin)
	{
		pokemon_decl();
		write_out();
	}
	else
		throw Error_filein(filein);
}

//see the 'ev list file format'
void EV_Counter_App::pokemon_decl()
{
	char c;
	string s;
	do
	{
		//fetch ID
		fin.get(c);

		//check fin state
		if (fin.bad())
			throw Error_fin_bad();
		if (fin.fail())
			throw Error_empty_evlist();

		//check for idlist end character
		if (c == idlist_end)
			return;

		//since the character fetched is not idlist_end or EOF, it is an id
		//check for a redundant id
		if (is_id(c))
			throw Error_repeated_id(c);

		//id is a new id, fetch the pokemon name
		fin >> s;
		//check fin
		if (fin.bad())
			throw Error_fin_bad();
		if (fin.fail())
			throw Error_empty_evlist();

		unique_ptr<Pokemon> temp = make_unique<Pokemon>(c, s);

		//store pokemon and id
		evlist.push_back(move(temp));
		idlist.push_back(c);

	} while (1);
}

void EV_Counter_App::ev_decl()
{
	vector<int> temp(6);
	char c;
	vector<uchar> active_ids;
	while (1)
	{
		//read a character
		fin.get(c);
		if (fin.bad())
			throw Error_fin_bad();
		if (fin.fail())
			return;

		switch (c)
		{
		case 'h':
			break;
		case 'a':
			break;
		case 'd':
			break;
		case 'p':
			break;
		case 'q':
			break;
		case 's':
			break;
		default:
			auto idit = find(idlist.begin(), idlist.end(), c);
			if (idit == idlist.end())
				throw Error_evlist_bad_id(c);
			//add the tallied evs to the respective pokemon
			
			//clear the active ids
			//find the new id

		}

	}

}

void EV_Counter_App::write_out()
{
	//for debugging
	for (int i = 0; i < evlist.size(); ++i)
	{
		cout << evlist[i]->id << ':' << ' ' << evlist[i]->name << '\n';
	}
}

int main()
{
	try
	{
		EV_Counter_App();
	}
	catch (Error_fin_bad& e)
	{
		cerr << "Fin was bad" << endl;
	}
	catch (Error_empty_evlist& e)
	{
		cerr << "reached eof before evlist" << endl;
	}
	catch (Error_repeated_id& e)
	{
		cerr << "id was repeated" << endl;
	}
	catch (Error_filein& e)
	{
		cerr << "could not open " << e.path << endl;
	}
}
