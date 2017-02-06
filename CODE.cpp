#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <climits>
#include <chrono>

int symbol_player = 0;
int symbol_computer = 0;

int count = 0;

int limite = INT_MAX;
bool optimize = false;
bool first = true;

char table_entry(const char* table, int index)
{
	if(table[index] == 1)
	{
		return 'X';
	}
	else if(table[index] == 2)
	{
		return 'O';
	}

	return '0' + index + 1;
}

void print_table(const char* table)
{
	printf("     |     |\n");
	printf("  %c  |  %c  |  %c\n", table_entry(table, 0), table_entry(table, 1), table_entry(table, 2));
	printf("     |     |\n");
	printf("-----+-----+-----\n");
	printf("     |     |\n");
	printf("  %c  |  %c  |  %c\n", table_entry(table, 3), table_entry(table, 4), table_entry(table, 5));
	printf("     |     |\n");
	printf("-----+-----+-----\n");
	printf("     |     |\n");
	printf("  %c  |  %c  |  %c\n", table_entry(table, 6), table_entry(table, 7), table_entry(table, 8));
	printf("     |     |\n\n");
}

int verify(const char* table)
{
	int possible_lines[8][3] = { { 0,1,2 },{ 3,4,5 },{ 6,7,8 },{ 0,3,6 },{ 1,4,7 },{ 2,5,8 },{ 0,4,8 },{ 2,4,6 } };

	for(int i = 0; i < 8; i++)
	{
		if(table[possible_lines[i][0]] == table[possible_lines[i][1]] && table[possible_lines[i][1]] == table[possible_lines[i][2]])
		{
			if(table[possible_lines[i][2]] != 0)
			{
				return table[possible_lines[i][0]];
			}
		}
	}

	return 0;
}


bool terminal(const char* table)
{
	bool flag = true;

	for(int i = 0; i < 9; i++)
	{
		if(table[i] == 0)
		{
			flag = false;
			break;
		}
	}
	return (flag || verify(table) != 0);
}


int current_possible_wins(const char* table, int symbol)
{
	int c = 0;

	int possible_lines[8][3] = { { 0,1,2 },{ 3,4,5 },{ 6,7,8 },{ 0,3,6 },{ 1,4,7 },{ 2,5,8 },{ 0,4,8 },{ 2,4,6 } };
	for(int i = 0; i < 8; i++)
	{
		if(((table[possible_lines[i][0]] == symbol) || (table[possible_lines[i][0]] == 0)) &&
			((table[possible_lines[i][1]] == symbol) || (table[possible_lines[i][1]] == 0)) &&
			((table[possible_lines[i][2]] == symbol) || (table[possible_lines[i][2]] == 0)))
			c++;

		return c;
	}
}


int utility_not_final(const char* table)
{
	int a = current_possible_wins(table, symbol_computer);
	int b = current_possible_wins(table, symbol_player);

	return a - b;
}

int utility_final(const char* table)
{
	int verify_output = verify(table);

	if(verify_output == symbol_computer)
	{
		return 10;
	}
	else if(verify_output == symbol_player)
	{
		return -10;
	}
	else
	{
		return 0;
	}
}

int max_value(const std::array<char, 9>& state, int alfa, int beta, int level);

int min_value(const std::array<char, 9>& state, int alfa, int beta, int level)
{
	count++;
	if(terminal(state.data()))
	{
		return utility_final(state.data());
	}

	if(level > limite)
	{
		return utility_not_final(state.data());
	}

	int v = INT_MAX;

	for(int i = 0; i < 9; i++)
	{
		std::array<char, 9> move = state;

		if(state[i] != 0)
		{
			continue;
		}

		move[i] = symbol_player;

		v = std::min(v, max_value(move, alfa, beta, level + 1));

		if(optimize)
		{
			if(v <= alfa)
				return v;

			beta = std::max(beta, v);
		}
	}

	return v;
}

int max_value(const std::array<char, 9>& state, int alfa, int beta, int level)
{
	count++;
	if(terminal(state.data()))
	{
		return utility_final(state.data());
	}

	if(level > limite)
	{
		return utility_not_final(state.data());
	}

	int v = INT_MIN;

	for(int i = 0; i < 9; i++)
	{
		std::array<char, 9> move = state;

		if(state[i] != 0)
		{
			continue;
		}

		move[i] = symbol_computer;

		v = std::max(v, min_value(move, alfa, beta, level + 1));

		if(optimize)
		{
			if(v >= beta)
				return v;

			alfa = std::max(alfa, v);
		}
	}

	return v;
}

std::array<char, 9> computerMove(const std::array<char, 9>& state)
{
	std::cout << "Movimento do Computador:" << std::endl << std::endl;
	int v = INT_MIN;
	int utility, a;
	int positions[3] = { 0,1,4 };

	std::array<char, 9> max_move;

	if(first) a = 3;
	else a = 9;


	for(int i = 0; i < a; i++)
	{
		std::array<char, 9> move = state;

		if(state[i] != 0)
		{
			continue;
		}

		if(first)
		{
			move[positions[i]] = symbol_computer;
		}
		else
		{
			move[i] = symbol_computer;
		}

		int x = min_value(move, INT_MIN, INT_MAX, 0);

		if(x > v)
		{
			v = x;
			max_move = move;
			utility = utility_not_final(move.data());
		}
		else if(x == v)
		{
			int a = utility_not_final(move.data());
			if(a > utility)
			{
				v = x;
				max_move = move;
				utility = a;
			}
		}
	}

	return max_move;
}

int playerMove(const std::array<char, 9>& state)
{
	int entry = 0;
	while(entry < 1 || entry > 9 || state[entry - 1] != 0)
	{
		std::cout << "Insira o numero da casa em que quer jogar: ";

		std::cin >> entry;

		std::cout << std::endl;
	}

	return entry;
}

int main()
{
	std::cout << std::endl << "JOGO DO GALO" << std::endl << std::endl;
	//ESSCOLHA DO SIMBOLO
	std::cout << "1 -> X" << "        " << "2 -> O" << std::endl;
	std::cout << "Escolha o simbolo que pretende utilizar:  ";

	while(symbol_player != 1 && symbol_player != 2)
	{
		std::cin >> symbol_player;
	}

	if(symbol_player == 1)
	{
		symbol_computer = 2;
	}

	else
	{
		symbol_computer = 1;
	}


	std::cout << std::endl << "Responda com .." << std::endl;
	std::cout << "s -> sim" << "      " << "n -> nao" << std::endl << std::endl;

	char player_choice;

	//UM
	std::cout << "Pretende ser o primeiro a jogar?         ";

	player_choice = 0;
	while(player_choice != 's' && player_choice != 'n')
	{
		std::cin >> player_choice;
	}

	bool turn_player = false;

	if(player_choice == 's')
	{
		turn_player = true;
	}
	std::cout << std::endl;


	//DOIS
	std::cout << "Pretende definir um limite?              ";

	player_choice = 0;
	while(player_choice != 's' && player_choice != 'n')
	{
		std::cin >> player_choice;
	}

	if(player_choice == 's')
	{
		limite = 4;
	}
	std::cout << std::endl;


	//TRES
	std::cout << "Pretende otimizar usando Alfa e Beta?    ";

	player_choice = 0;
	while(player_choice != 's' && player_choice != 'n')
	{
		std::cin >> player_choice;
	}

	if(player_choice == 's')
	{
		optimize = true;
	}
	std::cout << std::endl;




	std::array<char, 9> table = { 0,0,0,0,0,0,0,0,0 };

	print_table(table.data());

	int winner = 0;
	int num_expanded_nodes[5] = { 0,0,0,0,0 };
	double time[5] = { 0,0,0,0,0 };

	int c = 0;

	for(int turn = 0; turn < 9 && winner == 0; turn++, winner = verify(table.data()))
	{

		if(turn_player)
		{
			int entry = playerMove(table);

			table[entry - 1] = symbol_player;

			turn_player = false;
		}
		else
		{
			std::chrono::time_point<std::chrono::system_clock> start;
			start = std::chrono::system_clock::now();

			table = computerMove(table);

			std::chrono::time_point<std::chrono::system_clock> end;
			end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed = end - start;

			num_expanded_nodes[c] = count;
			time[c] = elapsed.count();
			c++;
			count = 0;

			turn_player = true;
		}
		first = false;

		print_table(table.data());
	}


	std::cout << std::endl;

	if(winner == symbol_player)
	{
		std::cout << "Ganhaste!" << std::endl;
	}
	else if(winner == symbol_computer)
	{
		std::cout << "Perdeste." << std::endl;
	}
	else
	{
		std::cout << "Empate." << std::endl;
	}



	std::cout << "              Nos Vistos    Tempo" << std::endl;
	for(int i = 0; i < c; i++)
	{
		std::cout << "Escolha " << i + 1 << ":    ";
		std::cout.precision(10);
		std::cout.width(10);
		std::cout << std::left << num_expanded_nodes[i] << "    ";
		std::cout << std::left << std::fixed << time[i] << std::endl;
	}

	return 0;
}
