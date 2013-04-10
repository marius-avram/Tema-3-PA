#ifndef _GRAPH_H_
#define _GRAPH_H_

#pragma once

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <iostream> 
#include <string>
#include <list>
#include <vector>
#include <cmath>
#include <fstream>

typedef std::pair<short, short> Position;

/* Reprezentarea unui nod din graf. Acesta contine 
 * valoarea citita din fisier, costurile pentru nodurile 
 * vecine si o variabila bool care spune daca este marcat 
 * sau nu. Va fi folosita in algoritmul de taietura minima */
struct Node{
	/* Valoare citita din fisier */
	unsigned short value;
	/* Capacitatiile catre nodurile vecine */
	double up, right, down, left;
	/* Marked, folosit pentru determinarea taieturii minime */
	bool marked;
	Node() : value(0), up(0), right(0), down(0), left(0), marked(false) {};
};

/* Reprezentarea grafului. Am ales sa tin graful intr-o matrice 
 * identica cu cea citita din fisier(nu matrice de adiacenta). Vecinii
 * pot fi aflati prin calcule simple. De exemplu vecinul drept al unui 
 * element din matrice [i, j] este [i, j+1]. Capacitatile catre vecini 
 * sunt tinute in structura Node. Capacitatile de la sursa la fiecare 
 * nod din graf sunt tinute tot intr-o matrice. Pentru elementul [i, j]
 * din capacitatea de la sursa este elementul cu aceeasi pozitie din 
 * sourceCapacity. Capacitatile de la drena la noduri sunt stocate in 
 * mod similar. */
struct Graph{

private:
	short width;
	short height;
	short maxval;
	bool allocated;
public:
	Node** matrix;
	double **sourceCapacity;
	double **drainCapacity;

	Graph() : width(0), height(0) { } ;

	friend std::istream& operator>>(std::istream& in, Graph& graph);

	short getHeight() { return height; }
	
	short getWidth()  { return width; }

	short getMaxval() { return maxval; }

	void toString(){
		for (short i = 0; i < height; i++){ 
			for (short j = 0; j < width; j++){
				std::cout << matrix[i][j].value << " ";
			}
			std::cout << std::endl;
		}
	}

	void sourceToString(std::ofstream& out){
		for(short i = 0; i < height; i++){
			for (short j = 0; j < width; j++){
				out << sourceCapacity[i][j] << " ";
			}
			out << std::endl;
		}
	}

	void drainToString(std::ofstream& out){
		for(short i = 0; i < height; i++){
			for (short j = 0; j < width; j++){
				out << drainCapacity[i][j] << " ";
			}
			out << std::endl;
		}
	}

	void completeGraph(std::ofstream& out){
		for(short i = 0; i < height; i++){
			for (short j = 0; j < width; j ++){
				out << "Node (" << i << ", " << j << ")" << std::endl;
				out << "source, c: " << sourceCapacity[i][j] << std::endl;
				out << "drain, c: " << drainCapacity[i][j] << std::endl;
				out << "up, c: " << matrix[i][j].up << std::endl;
				out << "right, c: " << matrix[i][j].right << std::endl;
				out << "down, c: " << matrix[i][j].down << std::endl;
				out << "left, c: " << matrix[i][j].left << std::endl;
				out << std::endl;
			}
			
		}
	}
	
	/* Seteaza capacitatile dintre toate nodurile grafului */
	void setCapacity(short lambda, short threshold);

	/* Seteaza capacitatile dintre sursa si fiecare nod si 
	 * capacitatile dintre drena si fiecare sursa */
	void sourceAndDrainCapacity(double niuF, double sigmaF, 
								double niuB, double sigmaB);

	/* Returneaza un vector cu vecinii nodului a carui pozitie 
	 * in matrice este data de cei doi parametri i si j. Sunt
	 * returnati doar vecini care au capacitati != 0 catre ei.*/
	std::list<Position> getNeighbours(short i, short j);

	/* Metoda supraincarcata pentru apelul cu Position */
	std::list<Position> getNeighbours(Position pos);

	/* Returneaza capacitatea corespunzatoare muchiei ce uneste 
	 * cele doua noduri. Parametrul saturate indica daca se doreste
	 * saturarea cu o anumuta valoare(value) a muchiei respective.*/
	double getEdgeCapacity(Position pos1, Position pos2, 
								  bool saturate, double value);

	~Graph() {  
		for (short i = 0; i < height; i++){ 
			delete[] matrix[i];
			delete[] sourceCapacity[i];
			delete[] drainCapacity[i];
		}
		delete[] matrix;
		delete[] sourceCapacity;
		delete[] drainCapacity;
	}
};

/* Retine intr-o lista de perechi nodurile ce fac parte din 
 * masca. Perechea contine indicii din matricea ce reprezinta 
 * graful */
struct Mask {
private:
	short width, height, maxval;
public:
	std::list<std::pair<short, short> > nodes;

	Mask () : width(0), height(0) {};

	int getHeight() { return height; }
	
	int getWidth()	{ return width; }


	friend std::istream& operator>>(std::istream& in, Mask& mask);

};

std::istream& operator>>(std::istream& in, Graph& graph);
std::istream& operator>>(std::istream& in, Mask& mask);

#endif
