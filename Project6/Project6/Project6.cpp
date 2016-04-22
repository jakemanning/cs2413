#include <iostream>
using namespace std;

#pragma region Exceptions
class Exception { };																	// Generic, all exceptions derive from this
class BinaryTreeException : public Exception {};										// Generic BinaryTree Exception
class BinaryTreeMemory : public BinaryTreeException {};									// In case the binary tree has transferred incorrect information from the datatype
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

#pragma region ParentBinaryTree
template <class DT>
class ParentMultiTree {
protected:
	DT* _parentArray;																	// Array containing the indexes of the parent
	DT* _childOrderArray;																// Array containing the order of the children
	int _numNodes;																		// Size of both arrays
	void copy(const ParentMultiTree<DT>& binaryTree);									// Copy constructor
	int max(int firstNumber, int secondNumber);											// Finds the maximum value between two integers
public:
	ParentMultiTree();																	// Empty constructor, initializes empty parentArray, childOrderArray, and sets numNodes to 0
	ParentMultiTree(int size);															// Initializer, creates a parentArray, and childOrderArray with 'size' number of zeroes, and sets numNodes to size
	~ParentMultiTree();																	// Destructor, deletes the parentArray, childOrderArray, and sets numNodes to NULL
	ParentMultiTree(const ParentMultiTree<DT>& binaryTree);								// Copy constructor, constructs a tree with the supplied tree
	void operator= (const ParentMultiTree<DT>& binaryTree);								// Overloaded equals to operator, creates an equivalent tree with the supplied tree
	void display(ostream& s, const int& parentValue);									// Recursively calls itself to show preorder traversal of the supplied tree
	int findIndexForValue(const int& value, const int& startingFrom);					// Finds the index for the given value, starting from a particular index in the tree
	int nodeSize(const int& parentValue);												// Recursively calls itself to find the size of the specific tree given the parentNode value
	int nodeHeight(const int& parentValue);												// Recursively calls itself to find the height of the specific tree given the parentNode value
	int getTreeHeight();																// Returns the height of the entire tree
	int getTreeSize();																	// Returns the size of the entire tree
	int getRoot();																		// Returns the parentNode of the entire tree
	int getLeft(const int& parentValue);												// Returns the left node of the given parentValue, -1 if there is no left node
	int getRight(const int& parentValue);												// Returns the right node of the given parentValue, -1 if there is no right node
	Vector<int>& getChildren(const int& parentValue);
	Vector<int>& getChildrenOrder(const int& parentValue);
	Vector<int> reorder(Vector<int> arr, Vector<int> index);
	void preOrderTreeTraversal();														// Traverses the entire tree, preOrder
	void preOrderTraversal(const int& parentValue);										// Prints in a recursive fashion: Root, Left, Right
	void preOrderChildrenTraversal(const int& parentValue);
	void insertToTree(const int& parent, const int& left, const int& right);			// Inserts nodes to the tree, starting from the parent, given the parentNode, the left and the right (-1 if no left/right)
	void insertToTree(const int& parent, const DT& children);

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
		throw BinaryTreeMemory();
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
ParentMultiTree<DT>::ParentMultiTree(const ParentMultiTree<DT>& binaryTree) {
	copy(binaryTree);
}
template<class DT>
void ParentMultiTree<DT>::operator=(const ParentMultiTree<DT>& binaryTree) {
	copy(binaryTree);
}
template <class DT>
void ParentMultiTree<DT>::copy(const ParentMultiTree<DT>& binaryTree) {
	_parentArray = binaryTree._parentArray;
	_childOrderArray = binaryTree._childOrderArray;
	_numNodes = binaryTree._numNodes;
	if (_parentArray == NULL || _childOrderArray == NULL) {
		throw BinaryTreeMemory();
	}
}
template<class DT>
ostream & operator<<(ostream& s, ParentMultiTree<DT>& binaryTree)
{
	binaryTree.display(s, binaryTree.getRoot());
	return s;
}
template<class DT>
void ParentMultiTree<DT>::display(ostream& s, const int& parentValue)
{
	if (parentValue == -1 && getLeft(parentValue) == -1 && getRight(parentValue) == -1) {
		return;
	}
	s << parentValue << " ";
	display(s, getLeft(parentValue));
	display(s, getRight(parentValue));
}
template<class DT>
void display(ostream& s, const int& parentValue) {
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	if (children.size() == 0) {
		return;
	}
	s << parentValue << " ";
	for (int i = 0; i < children.size(); ++i) {
		display(s, children[i]);
	}
}
template<class DT>
int ParentMultiTree<DT>::max(int firstNumber, int secondNumber) {
	return firstNumber >= secondNumber ? firstNumber : secondNumber;
}
template<class DT>
int ParentMultiTree<DT>::getTreeHeight() {
	return nodeHeight(getRoot());
}
template<class DT>
int ParentMultiTree<DT>::getTreeSize() {
	return nodeSize(getRoot());
}
template<class DT>
int ParentMultiTree<DT>::nodeSize(const int& parentValue) // recursive
{
	int size = 0;
	int leftSize;
	if (getLeft(parentValue) == -1) {
		leftSize = 0;
	}
	else {
		leftSize = nodeSize(getLeft(parentValue));
	}
	if (getRight(parentValue) == -1) {
		size = 1 + leftSize;
	}
	else {
		size = 1 + leftSize + nodeSize(getRight(parentValue));
	}
	return size;
}
template<class DT>
int ParentMultiTree<DT>::nodeHeight(const int& parentValue) // recursive
{
	int height = 0;
	int leftHeight;

	if (getLeft(parentValue) == -1) {
		leftHeight = 0;
	}
	else {
		leftHeight = nodeHeight(getLeft(parentValue));
	}
	if (getRight(parentValue) == -1) {
		height = 1 + leftHeight;
	}
	else {
		height = 1 + max(leftHeight, nodeHeight(getRight(parentValue)));
	}
	return height;
}
template<class DT>
int ParentMultiTree<DT>::getRoot()
{
	return findIndexForValue(-1, 0);
}
template<class DT>
int ParentMultiTree<DT>::getLeft(const int& parentValue)
{
	int potentialLeft = findIndexForValue(parentValue, 0);
	if (potentialLeft == -1) {
		return -1;
	}
	if ((*_childOrderArray)[potentialLeft] == 0) {
		return potentialLeft;
	}
	return -1;
}
template<class DT>
int ParentMultiTree<DT>::getRight(const int& parentValue)
{
	int potentialRight = findIndexForValue(parentValue, 0);
	if (potentialRight == -1) {
		return -1;
	}
	if ((*_childOrderArray)[potentialRight] == 0) {
		int newPotentialRight = findIndexForValue(parentValue, potentialRight + 1);
		if (newPotentialRight != -1 && (*_childOrderArray)[newPotentialRight] == 1) {
			return newPotentialRight;
		}
	}
	else if ((*_childOrderArray)[potentialRight] == 1) {
		return potentialRight;
	}
	// if: newPotentialRight == -1
	return -1;
}
template <class DT>
Vector<int>& ParentMultiTree<DT>::getChildren(const int& parentValue) {
	Vector<int> children;
	for (int i = 0; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == value) {
			children.add((*_parentArray)[i]);
		}
	}
	return children;
}
template <class DT>
Vector<int>& ParentMultiTree<DT>::getChildrenOrder(const int& parentValue) {
	Vector<int> childOrder
		for (int i = 0; i < _numNodes; ++i) {
			if ((*_parentArray)[i] == value) {
				childOrder.add((*_childOrderArray)[i]);
			}
		}

	return childOrder;
}
template <class DT>
Vector<int> ParentMultiTree<DT>::reorder(Vector<int> arr, Vector<int> index) {
	Vector<int> temp(arr.size());
	for (int i = 0; i < arr.size(); ++i) {
		temp[index[i]] = arr[i];
	}

	for (int i = 0; i < arr.size(); ++i) {
		arr[i] = temp[i];
	}

	return arr;
}
template<class DT>
void ParentMultiTree<DT>::preOrderTreeTraversal() {
	preOrderTraversal(getRoot());
	cout << endl;
}
template<class DT>
void ParentMultiTree<DT>::preOrderTraversal(const int& parentValue) // recursive
{
	if (parentValue == -1 && getLeft(parentValue) == -1 && getRight(parentValue) == -1) {
		return;
	}
	cout << parentValue << " ";
	preOrderTraversal(getLeft(parentValue));
	preOrderTraversal(getRight(parentValue));
}
template <class DT>
void ParentMultiTree<DT>::preOrderChildrenTraversal(const int& parentValue) {
	Vector<int> children = reorder(getChildren(parentValue), getChildrenOrder(parentValue));
	if (children.size() == 0) {
		return;
	}
	cout << parentValue << " ";
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
template<class DT>
void ParentMultiTree<DT>::insertToTree(const int & parent, const int & left, const int & right)
{
	// Only happens first time
	if (getRoot() == -1) {
		if (parent >= _numNodes) {
			throw BinaryTreeMemory();
		}
		else {
			(*_parentArray)[parent] = -1;
			(*_childOrderArray)[parent] = -1;
		}
	}

	if (left != -1) {
		if (left >= _numNodes) {
			throw BinaryTreeMemory();
		}
		else {
			(*_parentArray)[left] = parent;
			(*_childOrderArray)[left] = 0;
		}
	}

	if (right != -1) {
		if (right >= _numNodes) {
			throw BinaryTreeMemory();
		}
		else {
			(*_parentArray)[right] = parent;
			(*_childOrderArray)[right] = 1;
		}
	}
}
template <class DT>
void ParentMultiTree<DT>::insertToTree(const int& parent, const DT& children) {
	// Only happens first time
	if (getRoot() == -1) {
		if (parent >= _numNodes) {
			throw BinaryTreeMemory();
		}
		else {
			(*_parentArray)[parent] = -1;
			(*_childOrderArray)[parent] = -1;
		}
	}

	for (int i = 0; i < chilren.size(); ++i) {
		if (children[i] > _numNodes) {
			throw BinaryTreeMemory();
		}
		else {
			(*_parentArray)[i] = children[i];
			(*_childOrderArray)[i] = i;
		}
	}
}
#pragma endregion Methods

int main() {
	int numberOfNodes;
	int parent;
	int left;
	int right;

	cin >> numberOfNodes;
	ParentMultiTree<ArrayClass<int>>* parentBinaryTree = new ParentMultiTree<ArrayClass<int>>(numberOfNodes);
	while (cin >> parent) {
		cin >> left;
		cin >> right;
		(*parentBinaryTree).insertToTree(parent, left, right);
	}
	cout << "Original Binary Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*parentBinaryTree) << endl;
	cout << "Size of tree: " << (*parentBinaryTree).getTreeSize() << endl;
	cout << "Height of tree: " << (*parentBinaryTree).getTreeHeight() << endl;

	ParentMultiTree<ArrayClass<int>>* testCopyBinaryTree = new ParentMultiTree<ArrayClass<int>>(*parentBinaryTree);
	cout << endl << "Copied Binary Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*testCopyBinaryTree) << endl;
	cout << "Size of tree: " << (*testCopyBinaryTree).getTreeSize() << endl;
	cout << "Height of tree: " << (*testCopyBinaryTree).getTreeHeight() << endl;

	ParentMultiTree<ArrayClass<int>>* testBinaryTree = new ParentMultiTree<ArrayClass<int>>(13);
	(*testBinaryTree).insertToTree(6, 4, 8);
	(*testBinaryTree).insertToTree(4, 2, 5);
	(*testBinaryTree).insertToTree(2, 1, 3);
	(*testBinaryTree).insertToTree(1, 0, -1);
	(*testBinaryTree).insertToTree(3, -1, -1);
	(*testBinaryTree).insertToTree(8, 7, 9);
	(*testBinaryTree).insertToTree(7, -1, -1);
	(*testBinaryTree).insertToTree(9, -1, 10);
	(*testBinaryTree).insertToTree(10, -1, 11);
	(*testBinaryTree).insertToTree(11, -1, 12);
	testCopyBinaryTree = testBinaryTree;
	cout << endl << "Copied Test Binary Tree:" << endl;
	cout << "Overloaded ostream operator (preorder): ";
	cout << (*testCopyBinaryTree) << endl;
	cout << "Size of tree: " << (*testCopyBinaryTree).getTreeSize() << endl;
	cout << "Height of tree: " << (*testCopyBinaryTree).getTreeHeight() << endl;

	delete parentBinaryTree;
	delete testCopyBinaryTree;
	// testBinaryTree already was deleted when assigned to testCopyBinaryTree
	cout << endl << "Original after deleting: " << (*parentBinaryTree) << endl;
	cout << "Copied Binary Tree after deleting: " << (*testCopyBinaryTree) << endl;
	cout << "Copied Test Binary Tree after deleting: " << (*testCopyBinaryTree) << endl;
}