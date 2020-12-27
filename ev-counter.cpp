/*----------------------------------------------------------------------

ev-counter.cpp

Copyright (C) Elizabeth Bergshoeff 2020

*///--------------------------------------------------------------------
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

#define idlist_end '\n'

#include <iostream>
#include <fstream>
#include <exception>
#include <iomanip>
#include "Effort_Values.h"


using namespace std;

namespace
{
	void increment_ev(unsigned char& ev)
	{
		if (ev < 255)
			++ev;
	}
};

struct Error_rdev_ev_badbit : exception
{
	virtual const char* what() const throw()
	{ return "stream was bad in rdev_ev"; }
};
struct Error_rdev_pokedecl_notgood : exception
{
	virtual const char* what() const throw()
	{ return "badbit or failbit set before read_evlist finished reading pokemon declarations"; }
};
struct Error_read_evlist_pathin : exception
{
	Error_read_evlist_pathin(const char* path) : path(path) {}
	virtual const char* what() const throw()
	{ return "Read_evlist was unable to open the evlist file"; }
	string path;
};

class Evlist_Reader
{
public:
	Evlist_Reader() { }
	void read_evlist(const char* evlist_path);
	ostream& write_pokefile(ostream& os);

private:
	// ~~~ read_evlist (rdev) input loops ~~~

	// pokemon declaration, reads pokemon names and ids
	void rdev_pokedecl();
	// reads ev data
	void rdev_ev();

private:
	ifstream fin;
	unique_ptr<Pokemon> pokemon_buffer = make_unique<Pokemon>();
};


void Evlist_Reader::read_evlist(const char* evlist_path)
{
	fin.open(evlist_path);
	if (fin)
	{
		rdev_pokedecl();
		rdev_ev();
	}
	else
		throw Error_read_evlist_pathin(evlist_path);
}

// see the 'ev list file format'
void Evlist_Reader::rdev_pokedecl()
{
	char id;
	string name;
	do
	{
		// fetch ID
		if (fin.get(id))
		{
			switch (id)
			{
			case ' ':
				continue;
			case idlist_end:
				return;
			default:
				// retrieve pokemon name
				// If fin is notgood, will throw exception (see below)
				if (fin >> name)
				{
					//store pokemon and id
					pokemon_buffer->push_back(name, id);
					continue;
				}
			}
		}
		// fin was not good after fetching id or name
		throw Error_rdev_pokedecl_notgood();
	} while (true);
}

void Evlist_Reader::rdev_ev()
{
	// ~~~ local variable declarations ~~~

	// character from fin
	char c;

	Effort_Values temp_count;
	clear_effort_values(temp_count);

	// index to dereference count.
	size_t ev_index;

	bool reading_ids = false;

	// the pokemon which have been referenced by id before a list of ev data
	// see evlist file specification at top of page
	vector<int> current_ids;


	//input loop
	do
	{
		// read a character. If the character is a stat, set index for temp_count
		// otherwise, offload count then clear current_poke if applicable; then update current_ids
		if (fin.get(c))
		{
			switch (c)
			{
			case 'h':
				ev_index = (size_t)poke_base_stat::hp;
				break;
			case 'a':
				ev_index = (size_t)poke_base_stat::attack;
				break;
			case 'd':
				ev_index = (size_t)poke_base_stat::defense;
				break;
			case 'p':
				ev_index = (size_t)poke_base_stat::spattack;
				break;
			case 'q':
				ev_index = (size_t)poke_base_stat::spdefense;
				break;
			case 's':
				ev_index = (size_t)poke_base_stat::speed;
				break;
			default:
				if (!reading_ids)
				{
					reading_ids = true;
					// add the tallied evs to the respective pokemon
					for (auto a : current_ids)
					{
						pokemon_buffer->add(a, temp_count);
					}
					// clear current_ids and temp_count
					current_ids.resize(0);
					clear_effort_values(temp_count);
				}
				// append the new id to current_ids
				current_ids.push_back(c);
				continue;
			}
			reading_ids = false;
			increment_ev(temp_count[ev_index]);
		}
		else
		{
			// ~~~ clean up and return ~~~
			// add the tallied evs to the respective pokemon
			for (auto a : current_ids)
			{
				pokemon_buffer->add(a, temp_count);
			}
			return;
		}
	} while (true);
}

//iterates through pokemon_buffer and prints out each pokemon's info
ostream& Evlist_Reader::write_pokefile(ostream& os)
{
	Effort_Values temp;
	string name;

	for (size_t i = 0; i < pokemon_buffer->get_num_pokemon(); ++i)
	{
		name = pokemon_buffer->get_name(i);
		pokemon_buffer->get_ev(i, temp);
		os << name << endl;
		for (size_t j = 0; j < (size_t)poke_base_stat::size; ++j)
		{
			os << '\t' << setw(12) << poke_base_stat_names[j] << ": " << (unsigned int) temp[j] << endl;
		}
		os << endl;
	}
	return os;
}

int main()
try
{
	const char* filein = "evlist.txt";
	const char* fileout = "evsheet.txt";

	Evlist_Reader evr;
	evr.read_evlist(filein);

	ofstream fout(fileout);
	if (fout)
		evr.write_pokefile(fout);
}
catch (exception& e)
{
	cout << e.what();
}