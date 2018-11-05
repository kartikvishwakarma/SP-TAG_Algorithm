/*
	Implementation of SP_TAG algorithms
	using parallel dijkstra algorithms
	on tdEdgeId.src.sink.time.csv dataset.
*/

//#include <chrono>
//#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <vector>
#include <utility>
#include <queue>
#include <map>
#include <omp.h>

#define inf INT_MAX
#define time_length 10080


using namespace std;


typedef struct{
	int dest_id;
	int *time_funct;
}tag_node;

//typedef vector<tag_node*> link;
typedef map<int, vector<int> > link;
typedef map<int, link> TAG_Graph;
typedef pair<int, int> pi;
typedef map<int, int> Map;
typedef vector<pi> Node;
typedef map<int, Node> Graph;
typedef priority_queue<pi, Node, greater<pi> > Queue;

Graph graph;
TAG_Graph tag_graph;
link mapper;
unordered_set<int> S;
Map dist;
map<int, bool> visited;


void init_txt(void);
void init_csv(string file);
void show_graph();
void show_tag_graph();
Map dijkstra(int src, int time);
void print(Map dist);
tag_node* newnode(int dest);
tag_node* dest_pointer(int src, int dest);
void Insert(int src, int dest, int time_series, int travel_time);


int main(){
	int src=3;
	int time=100;
	string file = "tdEdgeId.src.sink.time.csv";
	/*clock_t start, end;
	
	
	start = clock();
*/	cout << "creating graph ...\n";
	init_csv(file);
	/*end = clock() - start;
	double duration = double(end - start) / double(CLOCKS_PER_SEC); 
*/	cout << "graph created.\n";
	//cout << duration << '\n';
	
	//start = clock();
	Map dist = dijkstra(src, time);
	print(dist);
	/*end = clock() - end;
	duration = double(end - start) / double(CLOCKS_PER_SEC); 
	cout << duration << '\n';
	*/

	//show_graph();
	/*for (auto p : mapper){
	cout << p.first << " | "; 
		for(auto l : p.second){
			cout << " --> " << l;
		}
	cout << "\n\n";
	}
	*/
	dist.clear();
	graph.clear();
	tag_graph.clear();
	mapper.clear();
	return 0;
}

Map dijkstra(int src, int time){
	Queue Q;
	int s, w, n, val, u,i;

	for(auto s : S){
		dist[s] = inf;
		visited[s] = false;
	}
	S.clear();

	dist[src] = time;
	
	Q.push({dist[src], src});
	
	
	while(!Q.empty()){
		s = Q.top().second;
		w = Q.top().first;
		Q.pop();
		n = mapper[s].size();
		cout<<"size: " << n << '\n';
		if(n){
			//vector<int> ptr = mapper[s];
			#pragma omp parallel for num_threads(8) private( i,u, val) shared(n,Q, visited, dist, tag_graph, mapper)
			for(i=0; i<n; i++){
				u = mapper[s][i];
				val = tag_graph[s][u][w];
				if(dist[u] > val ){
					dist[u] = val;// + p.second;
					#pragma omp critical
					{
					cout<<s << '\t' << dist[u] << '\t' << u << '\n';
					Q.push({dist[u], u});
					}
				}
			}
		}
	}

	return dist;
}


void show_graph(){
	for(auto &p : graph){
		cout << p.first << " |";
		Node node = p.second;
		for(auto &l : node)
			cout << " --> (" << l.first << ',' << l.second << ") ";
		cout << '\n';
	}
}

void show_tag_graph(){
	for(auto &g : tag_graph){
		cout << g.first << " | ";
		link tmp = g.second;
		for(auto &l : tmp){
			cout << " --> " << l.first << " { ";
			vector<int> v = l.second;
			for(auto k : v)
				cout << k << ", ";
			cout << "} ";
		}
		cout << '\n';
	}
}

void init_csv(string name){
	string edge_id, src, dest;
	string time_series, travel_cost;
	int s_src, t_travel_cost, d_dest;
	int t_time, t_time_series;

	ifstream file(name);

	if(!file.is_open()){
		cout << "ERROR: in open file, " << name << '\n';
		_Exit(1);
	}

	while(file.good()){
		getline(file, edge_id, ',');
		getline(file, src, ',');
		getline(file, dest, ',');
		getline(file, time_series, ',');
		getline(file, travel_cost, '\n');

		s_src = stoi(src);
		d_dest = stoi(dest);
		t_travel_cost = stoi(travel_cost);
		t_time_series = stoi(time_series);

		Insert(s_src, d_dest, t_time_series, t_travel_cost);
		
		//graph[s_src].push_back({d_dest,0});
		
	}	

}

void Insert(int src, int dest, int time_series, int travel_time){
	if(tag_graph[src].size() == 0){
		tag_graph[src] = map<int, vector<int> >();
		tag_graph[src][dest] = vector<int>(time_length, inf);
		tag_graph[src][dest] = vector<int>(time_length, inf);
		mapper[src].push_back(dest);	
		S.insert(src);
		S.insert(dest);
	}
	else if(tag_graph[src][dest].size() == 0){
		tag_graph[src][dest] = vector<int>(time_length, inf);
		mapper[src].push_back(dest);	
		S.insert(src);
		S.insert(dest);
	}

	tag_graph[src][dest][time_series] = travel_time+time_series;
}

void init_txt(){
	int n, id, s, d, w;

	cin >> n;
	for(int i=0; i<n; i++){
		cin >> id >> s >> d >> w;
		graph[s].push_back({d,w});
		graph[d].push_back({s,w});
		S.insert(s);
		S.insert(d);

	}

	return;
}

void print(Map dist){
	for(auto &l : dist)
		cout << l.first << '\t' << l.second << '\n';
	cout << '\n';

	return;
}

/*tag_node* newnode(int dest){
	tag_node* tmp = new tag_node();
	tmp->dest_id = dest;
	tmp->time_funct = new int[time_length];

	for(int i=0; i<time_length; i++)
		tmp->time_funct[i] = inf;

	return tmp;
}

tag_node* dest_pointer(int src, int dest){
	link v = tag_graph[src];

	for(int i=0; i<v.size(); i++){
		if(v[i]->dest_id == dest)
			return v[i];
	}
	return NULL;
}
*/