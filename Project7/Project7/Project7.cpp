#include <iostream>
#include <queue>
#include <list>
using namespace std;

#pragma region Exceptions
class Exception { };																	// Generic, all exceptions derive from this
class TreeException : public Exception {};												// Generic Tree Exception
class TreeMemory : public TreeException {};												// In case the Tree has transferred incorrect information from the datatype
class ArrayException : public Exception { };											// Generic array exception, all array exceptions derive from
class ArrayMemoryException : public ArrayException { };									// In case the array created causes an error
class ArrayBoundsException : public ArrayException { };									// In case the user asked for an index that was out of bounds
#pragma endregion

#pragma region array
																						// Purely virtual data type from which arrays derive
template <class DataType>
class AbstractArrayClass {
public:
	virtual int size() const = 0;														// Abstract template for size
	virtual DataType& operator[] (int k) = 0;											// Abstract template for array index overloading
};

template <class DataType>
class ArrayClass : virtual public AbstractArrayClass<DataType> {						// Encapsulation of a DataType array
protected:
	DataType *paObject;																	// Pointer to an array of DataType
	int _size;																			// Capacity of the array
	void copy(const ArrayClass<DataType>& ac);											// Allows for a copy constructor to take data from an external ArrayClass
public:
	ArrayClass();																		// Default constructor, creates a array of size 1
	ArrayClass(int n);																	// Initializer, creates an array of size n
	ArrayClass(int n, const DataType& val);												// Initializer, fills an array of size n with val
	ArrayClass(const ArrayClass<DataType>& ac);											// Copy constructor, transfers data from an external ArrayClass, and copies data into self
	virtual ~ArrayClass();																// Destructor
	virtual int size() const;															// Encapsulated method to access capacity
	virtual DataType& operator [] (int k);												// Overloads bracket operator to access data at index of k
	void operator=(const ArrayClass<DataType>& ac);										// Overloads equals operator to copy information from the given ArrayClass
};

// Purely virtual vector template
template <class DataType>
class AbstractVector : virtual public AbstractArrayClass<DataType> {
public:
	virtual void insert(const DataType& item, int index) = 0; 							// Insert a new object at position index in the vector
	virtual void remove(int index) = 0; 												// Removes the object at position index of the vector
	virtual void add(const DataType& item) = 0; 										// Adds item at end of the vector
};

// Encapsulation of a DataType Vector, allows for dynamically sized array
template <class DataType>
class Vector : virtual public ArrayClass<DataType>, virtual public AbstractVector<DataType> {
protected:
	int _currSize;																		// Active size of the array	
	int _incFactor;																		// Index at which the size of the array will be doubled
public:
	Vector();																			// Default constructor, calls underlying ArrayClass' default constructor, and sets current size to 0
	Vector(int n);																		// Initializer, calls underlying ArrayClass' initializer, and sets increment factor to default
	Vector(int n, DataType& val);														// Initializer, calls underlying ArrayClass' initializer, and sets increment factor to default
	Vector(const Vector<DataType>& v);													// Copy constructor, transfers data from an external Vector, and copies data into self
	Vector(const ArrayClass<DataType>& ac);												// Copy constructor, transfers data from an external ArrayClass, and copies data into underlying Array
	virtual ~Vector();																	// Destructor
	void operator= (const Vector<DataType>& v);											// Overloads equals operator to copy information from the given Vector
	void operator= (const ArrayClass<DataType>& ac);									// Overloads equals operator to copy information from the given ArrayClass
	bool operator== (Vector<DataType>& v);												// Overloads equality operator to check the contents of the two vectors, MUST BE EQUAL CASE
	virtual void insert(const DataType& item, int index);								// Shifts all items up to the current array from the given index, and inserts item into given index
	virtual void remove(int index);														// Removes item at given index, shifts down other objects, and decrements active array size
	virtual void add(const DataType& item);												// Appends an object to the underlying Array
	virtual int size() const; 															// Returns size of underlying array
	virtual int capacity() const;														// Returns capacity of underlying array
	virtual int incFactor() const;														// Returns current increment factor
	virtual void setIncFactor(int f);													// Resets the incedent factor to necessary size
	void setCapacity(int c);															// Resizes underlying array to the specified capacit
	bool contains(const DataType& i);													// Checks whether the vector contains a member of the specified dataType
};
#pragma region array
template <class DataType>
ArrayClass<DataType>::ArrayClass() {
	_size = 0;
	paObject = new DataType[1];
	if (paObject == NULL) { throw ArrayMemoryException(); }
	_size = 1;
}
template <class DataType>
ArrayClass<DataType>::ArrayClass(int n) {
	_size = 0; // Default in case allocation fails
	paObject = new DataType[n];
	if (paObject == NULL) { throw ArrayMemoryException(); }
	_size = n;
}
template <class DataType>
ArrayClass<DataType>::ArrayClass(int n, const DataType& val) {
	_size = 0; // Default in case allocation fails
	paObject = new DataType[n];
	if (paObject == NULL) { throw ArrayMemoryException(); }
	_size = n;
	for (int i = 0; i < n; ++i) {
		paObject[i] = val;
	}
}
template <class DataType>
ArrayClass<DataType>::ArrayClass(const ArrayClass<DataType>& ac) {
	copy(ac);
}
template <class DataType>
ArrayClass<DataType>::~ArrayClass() {
	if (paObject != NULL) { delete[] paObject; }
	paObject = NULL;
}
template <class DataType>
void ArrayClass<DataType>::copy(const ArrayClass<DataType>& ac) {
	_size = 0; // Default in case allocation fails
	paObject = new DataType[ac._size];
	if (paObject == NULL) { throw ArrayMemoryException(); }
	_size = ac._size;
	for (int i = 0; i < _size; ++i) {
		paObject[i] = ac.paObject[i];
	}
}
template <class DataType>
int ArrayClass<DataType>::size() const {
	return _size;
}
template <class DataType>
DataType& ArrayClass<DataType>::operator[] (int k) {
	if ((k < 0) || (k >= size())) { throw ArrayBoundsException(); }
	return paObject[k];
}
template <class DataType>
void ArrayClass<DataType>::operator=(const ArrayClass<DataType>& ac) {
	if (paObject != NULL) { delete[] paObject; } // Existing array is deleted and copied in to the new array
	copy(ac);
}
template <class DataType>
ostream& operator<< (ostream& s, AbstractArrayClass<DataType>& ac) {
	for (int i = 0; i < ac.size(); ++i) {
		s << ac[i];
	}
	return s;
}
#pragma endregion Methods

#pragma region Vector
template <class DataType>
Vector<DataType>::Vector() : ArrayClass<DataType>() {
	_currSize = 0; // Default values
	_incFactor = 5;
}
template <class DataType>
Vector<DataType>::Vector(int n) : ArrayClass<DataType>(n) {
	_currSize = 0;
	_incFactor = (n + 1) / 2; // Arbitrary
}
template <class DataType>
Vector<DataType>::Vector(int n, DataType& val) : ArrayClass<DataType>(n, val) {
	_currSize = n;
	_incFactor = n / 2; // Arbitrary
}
template <class DataType>
Vector<DataType>::Vector(const Vector<DataType>& v) : ArrayClass<DataType>(v) {
	_currSize = v._currSize;
	_incFactor = v.incFactor();
}
template <class DataType>
Vector<DataType>::Vector(const ArrayClass<DataType>& ac) : ArrayClass<DataType>(ac) {
	_currSize = ac.size();
	_incFactor = (_currSize + 1) / 2;
}
template <class DataType>
Vector<DataType>::~Vector() {
	_currSize = 0;
	setIncFactor(5);
}
template <class DataType>
void Vector<DataType>::operator= (const Vector<DataType>& v) {
	ArrayClass<DataType>::copy(v);
	_currSize = v._currSize;
	_incFactor = v.incFactor();
}
template <class DataType>
void Vector<DataType>::operator= (const ArrayClass<DataType>& ac) {
	ArrayClass<DataType>::copy(ac);
	_currSize = ac.size();
	_incFactor = (_currSize + 1) / 2;
}
template <class DataType>
bool Vector<DataType>::operator== (Vector<DataType>& v) {
	if (this->paObject == NULL || v.paObject == NULL || this->_currSize != v._currSize) {
		return false;
	}
	for (int i = 0; i < this->_currSize; ++i) {
		if (paObject[i] != v[i]) {
			return false;
		}
	}

	return true;
}
template <class DataType>
int Vector<DataType>::size() const {
	return _currSize;
}
template <class DataType>
int Vector<DataType>::capacity() const {
	return _size;
}
template <class DataType>
int Vector<DataType>::incFactor() const {
	return _incFactor;
}
template <class DataType>
void Vector<DataType>::setIncFactor(int f) {
	if (f >= 0) { _incFactor = f; }
}
template <class DataType>
void Vector<DataType>::setCapacity(int c) {
	int len = _currSize;
	if (len > c) { len = c; }
	DataType* paNew = new DataType[c];
	if (paNew == NULL) { throw ArrayMemoryException(); }
	for (int i = 0; i < len; ++i) {
		paNew[i] = paObject[i];
	}
	if (paObject != NULL) {
		delete[] paObject;
	}
	paObject = paNew;
	_size = c;
	if (_currSize > len) {
		_currSize = len;
	}
}
template <class DataType>
void Vector<DataType>::insert(const DataType& item, int index) {
	if ((index < 0) || (index > _currSize)) {
		throw ArrayBoundsException();
	}
	if (_currSize + 1 == _size) {
		setCapacity(_size + _incFactor);
	}
	++_currSize;
	for (int i = _currSize - 1; i > index; --i) {
		(*this)[i] = (*this)[i - 1];
	}
	(*this)[index] = item;
}
template <class DataType>
void Vector<DataType>::add(const DataType& item) {
	insert(item, _currSize);
}
template <class DataType>
void Vector<DataType>::remove(int index) {
	if ((index < 0) || (index >= _currSize)) {
		throw ArrayBoundsException();
	}
	if (_currSize <= _size - _incFactor) {
		setCapacity(_size - _incFactor);
	}
	for (int i = index; i < _currSize - 1; ++i) {
		(*this)[i] = (*this)[i + 1];
	}
	--_currSize;
}
template <class DataType>
bool Vector<DataType>::contains(const DataType& var) {
	for (int i = 0; i < _currSize; ++i) {
		if (paObject[i] == var) {
			return true;
		}
	}
	return false;
}
#pragma endregion Methods

#pragma endregion Classes

#pragma region ParentMultiTree
template <class DT>
class ParentMultiTree {
protected:
	DT* _parentArray;																	// Array containing the indexes of the parent
	DT* _childOrderArray;																// Array containing the order of the children
	int _numNodes;																		// Size of both arrays
	void copy(const ParentMultiTree<DT>& multiTree);									// Copy constructor
	int max(int firstNumber, int secondNumber);											// Finds the maximum value between two integers
public:
	ParentMultiTree();																	// Empty constructor, initializes empty parentArray, childOrderArray, and sets numNodes to 0
	ParentMultiTree(int size);															// Initializer, creates a parentArray, and childOrderArray with 'size' number of zeroes, and sets numNodes to size
	~ParentMultiTree();																	// Destructor, deletes the parentArray, childOrderArray, and sets numNodes to NULL
	ParentMultiTree(const ParentMultiTree<DT>& multiTree);								// Copy constructor, constructs a tree with the supplied tree
	void operator= (const ParentMultiTree<DT>& multiTree);								// Overloaded equals to operator, creates an equivalent tree with the supplied tree
	void display(ostream& s, const int& parentValue);									// Recursively calls itself to show preorder traversal of the supplied tree
	int indexOf(const int& value);														// Finds the first index for the given value
	int findHighestChildOrder(const int& value);										// Finds the last possible index for the given value
	int nodeChildrenSize(const int& parentValue);										// Recursively calls itself to find the size of the specific tree given the parentNode value
	int nodeChildrenHeight(const int& parentValue);										// Recursively calls itself to find the height of the specific tree given the parentNode value
	int getTreeChildrenHeight();														// Returns the height of the entire tree															
	int getTreeChildrenSize();															// Returns the size of the entire tree
	int getRoot();																		// Returns the parentNode of the entire tree
	Vector<int>& getChildren(const int& parentValue);									// Returns all of the children of the supplied parent node
	Vector<int>& getChildrenOrder(const int& parentValue);								// Returns the order the children should be in
	Vector<int> reorder(Vector<int>& arr, Vector<int>& index);							// Reorders the children based on the given child order
	void preOrderTreeTraversal();														// Traverses the entire tree, preOrder
	void preOrderChildrenTraversal(const int& parentValue);								// Traverses the tree in a recursive fashion: root, the left sub tree through the right subtree
	void insertToTree(const int& parent, DT& children);									// Inserts nodes to the tree, starting from the parent, given the parentNode, from the left to the right and fills the childOrderArray
	void printLevelByLevel();															// Prints level by level using a STL Queue
	void insertSingleNode(const int& parent, const int& child);							// Inserts a single node, with the child order after the last child
	void debug();																		// Prints out the tree's underlying arrays
	void clear();																		// Clears out the underlying arrays
};
template <class DT>
ParentMultiTree<DT>::ParentMultiTree() {
	_parentArray = new DT();
	_childOrderArray = new DT();
	_numNodes = 0;
}
template <class DT>
ParentMultiTree<DT>::ParentMultiTree(int size) {
	_parentArray = new DT(size, -2);
	_childOrderArray = new DT(size, -2);
	_numNodes = size;
	if (_parentArray == NULL || _childOrderArray == NULL) {
		throw TreeMemory();
	}
}
template <class DT>
ParentMultiTree<DT>::~ParentMultiTree() {
	delete _parentArray;
	_parentArray = NULL;
	delete _childOrderArray;
	_childOrderArray = NULL;
	_numNodes = NULL;
}
template <class DT>
ParentMultiTree<DT>::ParentMultiTree(const ParentMultiTree<DT>& multiTree) {
	copy(multiTree);
}
template<class DT>
void ParentMultiTree<DT>::operator=(const ParentMultiTree<DT>& multiTree) {
	copy(multiTree);
}
template <class DT>
void ParentMultiTree<DT>::copy(const ParentMultiTree<DT>& multiTree) {
	_parentArray = multiTree._parentArray;
	_childOrderArray = multiTree._childOrderArray;
	_numNodes = multiTree._numNodes;
	if (_parentArray == NULL || _childOrderArray == NULL) {
		throw TreeMemory();
	}
}
template<class DT>
ostream & operator<<(ostream& s, ParentMultiTree<DT>& multiTree)
{
	multiTree.display(s, multiTree.getRoot());
	return s;
}
template<class DT>
void ParentMultiTree<DT>::display(ostream& s, const int& parentValue)
{
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	s << parentValue << " ";
	if (children.size() == 0) {
		return;
	}
	for (int i = 0; i < children.size(); ++i) {
		display(s, children[i]);
	}
}
template <class DT>
void ParentMultiTree<DT>::debug() {
	cout << "Printing underlying tree array: " << endl;
	for (int i = 0; i < _numNodes; ++i) {
		cout << (*_parentArray)[i] << endl;
	}
	cout << "Child order: " << endl;
	for (int i = 0; i < _numNodes; ++i) {
		cout << (*_childOrderArray)[i] << endl;
	}
}
template <class DT>
void ParentMultiTree<DT>::clear() {
	for (int i = 0; i < _numNodes; ++i) {
		(*_parentArray)[i] = -2;
	}
}
template<class DT>
int ParentMultiTree<DT>::max(int firstNumber, int secondNumber) {
	return firstNumber >= secondNumber ? firstNumber : secondNumber;
}
template<class DT>
int ParentMultiTree<DT>::getTreeChildrenHeight() {
	return nodeChildrenHeight(getRoot());
}
template<class DT>
int ParentMultiTree<DT>::getTreeChildrenSize() {
	return nodeChildrenSize(getRoot());
}
template <class DT>
int ParentMultiTree<DT>::nodeChildrenSize(const int& parentValue) {
	int size = 1;
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	for (int i = 0; i < children.size(); ++i) {
		int nodeSize = 1;
		if (getChildren(children[i]).size() != 0) {
			nodeSize = nodeChildrenSize(children[i]);
		}
		size += nodeSize;
	}

	return size;
}
template <class DT>
int ParentMultiTree<DT>::nodeChildrenHeight(const int& parentValue) {
	int nodeHeight = 1;
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	for (int i = 0; i < children.size(); ++i) {
		if (getChildren(children[i]).size() != 0) {
			nodeHeight = max(nodeHeight, nodeChildrenHeight(children[i]));
		}
	}

	return 1 + nodeHeight;
}
template<class DT>
int ParentMultiTree<DT>::getRoot()
{
	return indexOf(-1);
}
template <class DT>
Vector<int>& ParentMultiTree<DT>::getChildren(const int& parentValue) {
	Vector<int>* children = new Vector<int>();
	for (int i = 0; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == parentValue) {
			(*children).add(i);
		}
	}

	return (*children);
}
template <class DT>
Vector<int>& ParentMultiTree<DT>::getChildrenOrder(const int& parentValue) {
	Vector<int>* childOrder = new Vector<int>();
	for (int i = 0; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == parentValue) {
			(*childOrder).add((*_childOrderArray)[i]);
		}
	}

	return (*childOrder);
}
template <class DT>
Vector<int> ParentMultiTree<DT>::reorder(Vector<int>& arr, Vector<int>& index) {
	DT* temp = new DT(arr.size(), 0);
	for (int i = 0; i < arr.size(); ++i) {
		(*temp)[index[i]] = arr[i];
	}
	for (int i = 0; i < arr.size(); ++i) {
		arr[i] = (*temp)[i];
	}

	return arr;
}
template <class DT>
void ParentMultiTree<DT>::preOrderTreeTraversal() {
	preOrderChildrenTraversal(getRoot());
}
template <class DT>
void ParentMultiTree<DT>::preOrderChildrenTraversal(const int& parentValue) {
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	cout << parentValue << " ";
	if (children.size() == 0) {
		return;
	}
	for (int i = 0; i < children.size(); ++i) {
		preOrderChildrenTraversal(children[i]);
	}
}
template<class DT>
int ParentMultiTree<DT>::indexOf(const int& value) {
	for (int i = 0; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == value) {
			return i;
		}
	}
	return -1;
}
template<class DT>
int ParentMultiTree<DT>::findHighestChildOrder(const int& value) {
	int maxChild = -1;
	for (int i = 0; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == value) {
			maxChild = max(maxChild, (*_childOrderArray)[i]);
		}
	}
	return maxChild;
}
template <class DT>
void ParentMultiTree<DT>::insertToTree(const int& parent, DT& children) {
	// Only happens first time
	if (getRoot() == -1) {
		if (parent >= _numNodes) {
			throw TreeMemory();
		}
		else {
			(*_parentArray)[parent] = -1;
			(*_childOrderArray)[parent] = -1;
		}
	}
	for (int i = 0; i < children.size(); ++i) {
		if (children[i] >= _numNodes) {
			throw TreeMemory();
		}
		else {
			(*_parentArray)[children[i]] = parent;
			(*_childOrderArray)[children[i]] = i;
		}
	}
}
template <class DT>
void ParentMultiTree<DT>::insertSingleNode(const int& parent, const int& child) {
	// Only happens first time
	if (getRoot() == -1) {
		if (parent >= _numNodes) {
			throw TreeMemory();
		}
		else {
			(*_parentArray)[parent] = -1;
			(*_childOrderArray)[parent] = -1;
		}
	}
	if (child >= _numNodes) {
		throw TreeMemory();
	}
	else {
		int lastOrder = findHighestChildOrder(parent);
		(*_childOrderArray)[child] = lastOrder + 1;
		(*_parentArray)[child] = parent;
	}
}
template <class DT>
void ParentMultiTree<DT>::printLevelByLevel() {
	int level = 0;
	int temp;
	int element;
	queue<int> myQueue;
	queue<int> levelQueue;

	// Priming
	myQueue.push(getRoot());
	levelQueue.push(1);
	while (!myQueue.empty()) {
		element = myQueue.front();
		myQueue.pop();
		temp = levelQueue.front();
		levelQueue.pop();
		if (level != temp) {
			cout << endl;
			level = temp;
			cout << "Level " << level << ": ";
		}

		Vector<int> children = reorder(getChildren(element), getChildrenOrder(element));
		for (int i = 0; i < children.size(); ++i) {
			myQueue.push(children[i]);
			levelQueue.push(level + 1);
		}

		cout << element << " ";
	}
	cout << endl;
}
#pragma endregion Methods

#pragma region GraphAdjList
template <class DT>
class GraphAdjList {
	template <class DT>
	friend ostream& operator<< (ostream& s, GraphAdjList<DT>& graph);					// Overloaded operator
private:
	void _setupForSearch();																// Clears out arrays preparing for search
	void _dfs(int x);																	// Allows a recursive depth first search on 'x'
protected:
	ArrayClass<list<DT>*>* _adjList;													// Adjacency list, an array of lists which contain each indexes neighbors
	ArrayClass<bool>* _visited;															// An array of visited nodes
	ParentMultiTree<ArrayClass<DT>>* _parentTree;										// Parent Array representation of the finished dfs and bfs traversals
	int _numNodes;																		// Number of nodes
	int _numEdges;																		// Number of edges
public:
	GraphAdjList(int numNodes, int numEdges);											// Initializer constructor, with numNodes and numEdges as input
	~GraphAdjList();																	// Destructor, removes any unused memory
	void addEdge(int x, int y);															// Adds a vertex between vertex 'x' and vertex 'y'
	ParentMultiTree<ArrayClass<DT>>* dfs(int x);										// Depth First Search, where 'x' is the starting node and its output is a parent array which represents the depth first search tree
	ParentMultiTree<ArrayClass<DT>>* bfs(int x);										// Breadth First Search, where 'x' is the starting node and its output is a parent array which represents the breadth first search tree
};
template <class DT>
GraphAdjList<DT>::GraphAdjList(int numNodes, int numEdges) {
	_numNodes = numNodes;
	_numEdges = numEdges;
	_adjList = new ArrayClass<list<DT>*>(numNodes, NULL);
	_visited = new ArrayClass<bool>(numNodes, false);
	_parentTree = new ParentMultiTree<ArrayClass<DT>>(numNodes);
}
template <class DT>
GraphAdjList<DT>::~GraphAdjList() {
	_numNodes = NULL;
	_numEdges = NULL;
	delete _adjList;
	delete _visited;
	delete _parentTree;
}
template <class DT>
void GraphAdjList<DT>::addEdge(int x, int y) {
	// Creates new list at specified index if not already a list
	if ((*_adjList)[x] == NULL) {
		(*_adjList)[x] = new list<DT>();
	}
	if ((*_adjList)[y] == NULL) {
		(*_adjList)[y] = new list<DT>();
	}
	list<int>* firstList = (*_adjList)[x];
	list<int>* secondList = (*_adjList)[y];
	(*firstList).push_back(y);
	(*secondList).push_back(x);
}
template <class DT>
ParentMultiTree<ArrayClass<DT>>* GraphAdjList<DT>::dfs(int x) {
	_setupForSearch();
	_dfs(x);
	return	_parentTree;
}
template <class DT>
void GraphAdjList<DT>::_dfs(int x) {
	(*_visited)[x] = true;
	// Find unvisited adjacent nodes, then call DFS recursively immediately
	list<int>* dfsList = (*_adjList)[x];
	list<int>::iterator t = (*dfsList).begin();
	while (t != (*dfsList).end()) {
		if (!(*_visited)[*t]) {
			(*_parentTree).insertSingleNode(x, (*t));
			_dfs(*t);
		}
		++t;
	}
}
template <class DT>
ParentMultiTree<ArrayClass<DT>>* GraphAdjList<DT>::bfs(int x) {
	_setupForSearch();
	(*_visited)[x] = true;
	queue<int> bfsQueue;
	bfsQueue.push(x);
	while (!bfsQueue.empty()) {
		int x = bfsQueue.front();
		bfsQueue.pop();
		list<int>* bfsList = (*_adjList)[x];
		list<int>::iterator t = (*bfsList).begin();
		while (t != (*bfsList).end()) {
			if (!(*_visited)[*t]) {
				(*_visited)[*t] = true;
				bfsQueue.push(*t);
				(*_parentTree).insertSingleNode(x, (*t));
			}
			++t;
		}
	}
	return _parentTree;
}
template <class DT>
ostream & operator<< (ostream & s, GraphAdjList<DT>& graph) {
	for (int i = 0; i < graph._numNodes; ++i) {
		s << "Nodes related to " << i << ": ";
		list<int>* indList = (*graph._adjList)[i];
		if (indList != NULL) {
			list<int>::iterator t = (*indList).begin();
			while (t != (*indList).end()) {
				s << *t << " ";
				++t;
			}
		}
		if (i != graph._numNodes - 1) {
			s << endl;
		}
	}
	return s;
}
template <class DT>
void GraphAdjList<DT>::_setupForSearch() {
	(*_parentTree).clear();
	for (int i = 0; i < _numNodes; ++i) {
		(*_visited)[i] = false;
	}
}
#pragma endregion Methods

int main() {
	int numNodes, numEdges;
	int searchNode = 0; // Node that the search/traversal acts on
	cin >> numNodes >> numEdges;
	GraphAdjList<int> *myGraph = new GraphAdjList<int>(numNodes, numEdges);
	for (int i = 0; i < numEdges; ++i) {
		int x, y;
		cin >> x >> y;
		(*myGraph).addEdge(x, y);
	}
	cout << "Overloaded Operator - Graph:" << endl;
	cout << (*myGraph) << endl;

	cout << endl << "Depth First Search on " << searchNode << ":" << endl << "Parent Multi Tree (Project6)" << endl;
	ParentMultiTree<ArrayClass<int>>* dfsTest = (*myGraph).dfs(searchNode);
	cout << (*dfsTest) << endl;
	(*dfsTest).printLevelByLevel();

	cout << endl << "Breadth First Search on " << searchNode << ":" << endl << "Parent Multi Tree (Project 6)" << endl;
	ParentMultiTree<ArrayClass<int>>* bfsTest = (*myGraph).bfs(searchNode);
	cout << (*bfsTest) << endl;
	(*bfsTest).printLevelByLevel();

	return 0;
}
