#include <iostream>
using namespace std;

#pragma region Exceptions
class Exception { };																	// Generic, all exceptions derive from this
class BinaryTreeException : public Exception {};
class BinaryTreeMemory : public BinaryTreeException {};
class BinaryTreeGaveSubtreeToEmptyTree : public BinaryTreeException {};
class BinaryTreeEmptyTree : public BinaryTreeException {};
class ArrayException : public Exception { };											// Genric array exception, all array exceptions derive from
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
class ParentBinaryTree {
	// Preorder traversal
	friend ostream& operator<< (ostream& s, ParentBinaryTree<DT>& binaryTree);
protected:
	DT* _parentArray;
	DT* _childOrderArray;
	int _numNodes;
	int findIndexForValue(const int& value, const int& startingFrom);
public:
	ParentBinaryTree();
	ParentBinaryTree(int size);
	~ParentBinaryTree();
	ParentBinaryTree(const ParentBinaryTree<DT>& binaryTree);
	void operator= (const ParentBinaryTree<DT>& binaryTree);
	int size();
	int height();
	int getLeft(const int& parentValue);
	int getRight(const int& parentValue);
	void preorderTraversal(); // Root, Left, Right
	void inorderTraversal(); // Prints in a recursive fashion: Left, Root, Right
	void postOrderTraversal(); // Prints in a recursive fashion: Left, Right, Root
	void insertToTree(const int& parent, const int& left, const int& right);
};
template <class DT>
ParentBinaryTree<DT>::ParentBinaryTree() {
	_parentArray = new DT();
	_childOrderArray = new DT();
	_numNodes = 0;
}
template <class DT>
ParentBinaryTree<DT>::ParentBinaryTree(int size) {
	_parentArray = new DT(size, 0);
	_childOrderArray = new DT(size, 0);
	_numNodes = size;

}
template <class DT>
ParentBinaryTree<DT>::~ParentBinaryTree() {
	delete[] _parentArray;
	_parentArray = NULL;
	_numNodes = 0;
}
template <class DT>
ParentBinaryTree<DT>::ParentBinaryTree(const ParentBinaryTree<DT>& binaryTree) {

}

template<class DT>
void ParentBinaryTree<DT>::operator=(const ParentBinaryTree<DT>& binaryTree)
{
	return false;
}

template<class DT>
ostream & operator<<(ostream& s, const ParentBinaryTree<DT>& binaryTree)
{
	// TODO: insert return statement here

}
template<class DT>
int ParentBinaryTree<DT>::size()
{
	return 0;
}

template<class DT>
int ParentBinaryTree<DT>::height()
{
	return 0;
}

template<class DT>
int ParentBinaryTree<DT>::getLeft(const int& parentValue)
{
	int potentialLeft = findIndexForValue(parentValue, 0);
	if (potentialLeft == -1) {
		return -1; //FIXME
	}
	if ((*_childOrderArray)[potentialLeft] == 0) {
		return potentialLeft;
	}
	return -1;
}

template<class DT>
int ParentBinaryTree<DT>::getRight(const int& parentValue)
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
	return -1;
}

template<class DT>
void ParentBinaryTree<DT>::preorderTraversal()
{
}

template<class DT>
void ParentBinaryTree<DT>::inorderTraversal()
{
}

template<class DT>
void ParentBinaryTree<DT>::postOrderTraversal()
{
}
template<class DT>
int ParentBinaryTree<DT>::findIndexForValue(const int& value, const int& startingFrom) {
	for (int i = startingFrom; i < _numNodes; ++i) {
		if ((*_parentArray)[i] == value) {
			return i;
		}
	}
	return -1;
}

template<class DT>
void ParentBinaryTree<DT>::insertToTree(const int & parent, const int & left, const int & right)
{
	// Only happens first time
	if (findIndexForValue(-1, 0) == -1) {
		(*_parentArray)[parent] = -1;
		(*_childOrderArray)[parent] = -1;
	}

	if (left != -1) {
		(*_parentArray)[left] = parent;
		(*_childOrderArray)[left] = 0;
	}

	if (right != -1) {
		(*_parentArray)[right] = parent;
		(*_childOrderArray)[right] = 1;
	}
}
#pragma endregion Methods


int main() {
	int numberOfNodes;
	int parent;
	int left;
	int right;

	cin >> numberOfNodes;
	ParentBinaryTree<ArrayClass<int>> parentBinaryTree(numberOfNodes);
	while (cin >> parent) {
		cin >> left;
		cin >> right;

		parentBinaryTree.insertToTree(parent, left, right);
		cout << parent << " ";
		cout << left << " ";
		cout << right << endl;
	}
	int node = 0;
	cout << "The Right node of " << node << " is " << parentBinaryTree.getRight(node) << endl;
}