#include <cstdlib> 
#include <iostream>
#include <string> 
#include <sstream> 
#include <math.h> 
#include <ctime>  
using namespace std;

#define INVALID -1

#define CITY_AMOUNT 50
#define POPULATION_SIZE 150
#define PHEROMONE_RATE 0.1
#define ALFA 1
#define BETA 2
#define MAX_ITERATIONS 30
#define EVAPORATION_RATE 0.5
#define POSITIVE_CONTS 0.75

struct ant {
	double distance;
	double fitness;
	int position;
	int route[CITY_AMOUNT];
};


ant ants[POPULATION_SIZE];
double pheromone_links[CITY_AMOUNT][CITY_AMOUNT];
int distance_links[CITY_AMOUNT][CITY_AMOUNT] ;
int best_distance = 1000000000;
int worse_distance = 0;
double best_fitness;
double worse_fitness;
int best_route[CITY_AMOUNT];
int worse_route[CITY_AMOUNT];
double average = 0.0;
double variance = 0.0;
double standard_deviation = 0.0;
int greater_distance = INVALID;

void init_ant(int index);
void seed_initial_pheromone(bool random);
void get_greater_distance();
bool check_visit(int ant_index, int position);
void build_solution();
void check_best_distance();
void calculate_fitness();
void pheromone_evaporates();
void update_pheromone();

double calculate_time(clock_t start, clock_t end);
int get_random_number(int from, int to);
void print_route(int ant_index, int route[CITY_AMOUNT], double distance);
void print_pheromone();
string number_to_String(double n);
void print_result();
void calculate_metrics();

void input_graph()
{
	for(int i=0;i<CITY_AMOUNT;i++)
	{
		for(int j=0;j<CITY_AMOUNT;j++)
		{
			cin>>distance_links[i][j];
			if(i==j)
			{
				distance_links[i][j]=INVALID;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	input_graph();
	clock_t time_start = clock();

	srand(time(0));

	int iteration = 0;

	seed_initial_pheromone(false);

	get_greater_distance();


	while (iteration < MAX_ITERATIONS) {
		build_solution();
		check_best_distance();
		calculate_fitness();
		pheromone_evaporates();
		update_pheromone();
		for (int i = 0; i < POPULATION_SIZE; i++) {
			print_route(i, ants[i].route, ants[i].distance);
		}
		iteration++;
	}


	print_result();


	return 0;
}

void init_ant(int index) {
	ants[index].distance = 0;
	ants[index].fitness = 0;
	ants[index].position = 0;
	for (int j = 0; j < CITY_AMOUNT; j++) {
		ants[index].route[j] = INVALID;
	}

	int random_city = get_random_number(0, (CITY_AMOUNT - 1));
	ants[index].route[0] = random_city;
}

void seed_initial_pheromone(bool random) {
	for (int i = 0; i < CITY_AMOUNT; i++) {
		for (int j = 0; j < CITY_AMOUNT; j++) {
			if (i != j) {
				double random_pheromone = (double) get_random_number(0, 100)
						/ 100.0;
				pheromone_links[i][j] = (random == true) ? random_pheromone
						: PHEROMONE_RATE;
			} else {
				pheromone_links[i][j] = INVALID;
			}
		}
	}
}

void get_greater_distance() {
	for (int i = 0; i < CITY_AMOUNT; i++) {
		for (int j = 0; j < CITY_AMOUNT; j++) {
			if (distance_links[i][j] > greater_distance) {
				greater_distance = distance_links[i][j];
			}
		}
	}
	greater_distance += 1;
}

bool check_visit(int ant_index, int position) {
	for (int i = 0; i < CITY_AMOUNT; i++) {
		if (ants[ant_index].route[i] == position) {
			return true;
		}
	}
	return false;
}

void build_solution() {

	for (int i = 0; i < POPULATION_SIZE; i++) {

		init_ant(i);


		while (ants[i].position < (CITY_AMOUNT - 1)) {
			int position = ants[i].position;

			double transition_probability[CITY_AMOUNT];

			double link_rate_sum = 0;

			for (int j = 0; j < CITY_AMOUNT; j++) {

				if (check_visit(i, j) == false) {
					if (pheromone_links[position][j] >= 0
							and distance_links[position][j] >= 0) {
						link_rate_sum
								+= pow(pheromone_links[position][j], ALFA)
										* pow(
												greater_distance
														- distance_links[position][j],
												BETA);
					}
				}
			}

			for (int j = 0; j < CITY_AMOUNT; j++) {

				if (check_visit(i, j) == false) {
					if (pheromone_links[position][j] >= 0
							and distance_links[position][j] >= 0) {
						transition_probability[j] = (pow(
								pheromone_links[position][j], ALFA) * pow(
								greater_distance - distance_links[position][j],
								BETA)) / link_rate_sum;
					} else {
						transition_probability[j] = 0;
					}
				} else {
					transition_probability[j] = 0;
				}
			}

			double roulette = (double) get_random_number(0, 100) / 100.0;
			double minor = 0;
			double major = 0;

			for (int j = 0; j < CITY_AMOUNT; j++) {

				if (check_visit(i, j) == false) {
					major += transition_probability[j];
					if (roulette >= minor and roulette <= major) {
						ants[i].route[position + 1] = j;
						ants[i].distance += distance_links[position][j];
						ants[i].position += 1;
						break;
					} else {
						minor = major;
					}
				}
			}
		}
	}
}

void check_best_distance() {
	for (int i = 0; i < POPULATION_SIZE; i++) {
		if (ants[i].distance < best_distance) {
			best_distance = ants[i].distance;
			for (int j = 0; j < CITY_AMOUNT; j++) {
				best_route[j] = ants[i].route[j];
			}
		} else if (ants[i].distance > worse_distance) {
			worse_distance = ants[i].distance;
			for (int j = 0; j < CITY_AMOUNT; j++) {
				worse_route[j] = ants[i].route[j];
			}
		}
	}
}

void calculate_fitness() {
	for (int i = 0; i < POPULATION_SIZE; i++) {
		double fitness = (double) ants[i].distance / (double) best_distance;
		if (fitness < best_fitness) {
			best_fitness = fitness;
		} else if (fitness > worse_fitness) {
			worse_fitness = fitness;
		}
		ants[i].fitness = fitness;
	}
}

void pheromone_evaporates() {
	for (int i = 0; i < CITY_AMOUNT; i++) {
		for (int j = 0; j < CITY_AMOUNT; j++) {
			if (pheromone_links[i][j] != INVALID) {
				pheromone_links[i][j] = (1 - EVAPORATION_RATE)
						* pheromone_links[i][j];
			}
		}
	}
}

void update_pheromone() {
	for (int i = 0; i < POPULATION_SIZE; i++) {
		double pheromone_to_sum = POSITIVE_CONTS / ants[i].fitness;
		for (int j = 0; j < (CITY_AMOUNT - 1); j++) {
			int city = ants[i].route[j];
			int next_city = ants[i].route[j + 1];
			if (pheromone_links[city][next_city] != INVALID) {
				pheromone_links[city][next_city] += pheromone_to_sum;
			}
		}
	}
}


int get_random_number(int from, int to) {
	return (from < to) ? (rand() % to) + from : 0;
}

double calculate_time(clock_t start, clock_t end) {
	return 1000.0 * ((double) (end - start) / (double) CLOCKS_PER_SEC);
}

void print_route(int ant_index, int route[CITY_AMOUNT], double distance) {
	string temp = "Rota da formiga " + number_to_String(ant_index) + " : ";
	for (unsigned int i = 0; i < CITY_AMOUNT; i++) {
		temp += number_to_String(route[i]);
		if ((i + 1) != CITY_AMOUNT) {
			temp += ", ";
		}
	}
	//temp += ". Tamanho: " + number_to_String(distance) + "\n";
	//cout << number_to_String(distance)  << endl;
}

void print_pheromone() {
	string temp = "Taxas de feromônio:";
	temp += "\n{";
	for (int i = 0; i < CITY_AMOUNT; i++) {
		temp += "{";
		for (int j = 0; j < CITY_AMOUNT; j++) {
			temp += number_to_String(pheromone_links[i][j]);
			if ((j + 1) != CITY_AMOUNT) {
				temp += ", ";
			}
		}
		if ((i + 1) != CITY_AMOUNT) {
			temp += "}, ";
		} else {
			temp += "}";
		}
	}
	temp += "}\n";
	//cout << temp << endl;
}

void print_result() {
	//cout << "Pior resultado:" << endl;
	//cout << "f(x):" << worse_fitness << endl;
	//print_route(0, worse_route, worse_distance);

	//cout << "Melhor resultado:" << endl;
	//cout << "f(x):" << best_fitness << endl;
	//print_route(0, best_route, best_distance);
	int best_ans=100000;
	for(int i=0;i<POPULATION_SIZE;i++)
	{
		if(best_ans>ants[i].distance)
		best_ans=ants[i].distance;
	}

	cout<<best_ans<<endl;

	//calculate_metrics();
	//cout << "Média:" << average << endl;
	//cout << "Variância:" << variance << endl;
	//cout << "Desvio padrão:" << standard_deviation << endl;
}

string number_to_String(double n) {
	stringstream out;
	out << n;
	return out.str();
}

void calculate_metrics() {

	double sum = 0;
	for (int i = 0; i < POPULATION_SIZE; i++) {
		sum += ants[i].distance;
	}
	average = (double) sum / (double) POPULATION_SIZE;
	sum = 0;
	for (int i = 0; i < POPULATION_SIZE; i++) {
		sum += pow(ants[i].distance - average, 2);
	}
	variance = (double) sum / (double) POPULATION_SIZE;
	standard_deviation = pow(variance, 0.5);
}