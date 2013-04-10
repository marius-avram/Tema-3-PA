#include "Graph.h"

void Graph::setCapacity(short lambda, short threshold){
	short diff;
	for (int i = 0; i < height-1; i++){
		for (int j = 0; j < width-1; j++){
			/* Verificam vecinul drept */
			diff = matrix[i][j].value - matrix[i][j+1].value;
			if (std::abs(diff) <= threshold){
				matrix[i][j].right = lambda;
				matrix[i][j+1].left = lambda;
			}
			/* verificam vecinul din partea de jos.*/
			diff = matrix[i][j].value - matrix[i+1][j].value;
			if (std::abs(diff) <= threshold){
				matrix[i][j].down = lambda;
				matrix[i+1][j].up = lambda;
			}
		}
	}

	/* Adaugam capacitati pentru ultima linie din matrice.*/
	for (int i = 0; i<height-1; i++){
		diff = matrix[i][width-1].value - matrix[i+1][width-1].value;
		if (std::abs(diff) <= threshold){
			matrix[i][width-1].down = lambda;
			matrix[i+1][width-1].up = lambda;
		}
	}

	/* Adaugam capacitati pentru ultima coloana din matrice */
	for (int j = 0; j<width-1; j++){
		diff = matrix[height-1][j].value - matrix[height-1][j+1].value;
		if (std::abs(diff) <= threshold) {
			matrix[height-1][j].right = lambda;
			matrix[height-1][j+1].left = lambda;
		}
	}
}

void Graph::sourceAndDrainCapacity(double niuF, double sigmaF, 
								   double niuB, double sigmaB) {
	for(int i=0; i < height; i++){
		for (int j=0; j < width; j++){
			/* Se aplica formula pentru fiecare nod in parte */
			sourceCapacity[i][j] = 1.0F/2.0F * 
			std::pow(((float)matrix[i][j].value - niuF)/sigmaF, 2) +
			(float)std::log(std::sqrt(2*M_PI*std::pow(sigmaF, 2)));
			if (sourceCapacity[i][j] > 10) {
				sourceCapacity[i][j] = 10.0F;
			}
			drainCapacity[i][j] = 1.0F/2.0F * 
			std::pow(((float)matrix[i][j].value - niuB)/sigmaB, 2) + 
			(float)std::log(std::sqrt(2*M_PI*std::pow(sigmaB, 2)));
			if (drainCapacity[i][j] > 10) {
				drainCapacity[i][j] = 10.0F;
			}
		}
	}
	
}

std::list<Position> Graph::getNeighbours(short i, short j){
		
		std::list<std::pair<short, short> > result;
		if (i < height-1 && matrix[i][j].down != 0)
			result.push_back(std::pair<int, int>(i+1, j));
		if (j < width-1 && matrix[i][j].right != 0)
			result.push_back(std::pair<int, int>(i, j+1));
		if (i > 0 && matrix[i][j].up != 0)
			result.push_back(std::pair<int, int>(i-1, j));
		if (j > 0 && matrix[i][j].left != 0)
			result.push_back(std::pair<int, int>(i, j-1));
		return result;
}

std::list<Position> Graph::getNeighbours(Position pos){
	return getNeighbours(pos.first, pos.second);
}

double Graph::getEdgeCapacity(Position pos1, Position pos2, 
									 bool saturate, double value){
	short xdiff = pos1.first - pos2.first;
	short ydiff = pos1.second - pos2.second;
	/* Daca nu se doreste saturarea cu o anumita valoare a muchiei
	 * gasite atunci ne asiguram ca valoarea data este 0 */
	if (!saturate)
		value = 0;
	if (xdiff == -1){
		matrix[pos1.first][pos1.second].down -= value;
		matrix[pos2.first][pos2.second].up -= value;
		return matrix[pos1.first][pos1.second].down;
	}
	else if (xdiff == 1){
		matrix[pos1.first][pos1.second].up -= value;
		matrix[pos2.first][pos2.second].down -= value;
		return matrix[pos1.first][pos1.second].up;
	}
	else if (ydiff == 1){
		matrix[pos1.first][pos1.second].left -=value;
		matrix[pos2.first][pos2.second].right -= value;
		return matrix[pos1.first][pos1.second].left;

	}
	else if (ydiff == -1){
		matrix[pos1.first][pos1.second].right -= value;
		matrix[pos2.first][pos2.second].left -= value;
		return matrix[pos1.first][pos1.second].right;
	}
	return -1;
}




std::istream& operator>>(std::istream& in, Graph& graph){
	std::string type;
	getline(in, type);
	if (type != "P2") {
		std::cout << "Tipul fisierului trebuie sa fie P2" << std::endl;
	}
	in >> graph.width;
	in >> graph.height;
	in >> graph.maxval;
	/* Alocam spatiu pentru matrici */
	graph.matrix = new Node*[graph.height];
	graph.sourceCapacity = new double*[graph.height];
	graph.drainCapacity = new double*[graph.height];

	for (int i = 0; i < graph.height; i++) {
		graph.matrix[i] = new Node[graph.width];
		graph.sourceCapacity[i] = new double[graph.width];
		graph.drainCapacity[i] = new double[graph.width];
		for (int j = 0; j < graph.width; j++) {
			in >> graph.matrix[i][j].value;
		}
	}
	return in;
}

std::istream& operator>>(std::istream& in, Mask& mask){
	std::string type;
	getline(in, type);
	if (type != "P2") { 
		std::cout << "Tipul fisierului trebuie sa fie P2" << std::endl;
	}
	in >> mask.width; 
	in >> mask.height;
	in >> mask.maxval;
	short value;
	for (int i = 0; i < mask.height; i++){ 
		for (int j = 0; j < mask.width; j++){
			in >> value;
			if (value != 0)
				mask.nodes.push_back(std::pair<short, short>(i,j));
		}
	}
	return in;
}