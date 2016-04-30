#include <iostream>
#include <queue>
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
ostream& operator << (ostream& s, AbstractArrayClass<DataType>& ac) {
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
	_currSize = 0;
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
	int findIndexForValue(const int& value, const int& startingFrom);					// Finds the index for the given value, starting from a particular index in the tree
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
};
template <class DT>
ParentMultiTree<DT>::ParentMultiTree() {
	_parentArray = new DT();
	_childOrderArray = new DT();
	_numNodes = 0;
}
template <class DT>
ParentMultiTree<DT>::ParentMultiTree(int size) {
	_parentArray = new DT(size, 0);
	_childOrderArray = new DT(size, 0);
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
	return findIndexForValue(-1, 0);
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
	DT* temp = new DT(arr.size());
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
int ParentMultiTree<DT>::findIndexForValue(const int& value, const int& startingFrom) {
	for (int i = startingFrom; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == value) {
			return i;
		}
	}
	return -1;
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
		if (children[i] > _numNodes) {
			throw TreeMemory();
		}
		else {
			(*_parentArray)[children[i]] = parent;
			(*_childOrderArray)[children[i]] = i;
		}
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
}
#pragma endregion Methods

int main() {
	int totalNumberOfNodes;
	int numNodes;
	int parent;

	// Reads in the number of nodes and constructs a new multi tree
	cin >> totalNumberOfNodes;
	ParentMultiTree<ArrayClass<int>>* parentMultiTree = new ParentMultiTree<ArrayClass<int>>(totalNumberOfNodes);

	while (cin >> parent) {
		cin >> numNodes;
		ArrayClass<int> nodes(numNodes);
		for (int i = 0; i < numNodes; ++i) {
			int temp;
			cin >> temp;
			nodes[i] = temp;
		}
		if (numNodes > 0) {
			(*parentMultiTree).insertToTree(parent, nodes);
		}
	}
	cout << "Original Multi Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*parentMultiTree) << endl;
	cout << "Size of tree: " << (*parentMultiTree).getTreeChildrenSize() << endl;
	cout << "Height of tree: " << (*parentMultiTree).getTreeChildrenHeight() << endl;

	// EXTRA CREDIT
	cout << "Printing level by level: ";
	(*parentMultiTree).printLevelByLevel();
	cout << endl;

	ParentMultiTree<ArrayClass<int>>* testCopyMultiTree = new ParentMultiTree<ArrayClass<int>>(*parentMultiTree);
	cout << endl << "Copied Multi Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*testCopyMultiTree) << endl;
	cout << "Size of tree: " << (*testCopyMultiTree).getTreeChildrenSize() << endl;
	cout << "Height of tree: " << (*testCopyMultiTree).getTreeChildrenHeight() << endl;
	cout << "Preorder tree traversal: "; (*testCopyMultiTree).preOrderTreeTraversal();
	cout << endl;

	ParentMultiTree<ArrayClass<int>>* testMultiTree = new ParentMultiTree<ArrayClass<int>>(26);
	ArrayClass<int> a(4); a[0] = 2; a[1] = 9; a[2] = 14; a[3] = 13; (*testMultiTree).insertToTree(25, a);
	ArrayClass<int> b(2); b[0] = 4; b[1] = 1; (*testMultiTree).insertToTree(2, b);
	ArrayClass<int> c(1); c[0] = 8; (*testMultiTree).insertToTree(4, c);
	ArrayClass<int> d(1); d[0] = 10; (*testMultiTree).insertToTree(8, d);
	ArrayClass<int> e(1); e[0] = 3; (*testMultiTree).insertToTree(10, e);
	ArrayClass<int> f(2); f[0] = 11; f[1] = 7; (*testMultiTree).insertToTree(1, f);
	ArrayClass<int> g(3); g[0] = 18; g[1] = 17; g[2] = 19; (*testMultiTree).insertToTree(11, g);
	ArrayClass<int> h(1); h[0] = 20; (*testMultiTree).insertToTree(18, h);
	ArrayClass<int> i(1); i[0] = 0; (*testMultiTree).insertToTree(17, i);
	ArrayClass<int> j(2); j[0] = 5; j[1] = 6; (*testMultiTree).insertToTree(9, j);
	ArrayClass<int> k(1); k[0] = 21; (*testMultiTree).insertToTree(14, k);
	ArrayClass<int> l(1); l[0] = 24; (*testMultiTree).insertToTree(21, l);
	ArrayClass<int> m(5); m[0] = 12; m[1] = 15; m[2] = 16; m[3] = 23; m[4] = 22; (*testMultiTree).insertToTree(13, m);
	testCopyMultiTree = testMultiTree;
	cout << endl << "Copied Test Multi Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*testCopyMultiTree) << endl;
	cout << "Size of tree: " << (*testCopyMultiTree).getTreeChildrenSize() << endl;
	cout << "Height of tree: " << (*testCopyMultiTree).getTreeChildrenHeight() << endl;

	delete parentMultiTree;
	delete testCopyMultiTree;

	// testMultiTree already was deleted when assigned to testCopyMultiTree
	cout << endl << "Original after deleting: " << (*parentMultiTree) << endl;
	cout << "Copied Multi Tree after deleting: " << (*testCopyMultiTree) << endl;
	cout << "Copied Test Multi Tree after deleting: " << (*testCopyMultiTree) << endl;


	return 0;
}