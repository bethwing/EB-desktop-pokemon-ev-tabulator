/*-------------------------------------------------------------------------------------
 Effort_Values.h
 
 Copyright (C) Elizabeth Bergshoeff 2020

 This header defines class Pokemon, a container for ev info storage.
 Each pokemon has a name (string), id (char), and set of ev
 Each pokemon id must be unique. 
 Attempting to find a pokemon using 
 an id which does not exist throws Error_pokemon_bad_id
 Attempting to create a new pokemon with 
 an id which is already in use throws Error_pokemon_repeated_id


*///-----------------------------------------------------------------------------------


#pragma once
#include <vector>
#include <array>
#include <string>
#include <cassert>
#include <algorithm>
#include <sal.h>

using namespace std;

namespace
{
	// ceil the sum of two positive integers. Beware int overflow of sum
	inline unsigned int ceil_pos_sum(unsigned int a, unsigned int b, unsigned int ceil)
	{
		unsigned int z = a + b;
		if (z > ceil)
			return ceil;
		return z;
	}
};

struct Error_Pokemon_bad_id : exception
{
	Error_Pokemon_bad_id() = delete;
	Error_Pokemon_bad_id(int c) : id(c) {}

	int id;

	const char* what()
	{
		return "an instance of pokemon was unable to find a given id";
	}
};
struct Error_Pokemon_repeated_id
{
	Error_Pokemon_repeated_id() = delete;
	Error_Pokemon_repeated_id(int c) : id(c) {}

	int id;

	const char* what()
	{
		return "tried to create a new pokemon using an id that already exists";
	}
};
struct Error_Pokemon_push_back_memory
{
	const char* what() { return "pokemon::push_back was called when the number of pokemon in it equaled the name buffer size"; }
};

enum class poke_base_stat : size_t
{
	hp = 0,
	attack, defense,
	spattack, spdefense,
	speed,
	size
};

const array<const char*, (size_t)poke_base_stat::size> poke_base_stat_names =
{
	"hit points",
	"attack",
	"defense",
	"spattack",
	"spdefense",
	"speed"
};

typedef array<unsigned char, (size_t)poke_base_stat::size> Effort_Values;

inline void clear_effort_values(Effort_Values& ev)
{
	for (size_t i = 0; i < (size_t)poke_base_stat::size; ++i)
		ev[i] = 0;
}

struct Single_Pokemon
{
	string name;
	int id;
	Effort_Values ev;
};

//container for ev data, pokemon names
//each pokemon has an id
class Pokemon
{
	// ~~~ Constructors ~~~

public:
	Pokemon(size_t size)
	{
		name_buffer.resize(size);

		const size_t ev_buffer_size = (size_t)poke_base_stat::size * size;
		ev_buffer = vector<unsigned char>(ev_buffer_size, 0);
	}

	Pokemon()
	{
		const size_t buffer_size = 256;
		name_buffer.resize(buffer_size);

		const size_t ev_buffer_size = (size_t) poke_base_stat::size * buffer_size;
		ev_buffer = vector<unsigned char>(ev_buffer_size, 0);
	}

	//creates a new pokemon
	//can throw Error_pokemon_repeated_id
	void push_back(string new_name, int new_id);

	// used to verify that a pokemon id is not already in Pokemon::id in push_back
	bool is_id(int new_id);

	//appends ev data to a given pokemon
	void add(size_t index, Effort_Values& ev _In_)
	{
		assert(index < get_num_pokemon());
		for (size_t i = 0; i < (size_t)poke_base_stat::size; ++i)
		{
			const int j = i * get_buffer_size() + index;
			const unsigned int temp = ev_buffer[j];
			ev_buffer[j] = ceil_pos_sum(temp, ev[i], max_ev);
		}
	}
	inline void add(int id, Effort_Values& ev _In_) 
	{ 
		add(get_index(id), ev); 
	}

	// ~~~ Get functions ~~~

	inline size_t get_buffer_size() 
	{ 
		return name_buffer.size(); 
	}
	inline size_t get_num_pokemon()
	{
		return id.size();
	}

	// retrieves an index given an id.
	// throws Error_pokemon_get_index if the id is not found
	size_t get_index(int uk_id);

	void get_ev(size_t index, Effort_Values& out _Out_);
	inline void get_ev(int id, Effort_Values& out _Out_) 
	{ 
		get_ev(get_index(id), out); 
	}

	inline string get_name(size_t index) 
	{ 
		assert(index < get_num_pokemon()); 
		return name_buffer[index]; 
	}
	inline string get_name(int id) 
	{ 
		return get_name(get_index(id)); 
	}

	// ~~~ constants ~~~

	const unsigned int max_ev = 255;

private:
	vector<string> name_buffer;
	vector<unsigned char> ev_buffer;
	vector<int> id;
};


//creates a new pokemon
void Pokemon::push_back(string new_name, int new_id)
{
	if (is_id(new_id))
		throw Error_Pokemon_repeated_id(new_id);

	//numpokemon_n-1 = index_newpokemon
	//'pokemon' can be plural or singular
	if (get_num_pokemon() < get_buffer_size())
	{
		name_buffer[get_num_pokemon()] = new_name;
		id.push_back(new_id);
	}
	else
		throw Error_Pokemon_push_back();
}

void Pokemon::get_ev(size_t index, Effort_Values& out _Out_)
{
	assert(index < id.size());
	for (size_t i = 0; i < (size_t)poke_base_stat::size; ++i)
	{
		out[i] = ev_buffer[i * get_buffer_size() + index];
	}
}

//throws Error_pokemon_get_index if the id is not found
size_t Pokemon::get_index(int uk_id)
{
	std::vector<int>::iterator it = find(id.begin(), id.end(), uk_id);
	if (it == id.end())
		throw Error_Pokemon_bad_id();
	return it - id.begin();
}

bool Pokemon::is_id(int new_id)
{
	if (find(id.begin(), id.end(), id) == id.end()) return 0;
	return 1;
}