#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <climits>
#include <time.h>
#include "Graph.h"


/* Fisierele de intrare */
#define IMAGINE "imagine.pgm"
#define BACKGROUND "mask_bg.pgm"
#define FOREGROUND "mask_fg.pgm"
#define PARAMETRI "parametri.txt"
#define SEGMENT "segment.pgm"

/* Afiseaza un mesaj de eroare la output si inchide procesul curent */
void errorMessage(std::string message){ 
	std::cout << "Fisierul " << message << " nu a putut fi deschis. "
			  << std::endl;
	exit(1);
}

/* Calculeaza Niu pentru un graf dat si o masca */
double determineNiu(Graph& graph, Mask& mask) {
	int size = mask.nodes.size();
	if (size == 0){
		return -1;
	}
	double sum = 0;
	if (!(graph.getWidth() == mask.getWidth() && 
		  graph.getHeight() == mask.getHeight())){
		/* Graful si masca au dimensiuni incompatibile */
		return -1;
	}
	std::list<std::pair<short, short> >::iterator it;
	for (it = mask.nodes.begin(); it != mask.nodes.end(); ++it){
		sum += graph.matrix[it->first][it->second].value;
	}
	return (double)sum/size;
}

/* Calculeaza Sigma pentru un graf dat si o masca */
double determineSigma(double niu, Graph& graph, Mask& mask){
	int size = mask.nodes.size(); 
	if (size == 0){
		return -1;
	}
	double result = 0;
	if (!(graph.getWidth() == mask.getWidth() && 
		  graph.getHeight() == mask.getHeight())){
		/* Graful si masca au dimensiuni incompatibile */
		return -1;
	}
	std::list<std::pair<short, short> >::iterator it;
	for (it = mask.nodes.begin(); it != mask.nodes.end(); ++it){
		result += std::pow(niu-graph.matrix[it->first][it->second].value,2);
	}
	return std::sqrt(result/size);
}

/* Determina o cale de la sursa la drena care trece prin nodul dat ca 
 * parametru si totodata determina costul minim de pe drumul respectiv.
 * Pentru fiecare nod din graf se va cauta o cale care sa contina doar
 * sursa->nod->drena. Cel mai sigur aceasta calea va fi gasita. Dupa 
 * ce s-a efectuat aceasta procedura pentru toate nodurile din graf 
 * se efectueaza bfs normal si se cauta calea cea mai scurta dintre 
 * sursa si drena (cum se face in mod normal). Pentru aceasta node
 * trebuie sa fie (-3, -3). */
std::vector<Position> bfs(Graph& graph, Position node, double& min){
	std::vector<Position> result;
	/* Retinem parintii nodurilor accesate intr-un map */
	std::vector<bool> visited(graph.getHeight()* graph.getWidth(), false);
	std::map<Position, Position> parents;
	int width = graph.getWidth();
	double cost;
	min = LONG_MAX;
	std::queue<Position> q;
	/* Vom coda in felul urmator nodurile : 
	 (-1, -1) - nodul sursa; (-2, -2) - nodul drena; */
	if (node.first != -3) {
		/* S-a dat un nod */
		if (graph.sourceCapacity[node.first][node.second] != 0){
			q.push(node);
			visited[node.first*width+node.second] = true;
		}	
	}
	else {
		/* Nu s-a dat nici un nod deci introducem in coada 
		 * toate elementele care sunt accesibile din sursa */
		for (int i = 0; i < graph.getHeight(); i++){
			for (int j = 0; j < width; j++){
				if (graph.sourceCapacity[i][j] != 0) {
					visited[i*width+j] = true;
					q.push(Position(i,j));
				}
			}
		}
	}
	bool found = false;
	Position current;
	int cycles = 0;
	while (!q.empty()) {
		current = q.front();
		/* Daca avem o cale catre drena din nodul curent il adaugam in
		 * mapul de parinti si oprim cautarea caii */
		if (graph.drainCapacity[current.first][current.second] != 0) {
			parents.insert(std::pair<Position, Position>
						   (Position(-2, -2), current));
			found = true;
			break;
		}
		std::list<Position> neighbours = graph.getNeighbours(current);
		std::list<Position>::iterator it;
		for (it = neighbours.begin(); it != neighbours.end(); ++it){
			if (visited[it->first*width+it->second] == false) {
				visited[it->first*width+it->second] = true;
				parents.insert(std::pair<Position, Position>(*it,current));
				q.push(*it);
			}
		}
		cycles++;
		q.pop();
	}
	
	if (found) {
		/* Daca s-a gasit calea o reconstituim si calculam totodata 
		 * capacitatea minima a muchiilor ce o alcatuiesc */
		Position prev = Position(-2, -2);
		prev = parents[prev];
		min = graph.drainCapacity[prev.first][prev.second];
		while(true) {
			result.push_back(prev);
			if (parents.find(prev) == parents.end()){
				break;
			}
			cost = graph.getEdgeCapacity(prev, parents[prev], false, 0);
			if (cost < min) { 
				min = cost;
			}
			prev = parents[prev];
		}
		if (graph.sourceCapacity[prev.first][prev.second] < min){
			min = graph.sourceCapacity[prev.first][prev.second];
		}
	}
	return result;
}

/* Primeste o cale si fluxul ce trebuie sa treaca prin calea 
 * respectiva. Realizeaza saturarea caii. */
void saturatePath(Graph& graph, std::vector<Position>& path, double flow){
	/* Calea este neinversata, insa nu conteaza pentru ca oricum graful 
	 * este neorientat si ne intereseaza doar saturarea muchiilor. */
	graph.drainCapacity[path[0].first][path[0].second] -= flow;
	for (int i = 0; i < (int)path.size()-1; i++){
		graph.getEdgeCapacity(path[i], path[i+1], true, flow);
	}
	graph.sourceCapacity[path[path.size()-1].first]
						[path[path.size()-1].second] -= flow;
	
}

/* Determina fluxul maxim in graful dat ca parametru */
double maxflow(Graph& graph) {
	double min = 0;
	double totalflow = 0;
	std::vector<Position> path;

	/* Initial se satureaza fiecare cale de forma 
	 * sursa -> nod -> drena */
	for (int i=0; i < graph.getHeight(); i++){
		for (int j=0; j < graph.getWidth(); j++){
			path.clear();
			path = bfs(graph, Position(i,j), min);
			if (path.size() != 0){
				saturatePath(graph, path, min);
				totalflow += min;
			}
		}
	}

	while(true){
		path.clear();
		path = bfs(graph, Position(-3,-3), min);
		
		if (path.size() != 0) { 
		/* S-a gasit o cale de la sursa la drena trecand prin cel
		 * putin un nod intermediar dat. Deci o saturam */
			saturatePath(graph, path, min);
			totalflow += min;
		}
		else {
			break;
		}
	}
	std::cout.precision(6);
	std::cout << "Total flow = " << std::fixed << totalflow << std::endl;
	return totalflow;
}

/* Determina taietura minima pe graful pe care s-a aplicat 
 * flux maxim anterior */
void mincut(Graph& graph){
	/* Punem toate nodurile accesibile din sursa intr-o coada*/
	std::queue<Position> q;
	for (short i = 0; i < graph.getHeight(); i++){
		for (short j = 0; j < graph.getWidth(); j++){
			if (graph.sourceCapacity[i][j] != 0) { 
				q.push(Position(i, j));
			}
		}
	}

	Position current;
	while (!q.empty()){
		current = q.front();
		if (!graph.matrix[current.first][current.second].marked) { 
			graph.matrix[current.first][current.second].marked = true;
			/* Metoda ce ne returneaza vecinii ne asigura ca acestia 
			 * au capacitatea ramasa diferita de zero */
			std::list<Position> neighbours = graph.getNeighbours(current);
			std::list<Position>::iterator it;
			for (it = neighbours.begin(); it != neighbours.end(); ++it){
				if (!graph.matrix[it->first][it->second].marked){
					q.push(*it);
				}
			}
		}
		q.pop();
	}

}

/* Scrie in fisierul "segment.pgm" poza obtinuta in urma aplicarii 
 * algoritmului de taietura minima */
void writeSegment(Graph& graph, std::ofstream& out){
	out << "P2" << std::endl;
	out << graph.getWidth() << " " << graph.getHeight() << std::endl;
	out << graph.getMaxval() << std::endl;
	for (short i = 0; i < graph.getHeight(); i++){
		for (short j = 0; j < graph.getWidth(); j++){
			if (graph.matrix[i][j].marked) 
				out << 0 << std::endl;
			else 
				out << 255 << std::endl;
		}
	}

}

int main(int argc, char *argv[]){

	clock_t start = clock();
	
	std::ifstream imagine(IMAGINE);
	if (!imagine.is_open())
		errorMessage(IMAGINE);

	std::ifstream background(BACKGROUND);
	if (!background.is_open())
		errorMessage(BACKGROUND);

	std::ifstream foreground(FOREGROUND);
	if (!foreground.is_open())
		errorMessage(FOREGROUND);

	std::ifstream parametri(PARAMETRI);
	if (!parametri.is_open())
		errorMessage(PARAMETRI);

	short lambda, threshold;
	parametri >> lambda; parametri >> threshold;
	parametri.close();

	Graph graphImage;
	Mask foregroundMask, backgroundMask;
	imagine >> graphImage;

	foreground >> foregroundMask;
	background >> backgroundMask;
	
	imagine.close();
	foreground.close();
	background.close();

	double niuF, niuB, sigmaF, sigmaB;
	niuF = determineNiu(graphImage, foregroundMask);
	
	sigmaF = determineSigma(niuF, graphImage, foregroundMask);
	niuB = determineNiu(graphImage, backgroundMask); 
	sigmaB = determineSigma(niuB, graphImage, backgroundMask);
	
	graphImage.setCapacity(lambda, threshold);
	graphImage.sourceAndDrainCapacity(niuF, sigmaF, niuB, sigmaB);
	
	std::ofstream segment(SEGMENT);

	maxflow(graphImage);
	mincut(graphImage);
	writeSegment(graphImage, segment);
	segment.close();

	std::cout << "Time taken: " << (double)(clock()-start)/CLOCKS_PER_SEC << std::endl;
	return 0;
}
