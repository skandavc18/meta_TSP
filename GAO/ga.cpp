// C++ implementation of the above approach 
#include <string>
#include<iostream> 
#include<vector>
#include <limits.h> 
#include<algorithm>
#include<cmath>
using namespace std; 
  
// Number of cities in TSP 
#define V 10
  
// Names of the cities 
#define GENES ABCDEFGHIJ
  
// Starting Node Value 
#define START 0 
  
// Initial population size for the algorithm 
#define POP_SIZE 100
  
// Structure of a GNOME 
// string defines the path traversed 
// by the salesman while the fitness value 
// of the path is stored in an integer 
  
struct individual { 
    string gnome; 
    int fitness; 
}; 
  
// Function to return a random number 
// from start and end 
int rand_num(int start, int end) 
{ 
    int r = end - start; 
    int rnum = start + rand() % r; 
    return rnum; 
} 
  
// Function to check if the character 
// has already occurred in the string 
bool repeat(string s, char ch) 
{ 
    for (int i = 0; i < s.size(); i++) { 
        if (s[i] == ch) 
            return true; 
    } 
    return false; 
} 
  
// Function to return a mutated GNOME 
// Mutated GNOME is a string 
// with a random interchange 
// of two genes to create variation in species 
string mutatedGene(string gnome) 
{ 
    while (true) { 
        int r = rand_num(1, V); 
        int r1 = rand_num(1, V); 
        if (r1 != r) { 
            char temp = gnome[r]; 
            gnome[r] = gnome[r1]; 
            gnome[r1] = temp; 
            break; 
        } 
    } 
    return gnome; 
} 
  
// Function to return a valid GNOME string 
// required to create the population 
string create_gnome() 
{ 
    string gnome = "0"; 
    while (true) { 
        if (gnome.size() == V) { 
            gnome += gnome[0]; 
            break; 
        } 
        int temp = rand_num(1, V); 
        if (!repeat(gnome, (char)(temp + 48))) 
            gnome += (char)(temp + 48); 
    } 
    return gnome; 
} 
  
// Function to return the fitness value of a gnome. 
// The fitness value is the path length 
// of the path represented by the GNOME. 
int cal_fitness(string gnome,int map[V][V]) 
{ 
    int f = 0; 
    for (int i = 0; i < gnome.size() - 1; i++) { 
        if (map[gnome[i] - 48][gnome[i + 1] - 48] == INT_MAX) 
            return INT_MAX; 
        f += map[gnome[i] - 48][gnome[i + 1] - 48]; 
    } 
    return f; 
} 
  
// Function to return the updated value 
// of the cooling element. 
int cooldown(int temp) 
{ 
    return (90 * temp) / 100; 
} 
  
// Comparator for GNOME struct. 
bool lessthan(struct individual t1, 
              struct individual t2) 
{ 
    return t1.fitness < t2.fitness; 
} 
  
// Utility function for TSP problem. 
void TSPUtil(int map[V][V]) 
{ 
    // Generation Number 
    int gen = 1; 
    // Number of Gene Iterations 
    int gen_thres = 5; 
  
    vector<struct individual> population; 
    struct individual temp; 
  
    // Populating the GNOME pool. 
    for (int i = 0; i < POP_SIZE; i++) { 
        temp.gnome = create_gnome(); 
        temp.fitness = cal_fitness(temp.gnome,map); 
        population.push_back(temp); 
    } 
  
    //cout << "\nInitial population: " << endl << "GNOME     FITNESS VALUE\n"; 
    for (int i = 0; i < POP_SIZE; i++) 
        //cout << population[i].gnome << "   "<< population[i].fitness << endl; 
    //cout << "\n"; 
  
    bool found = false; 
    int temperature = 10000; 
  
    // Iteration to perform 
    // population crossing and gene mutation. 
    while (temperature > 1000 && gen <= gen_thres) { 
        sort(population.begin(), population.end(), lessthan); 
        //cout << "\nCurrent temp: " << temperature << "\n"; 
        vector<struct individual> new_population; 
  
        for (int i = 0; i < POP_SIZE; i++) { 
            struct individual p1 = population[i]; 
  
            while (true) { 
                string new_g = mutatedGene(p1.gnome); 
                struct individual new_gnome; 
                new_gnome.gnome = new_g; 
                new_gnome.fitness = cal_fitness(new_gnome.gnome,map); 
  
                if (new_gnome.fitness <= population[i].fitness) { 
                    new_population.push_back(new_gnome); 
                    break; 
                } 
                else { 
  
                    // Accepting the rejected children at 
                    // a possible probablity above threshold. 
                    float prob = pow(2.7, 
                                     -1 * ((float)(new_gnome.fitness 
                                                   - population[i].fitness) 
                                           / temperature)); 
                    if (prob > 0.5) { 
                        new_population.push_back(new_gnome); 
                        break; 
                    } 
                } 
            } 
        } 
  
        temperature = cooldown(temperature); 
        population = new_population; 
        //cout << "Generation " << gen << " \n"; 
        //cout << "GNOME     FITNESS VALUE\n"; 
        //for (int i = 0; i < POP_SIZE; i++) 
            //cout << population[i].gnome << "   "<< population[i].fitness << endl; 
        gen++; 
    } 
    int min=population[0].fitness;
        for (int i = 0; i < POP_SIZE; i++) 
        {
            if(min>population[i].fitness)
                min=population[i].fitness;
        }
    cout << min << endl;
} 
  
int main() 
{ 
  
int gr[][10]={{0, 69, 90, 58, 45, 58, 77, 85, 16, 87}, {24, 0, 61, 48, 71, 61, 72, 3, 34, 8}, {87, 15, 0, 36, 75, 26, 25, 8, 30, 56}, {22, 60, 49, 0, 17, 95, 98, 76, 91, 91}, {9, 24, 69, 76, 0, 48, 98, 94, 25, 27}, {53, 90, 83, 34, 14, 0, 52, 62, 16, 91}, {39, 67, 68, 41, 13, 78, 0, 23, 28, 57}, {15, 79, 67, 100, 32, 77, 51, 0, 5, 97}, {96, 28, 82, 40, 46, 10, 81, 33, 0, 5}, {52, 44, 11, 87, 31, 61, 97, 32, 15, 0}};
TSPUtil(gr); 
} 