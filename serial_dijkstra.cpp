/* Implementation of SP-TAG algorithms
   using serial algorithms.
   
   for parallel: goto
   					/workspace/Mtech_project/other/parallel_main.cpp
*/

#include <iostream>
#include <unordered_map>
#include <map>
#include <vector>	
#include <algorithm>
#include <fstream>
#include <iterator>
#include <cstdlib>
#include <climits>

#define inf INT_MAX
#define time_length 10080
#define N 1417

using namespace std;

typedef struct Node{
	int dest_id;
	int *time_funct;
}node;

//unordered_map<int, vector<node*> > adj_list;
typedef vector<node*> link;
typedef map <int, vector<node*> > Graph;
Graph adj_list;


void add_edge(int src, node* ptr);
void show();
node* newnode(int dest);
node* dest_Pointer(int src, int dest);
void insert(int src, int dest, int time_series, int travel_time);
void Dijkstra(int src, int time);
int get_min(map<int, int> dist, map<int, bool> visited, int time);
void show_dist(map<int, int> dist, int src);


int main(int argc, char const *argv[])
{
	//cout<<argc<<"\n";
	clock_t start, end;
	string name;
	if(argc==2)
		name = argv[1];
	else
		name = "test.csv";

	ifstream file(name);

	if(!file.is_open())
		cout<<"ERROR: File Open"<<"\n";

	string edge_id, src, dest;
	string time_series, travel_time;

	int prev_src=-1;
	int pre_time_cost=-1;
	node *tmp=NULL;
	
	int depature_node = 3;
	int depature_time = 100;

	//Graph graph = Graph(N);
	//graph.V = -1;
	cout << "creating graph...\n";
	start = clock();

	while(file.good()){
		getline(file, edge_id, ',');
		getline(file, src, ',');
		getline(file, dest, ',');
		getline(file, time_series, ',');
		getline(file, travel_time, '\n');
		
		int s_src =  stoi(src);
		int t_time = stoi(travel_time);
		/* look up at src and cost if both are same no insertion
			will be here atleast one parameter must change 
			(avoiding duplicates records here)
		*/
		//if(prev_src != s_src || pre_time_cost != t_time){
		//	prev_src = s_src; pre_time_cost = t_time;
			insert(s_src, stoi(dest), stoi(time_series), t_time);
		//}
	}
	//show();
	cout << "graph created.\n";
	end = clock() - start;
	double duration = double(end - start) / double(CLOCKS_PER_SEC); 
	cout << duration << '\n';

	start = clock();
	Dijkstra(depature_node, depature_time);
	end = clock() - start;
	duration = double(end - start) / double(CLOCKS_PER_SEC); 
	cout << duration << '\n';
	
	return 0;
}

void Dijkstra(int src, int time){
	map<int, int> dist;
	map<int, bool> visited;
	vector<node*>::iterator it;

	int i;
	int j;
	int min_index;
	int curr_time = time;

	for (auto &p : adj_list){
		dist[p.first] = inf;
		visited[p.first] = false;
	}

	dist[src] = curr_time;

	for (i = 0; i < N; i++){
		min_index = get_min(dist, visited, curr_time);
		if(min_index == -1){
			cout << "No index found \n Abort !!!\n";
			_Exit(1);
		}

		vector<node*> ptr = adj_list[min_index];
		for( it = ptr.begin(); it != ptr.end(); it++){
			if (dist[(*it)->dest_id] > (*it)->time_funct[dist[min_index]]){
				//cout << dist[(*it)->dest_id] << ' ' << dist[min_index] << ' ' << (*it)->time_funct[dist[min_index]] << '\n'; 
				dist[(*it)->dest_id] = (*it)->time_funct[dist[min_index]];

			}
		}
		visited[min_index] = true;
	}

	show_dist(dist, src);
}

void show_dist(map<int, int> dist, int src){
	cout <<"from "<< src << "		distance \n";
	cout <<"------" << "		--------\n";
	for ( auto p : dist ){
		if (p.second == inf)
			cout << p.first << "\t\t" << "inf" << '\n';
		else	
			cout << p.first << "\t\t" << p.second << '\n';
	}
	cout << '\n';
}

int get_min(map<int, int> dist, map<int, bool> visited, int time){
	int min = inf;
	int index = -1;

	for (auto &p : dist){
		if(!visited[p.first] && dist[p.first] < min){
			min = dist[p.first];
			index = p.first;
		}
	}
	return index;
}

void add_edge(int src, node* ptr){
	adj_list[src].push_back(ptr);
}

void show(){
	//unordered_map<int, vector<node*> >::iterator it;
	map<int, vector<node*> >::iterator it;

	for(it = adj_list.begin(); it != adj_list.end(); it++){
		cout<<it->first<<" |";
		vector<node*> v = it->second;

		for(int i=0; i<v.size(); i++){
			cout<<" --> "<<v[i]->dest_id<<" { ";
		
			for(int k=0;k<time_length; k++){
				//cout<<"[ ";
				if(v[i]->time_funct[k] != inf)
					cout<<k<<":"<<v[i]->time_funct[k]<<", ";
			}
			cout<<" }";
		}
		cout<<"\n";
	}
}

node* newnode(int dest){
	node* tmp = new node();
	tmp->dest_id = dest;
	tmp->time_funct = new int[time_length];

	for(int i=0; i<time_length; i++)
		tmp->time_funct[i] = inf;
	return tmp;
}

node* dest_Pointer(int src, int dest){
	vector<node* > v = adj_list[src];

	for(int i=0; i<v.size(); i++){
		if(v[i]->dest_id == dest)
			return v[i];
	}
	return NULL;
}

void insert(int src, int dest, int time_series, int travel_time){
	if(adj_list[src].size()==0){
		//points.push_back(src);

		node* tmp = newnode(dest);
		tmp->time_funct[time_series] = travel_time+time_series;

		add_edge(src, tmp);
	}
	
	else if(adj_list[src].size()){
		// if dest already exist
		node* ptr = dest_Pointer(src, dest); 
		if (ptr != NULL)
			ptr->time_funct[time_series] = travel_time+time_series;
		else{
			node* tmp = newnode(dest);
			tmp->time_funct[time_series] = travel_time+time_series;

			adj_list[src].push_back(tmp);
		}
	}
}