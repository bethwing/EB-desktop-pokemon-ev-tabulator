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

struct Error_pokemon_bad_id {};
struct Error_pokemon_repeated_id {};

enum class poke_base_stat : size_t
{
	h = 0,
	a, d,
	p, q,
	s,
	size
};

typedef array<unsigned char, (size_t)poke_base_stat::size> Effort_Values;

class Pokemon
{
public:
	Pokemon()
	{
		const size_t name_buffer_size = 512;
		name_buffer.resize(name_buffer_size);

		const size_t ev_buffer_size = (size_t) poke_base_stat::size * name_buffer.size();
		ev_buffer = vector<unsigned char>(ev_buffer_size, 0);

	}

	const unsigned int max_ev = 255;

	//creates a new pokemon
	void push_back(string new_name, char new_id);

	inline void add(char id, Effort_Values& ev) { add(get_index(id), ev); }
	void add(size_t index, Effort_Values& ev);

	//can throw Error_pokemon_get_index
	inline void get_ev(char id, Effort_Values& out) { get_ev(get_index(id), out); }
	void get_ev(size_t index, Effort_Values& out);

	//throws Error_pokemon_get_index if the id is not found
	size_t get_index(char uk_id);

	inline char get_name(char id) { return get_name(get_index(id)); }

	inline size_t get_num_pokemon()
	{
		return id.size();
	}

private:
	//used to verify that a pokemon id is not already in Pokemon::id in push_back
	bool is_id(char new_id);



private:
	vector<string> name_buffer;
	vector<unsigned char> ev_buffer;
	vector<char> id;
};


//creates a new pokemon
void Pokemon::push_back(string new_name, char new_id)
{
	//numpokemon_n-1 = index_newpokemon
	//'pokemon' can be plural or singular
	if (get_num_pokemon() < name_buffer.size())
	{
		name_buffer[get_num_pokemon()] = new_name;
		id.push_back(new_id);
	}
	else
		throw Error_pokemon_push_back();
}

void Pokemon::get_evs(size_t index, Effort_Values& out)
{
	assert(index < id.size());
	for (size_t i = 0; i < (size_t)poke_base_stat::size; ++i)
	{
		out[i] = ev_buffer[i * name_buffer.size() + index];
	}
}

//throws Error_pokemon_get_index if the id is not found
size_t Pokemon::get_index(char uk_id)
{
	std::vector<char>::iterator it = find(id.begin(), id.end(), uk_id);
	if (it == id.end())
		throw Error_pokemon_bad_id();
	return it - id.begin();
}

void Pokemon::add(size_t index, Effort_Values& ev)
{
	assert(index < id.size());
	for (size_t i = 0; i < (size_t)poke_base_stat::size; ++i)
	{
		const int j = i * evlist_size + index;
		const unsigned int temp = ev_buffer[j];
		ev_buffer[j] = ceil_pos_sum(ev[i], temp, max_ev);
	}
	return ev_index;
}