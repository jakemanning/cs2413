#include <iostream>
using namespace std;

#pragma region Exceptions
class Exception { };																	// Generic, all exceptions derive from this
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

#pragma region abstractLinkedList
template <class DT>
class Enumeration {
public:
	virtual bool hasMoreElements() = 0;													// Boolean method which determines whether there are any more elements in the data structure being Enumerated
	virtual DT& nextElement() = 0;														// Returns the object which is the next element
};

template <class DT>
class AbstractLinkedList {
public:
	virtual DT& info() = 0;																// Returns the object in the head of the linked list
	virtual AbstractLinkedList<DT>* next() = 0;											// Returns the LinkedLIst pointed by this LinkedList
	virtual bool isEmpty() = 0;															// Returns true if the list is empty
	virtual void add(const DT& object) = 0;												// Adds object to the beginning of the list
	virtual AbstractLinkedList<DT>* setNext(AbstractLinkedList<DT>* next) = 0;			// Attaches next as _next field of list; returns old _next fiel
	virtual void insertAt(const DT& newObj, int position) = 0;							// Inserts newObj so that it will be at node number position (counting the head node as 0)
	virtual DT& infoAt(int position) = 0;												// Return the object in the linked list at the location specified by position
	virtual DT& find(const DT& key) = 0;												// Returns a node matching key
	virtual DT remove() = 0;															// Deletes the first node of the linked list, if any, and returns it
	virtual DT removeAt(int position) = 0;												// Deletes the node matching key, if any, and returns it
	virtual int size() = 0;																// Returns the number of nodes in the list
	virtual Enumeration<DT>* enumerator();												// Returns an enumeration of the data contained in the list
	virtual void display(ostream& s);													// Display the nodes of the linked list
};

template <class DT>
class LLEnumerator : public Enumeration<DT> {
	friend AbstractLinkedList<DT>;
protected:
	AbstractLinkedList<DT>* _LL;
public:
	LLEnumerator();
	LLEnumerator(AbstractLinkedList<DT>* LL);
	virtual bool hasMoreElements();
	virtual DT& nextElement();
};

template <class DT>
Enumeration<DT>* AbstractLinkedList<DT>::enumerator() {
	// Returns an enumeration of the data contained in the list 
	return new LLEnumerator<DT>(this);
}

template <class DT>
LLEnumerator<DT>::LLEnumerator() {
	_LL = NULL;
}
template<class DT>
LLEnumerator<DT>::LLEnumerator(AbstractLinkedList<DT>* LL) {
	_LL = LL;
}
template<class DT>
bool LLEnumerator<DT>::hasMoreElements() {
	return ((_LL != NULL) && (!_LL->isEmpty()));
}
template <class DT>
DT& LLEnumerator<DT>::nextElement() {
	if ((_LL == NULL) || (_LL->isEmpty())) {
		cout << "Empty";
	}
	else {
		AbstractLinkedList<DT>* curr = _LL;
		_LL = _LL->next();
		return curr->info();
	}
}
template <class DT>
void AbstractLinkedList<DT>::display(ostream& s) {
	s << "[";
	bool first = true;
	Enumeration<DT>* e = enumerator();

	while (e->hasMoreElements()) {
		if (!first) {
			s << ", ";
		}
		else {
			first = false;
		}
		s << e->nextElement();
	}
	s << "]";
	delete e;
}
template <class DT>
ostream& operator<< (ostream& s, AbstractLinkedList<DT>& LL) {
	LL.display(s);
	return s;
}
#pragma endregion Definitions

#pragma region LinkedList
template <class DT>
class Cell : virtual public AbstractLinkedList<DT> {
protected:
	DT* _value;																			// Each cell's value
	Cell<DT>* _right;																	// The reference to the cell to the right
	void Cell<DT>::copy(const Cell<DT>& linkedList);									// Copies all information from given cell to current cell and deletes given cell
public:
	Cell();																				// Constructs a new cell
	Cell(const DT& info);																// Constructs a new cell with the supplied information
	Cell(const DT& info, Cell<DT>* cell);												// Constructs a new cell with the supplied information and the cell to the riht
	Cell(const Cell<DT>& linkedList);													// Constructs a new Cell with the supplied copy constructor
	AbstractLinkedList<DT>* next();														// The next cell in the list
	DT& info();																			// The underlying node's information
	bool isEmpty();																		// Checks whether the info in the cell is null
	void add(const DT& object);															// Adds a new cell to the front of the list with the supplied information
	AbstractLinkedList<DT>* setNext(AbstractLinkedList<DT>* next);						// Sets the cell to the right to the supplied list
	void insertAt(const DT& newObj, int position);										// Inserts a cell to the end of the list
	DT& infoAt(int position);															// Return the value in the linked list at the location specified by position
	DT& find(const DT& key);															// Returns a node matching key
	DT remove();																		// Deletes the first node of the linked list, if any, and returns it
	DT removeAt(int position);															// Deletes the node matching key, if any, and returns it
	int size();																			// Returns the number of cells in the list
	~Cell();																			// Destructor
	void operator= (const Cell<DT>& cell);												// Copies information from supplied cell to current cell
};

template <class FirstDT, class SecondDT>
class CellNode {
protected:
	FirstDT* _info;																		// Each CellNode's information
	Cell<SecondDT>* _myCell;															// Link to the underlying cellNode's cell
public:
	CellNode();																			// Constructs an empty cell node
	CellNode(const FirstDT& info);														// Constructs a cell node with infromation
	CellNode(const CellNode<FirstDT, SecondDT>& cellNode);								// Constructs a new cell node with the supplied cellNode
	CellNode<FirstDT, SecondDT>(const FirstDT& info, Cell<SecondDT>* myCell);			// Constructs a new cell with the supplied information and cell
	~CellNode();																		// Destructor
	void copy(const CellNode<FirstDT, SecondDT>& cellNode);								// Copies infromation from supplied cell node to current cell
	Cell<SecondDT>& returnMyCell();														// Returns the underlying cell
	FirstDT& returnMyInfo();															// Returns the underlying information
	void operator= (const CellNode<FirstDT, SecondDT>& cellnode);						// Copies information from supplied cellnode to current cellNode
};

template <class FirstDT, class SecondDT>
class MasterCell {
protected:
	Vector<CellNode<FirstDT, SecondDT>> _myCellNodes;									// The underlying resizable vector containing all of the cell nodes
public:
	MasterCell();																		// Constructs an empty Master Cell
	MasterCell(int vectorSize);															// Sets the capacity of the underlying vector
	MasterCell(const CellNode<FirstDT, SecondDT>& cellNode);							// Constructs a master cell with a constructor including a cell Node
	~MasterCell();																		// Destructor
	void insertCellNode(CellNode<FirstDT, SecondDT>& cellNode);							// Inserts the cell node to the end of the underlying vector
	void operator= (const MasterCell<FirstDT, SecondDT>& masterCell);					// Copies information from the supplied masterCell to the current masterCell
	Vector<CellNode<FirstDT, SecondDT>>& getVector();									// Retrieves the vector
	Vector<FirstDT>& findKeywords(int keyword);											// Returns an array of strings that matches an input value
	Vector<int>& and(FirstDT& firstKeyword, FirstDT& secondKeyword);					// Takes two keywords and returns all integers that are common to both
	Vector<int>& or (FirstDT& firstKeyword, FirstDT& secondKeyword);					// Takes two keywords and returns all integers that are in either of them
	Vector<int>& xor(FirstDT& firstKeyword, FirstDT& secondKeyword);					// Takes two keywords and returns all integers that are not common in both
	Vector<int>& and(Vector<int>& intArray, FirstDT& firstKeyword);						// Takes two keywords and returns all integers that are common to both
	Vector<int>& or (Vector<int>& intArray, FirstDT& firstKeyword);						// Takes two keywords and returns all integers that are in either of them
	Vector<int>& xor(Vector<int>& intArray, FirstDT& firstKeyword);						// Takes two keywords and returns all integers that are not common in both

};

#pragma region Cell
template <class DT>
Cell<DT>::Cell() {
	_value = NULL;
	_right = NULL;
	// Add to front
}
template <class DT>
Cell<DT>::Cell(const DT& value) {
	_value = new DT(value);
	if (_value != NULL) {
		_right = NULL;
	}
	else {
		cout << "Value is Null" << endl;
	}
	// Add to front
}
template <class DT>
Cell<DT>::Cell(const DT& info, Cell<DT>* cell) {
	_value = new DT(info);
	if (_value != NULL) {
		_right = cell;
	}
	else {
		cout << "Value is Null" << endl;
	}
	// Add to front
}
template <class DT>
Cell<DT>::Cell(const Cell<DT>& linkedList) {
	copy(linkedList);
	// Add to front
}
template <class DT>
Cell<DT>::~Cell() {
	if (_value != NULL) {
		delete _value;
		_value = NULL;
	}
	if (_right != NULL) {
		delete _right; // Recursive call to destructor
		_right = NULL;
	}
}
template <class DT>
void Cell<DT>::copy(const Cell<DT>& linkedList) {
	if (linkedList._value == NULL) {
		_value = NULL;
	}
	else {
		_value = new DT(*(linkedList._value));

	}
	if (linkedList._right == NULL) {
		if (_value != NULL) {
			_right = NULL;
		}
	}
	else {
		_right = new Cell<DT>(*(linkedList._right));
	}
}
template <class DT>
void Cell<DT>::operator= (const Cell<DT>& linkedList) {
	if (_value != NULL) {
		delete _value;
	}
	if (_right != NULL) {
		delete _right;
	}
	copy(linkedList);
}
template <class DT>
bool Cell<DT>::isEmpty() {
	return (_value == NULL);
}
template <class DT>
DT& Cell<DT>::info() {
	if (!isEmpty()) {
		return *_value;
	}
}
template <class DT>
AbstractLinkedList<DT>* Cell<DT>::next() {
	return _right;
}
template <class DT>
void Cell<DT>::add(const DT& object) {
	if (_value == NULL) {
		_value = new DT(object);
	}
	else {
		Cell<DT>* newList = new Cell<DT>(*_value, _right);
		if (newList != NULL) {
			*_value = object;
			_right = newList;
		}
	}
}
template <class DT>
void Cell<DT>::insertAt(const DT& newObj, int position) {
	if (position == 0) {
		add(newObj);
	}
	else {
		if (_right == NULL) {
			_right = new Cell(newObj);
		}
		else {
			if (_right != NULL) {
				_right->insertAt(newObj, position - 1);
			}
		}
	}
}
template <class DT>
AbstractLinkedList<DT>* Cell<DT>::setNext(AbstractLinkedList<DT>* next) {
	if (!isEmpty()) {
		AbstractLinkedList<DT>* temp = _right;
		_right = dynamic_cast<Cell<DT>*>(next);
		return temp;
	}

}
template <class DT>
int Cell<DT>::size() {
	if (_right == NULL) {
		if (_value == NULL) {
			return 0;
		}
		else {
			return 1;
		}
	}
	else return 1 + _right->size();
}
template <class DT>
DT& Cell<DT>::find(const DT& key) {
	if (!isEmpty()) {
		if (key == *_value) {
			return *_value;
		}
		if (_right != NULL) {
			return _right->find(key);
		}
	}
	int* notFound = new int(-1);
	return *notFound;

}
template <class DT>
DT& Cell<DT>::infoAt(int position) {
	if (!isEmpty()) {
		if (position == 0) {
			return *_value;
		}
		if (_right != NULL) {
			return _right->infoAt(position - 1);
		}
	}

}
template <class DT>
DT Cell<DT>::remove() {
	if (!isEmpty()) {
		DT temp = *_value;
		delete _value;
		if (_right == NULL) {
			_value = NULL;
		}
		else {
			Cell<DT>* oldNext = _right;
			_value = _right->_value;
			_right = _right->_right;
			// Remove stray pointers to linked list
			oldNext->_value = NULL;
			oldNext->_right = NULL;
			delete oldNext;
		}
		return temp;
	}

}
template <class DT>
DT Cell<DT>::removeAt(int position) {
	if (!isEmpty()) {
		if (position == 0) {
			return remove();
		}
		if (_right == NULL) {

		}
		return _right->removeAt(position - 1);
	}

}
#pragma endregion Methods

#pragma region CellNode
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode() {
	_info = NULL;
	_myCell = NULL;
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode(const FirstDT& info) {
	_info = = new FirstDT(info);
	if (_info != NULL) {
		_myCell = NULL;
	}
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode(const CellNode<FirstDT, SecondDT>& cellNode) {
	copy(cellNode);
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode(const FirstDT& info, Cell<SecondDT>* myCell) {
	_info = new FirstDT(info);
	if (_info != NULL) {
		_myCell = myCell;
	}
}
template <class FirstDT, class SecondDT>
void CellNode<FirstDT, SecondDT>::copy(const CellNode<FirstDT, SecondDT>& cellNode) {
	if (cellNode._info == NULL) {
		_info = NULL;
	}
	else {
		_info = new FirstDT(*(cellNode._info));

	}
	if (cellNode._myCell == NULL) {
		if (_info != NULL) {
			_myCell = NULL;
		}
	}
	else {
		_myCell = new Cell<SecondDT>(*(cellNode._myCell));
	}
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::~CellNode() {
	if (_info != NULL) {
		delete _info;
		_info = NULL;
	}
	if (_myCell != NULL) {
		delete _myCell; // Recursive call to destructor
		_myCell = NULL;
	}
}
template <class FirstDT, class SecondDT>
void CellNode<FirstDT, SecondDT>::operator= (const CellNode<FirstDT, SecondDT>& cellNode) {
	if (_info != NULL) {
		delete _info;
	}
	if (_myCell != NULL) {
		delete _myCell;
	}
	copy(cellNode);
}
template <class FirstDT, class SecondDT>
Cell<SecondDT>& CellNode<FirstDT, SecondDT>::returnMyCell() {
	if (_myCell != NULL) {
		return (*_myCell);
	}
}
template <class FirstDT, class SecondDT>
FirstDT& CellNode<FirstDT, SecondDT>::returnMyInfo() {
	if (_info != NULL) {
		return (*_info);
	}
}
template <class FirstDT, class SecondDT>
ostream& operator<< (ostream& s, CellNode<FirstDT, SecondDT> & cellNode) {
	s << "Info: " << cellNode.returnMyInfo() << ", Cells: " << cellNode.returnMyCell();
	return s;
}
#pragma endregion Methods

#pragma region MasterCell
template <class FirstDT, class SecondDT>
MasterCell<FirstDT, SecondDT>::MasterCell() {

}
template <class FirstDT, class SecondDT>
MasterCell<FirstDT, SecondDT>::MasterCell(int vectorSize) {
	_myCellNodes.setCapacity(vectorSize);
}
template<class FirstDT, class SecondDT>
MasterCell<FirstDT, SecondDT>::MasterCell(const CellNode<FirstDT, SecondDT>& cellNode) {
	_myCellNodes.add(cellNode);
}
template <class FirstDT, class SecondDT>
MasterCell<FirstDT, SecondDT>::~MasterCell() {
	// automatically deletes internal vector
}
template <class FirstDT, class SecondDT>
void MasterCell<FirstDT, SecondDT>::insertCellNode(CellNode<FirstDT, SecondDT>& cellNode) {
	_myCellNodes.add(cellNode);
}
template<class FirstDT, class SecondDT>
void MasterCell<FirstDT, SecondDT>::operator= (const MasterCell<FirstDT, SecondDT>& masterCell) {
	_myCellNodes = masterCell._myCellNodes;
}
template <class FirstDT, class SecondDT>
Vector<CellNode<FirstDT, SecondDT>>& MasterCell<FirstDT, SecondDT>::getVector() {
	return _myCellNodes;
}
template <class FirstDT, class SecondDT>
ostream& operator<< (ostream& s, MasterCell<FirstDT, SecondDT>& masterCell) {
	for (int i = 0; i < masterCell.getVector().size(); ++i) {
		s << masterCell.getVector()[i];
		if (i + 1 != masterCell.getVector().size()) {
			s << endl;
		}
	}
	return s;
}
template<class FirstDT, class SecondDT>
Vector<FirstDT>& MasterCell<FirstDT, SecondDT>::findKeywords(int keyword)
{
	Vector<FirstDT>* strings = new Vector<FirstDT>(_myCellNodes.size());
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		if (_myCellNodes[i].returnMyCell().find(keyword) != -1) {
			(*strings).add(_myCellNodes[i].returnMyInfo());
		}
	}
	return *strings;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: and (FirstDT& firstKeyword, FirstDT& secondKeyword)
{
	int firstKeywordIndex = -1, secondKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
			else if (_myCellNodes[i].returnMyInfo() == secondKeyword) {
				secondKeywordIndex = i;
			}

		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	// Takes O(N^2) time, loops through all values in second list for each value in first list
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int infoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (_myCellNodes[secondKeywordIndex].returnMyCell().find(infoAtIndex) != -1) {
			(*ids).add(infoAtIndex);
		}
	}
	return *ids;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: or (FirstDT& firstKeyword, FirstDT& secondKeyword)
{
	int firstKeywordIndex = -1, secondKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
			else if (_myCellNodes[i].returnMyInfo() == secondKeyword) {
				secondKeywordIndex = i;
			}
		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	// Adds all ids, if not already inside
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int firstInfoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (!(*ids).contains(firstInfoAtIndex)) {
			(*ids).add(firstInfoAtIndex);
		}
	}
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int secondInfoAtIndex = _myCellNodes[secondKeywordIndex].returnMyCell().infoAt(index);
		if (!(*ids).contains(secondInfoAtIndex)) {
			(*ids).add(secondInfoAtIndex);
		}
	}
	return *ids;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: xor (FirstDT& firstKeyword, FirstDT& secondKeyword)
{
	int firstKeywordIndex = -1, secondKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
			else if (_myCellNodes[i].returnMyInfo() == secondKeyword) {
				secondKeywordIndex = i;
			}

		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	// Checks each value in second list for value in first list, to check not contains
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int infoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (_myCellNodes[secondKeywordIndex].returnMyCell().find(infoAtIndex) == -1) {
			if (!(*ids).contains(infoAtIndex)) {
				(*ids).add(infoAtIndex);
			}
		}
	}
	for (int index = 0; index < _myCellNodes[secondKeywordIndex].returnMyCell().size(); ++index) {
		int infoAtIndex = _myCellNodes[secondKeywordIndex].returnMyCell().infoAt(index);
		if (_myCellNodes[firstKeywordIndex].returnMyCell().find(infoAtIndex) == -1) {
			if (!(*ids).contains(infoAtIndex)) {
				(*ids).add(infoAtIndex);
			}
		}
	}
	return *ids;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: and (Vector<int>& intArray, FirstDT& firstKeyword)
{
	int firstKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	// Similar to above 'AND' method
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int infoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (intArray.contains(infoAtIndex)) {
			(*ids).add(infoAtIndex);
		}
	}
	return *ids;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: or (Vector<int>& intArray, FirstDT& firstKeyword)
{
	int firstKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	// Similar to above 'OR' method
	for (int index = 0; index < intArray.size(); ++index) {
		int secondInfoAtIndex = intArray[index];
		if (!(*ids).contains(secondInfoAtIndex)) {
			(*ids).add(secondInfoAtIndex);
		}
	}
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int firstInfoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (!(*ids).contains(firstInfoAtIndex)) {
			(*ids).add(firstInfoAtIndex);
		}
	}

	return *ids;
}
template<class FirstDT, class SecondDT>
Vector<int>& MasterCell<FirstDT, SecondDT>:: xor (Vector<int>& intArray, FirstDT& firstKeyword)
{
	int firstKeywordIndex = -1;
	Vector<int>* ids = new Vector<int>();
	// Finds first and second keyword index
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		try {
			if (_myCellNodes[i].returnMyInfo() == firstKeyword) {
				firstKeywordIndex = i;
			}
		}
		catch (ArrayBoundsException) {
			cout << "Didn't contain firstKeyword or secondKeyword" << endl;
		}
	}
	//Similar to above 'XOR' method
	for (int index = 0; index < intArray.size(); ++index) {
		int infoAtIndex = intArray[index];
		if (_myCellNodes[firstKeywordIndex].returnMyCell().find(infoAtIndex) == -1) {
			if (!(*ids).contains(infoAtIndex)) {
				(*ids).add(infoAtIndex);
			}
		}
	}
	for (int index = 0; index < _myCellNodes[firstKeywordIndex].returnMyCell().size(); ++index) {
		int infoAtIndex = _myCellNodes[firstKeywordIndex].returnMyCell().infoAt(index);
		if (!intArray.contains(infoAtIndex)) {
			if (!(*ids).contains(infoAtIndex)) {
				(*ids).add(infoAtIndex);
			}
		}
	}
	return *ids;
}
#pragma endregion Methods
#pragma endregion Definitions

template <class DataType>
void vectEmpty(Vector<DataType>& str) {
	for (int i = str.size() - 1; i >= 0; --i) {
		str.remove(i);
	}
}

int main() {
	Vector<char> charInfo;																// Information used when reading InputFile_Part1
	int intInfo;																		// Information used when reading InputFile_Part2
	int noItems;																		// Number of id's to read
	int noLines;																		// Number of lines to read
	int id;																				// Each ID that is read
	char buffer;																		// Used to read into charInfo
	char comma;																			// Junk variable
	char operate;																		// Operator input
	Vector<Vector<char>> operands(7);													// Character operands
	Vector<char> operators(5);															// Character operators
	MasterCell<Vector<char>, int> charMasterCell;										// Master cell
	cin >> noLines;
	for (int i = 0; i < noLines; ++i) {
		cin >> buffer;
		vectEmpty(charInfo);
		charInfo.add(buffer);
		do {
			cin.get(buffer);
			charInfo.add(buffer);
		} while (cin.peek() != ',');

		cin.get(comma);
		cin >> noItems;
		Cell<int>* cell = new Cell<int>();
		for (int i = 0; i < noItems; ++i) {
			cin >> id;
			// Do stuff with id
			(*cell).insertAt(id, i);
		}
		CellNode<Vector<char>, int> cellNode(charInfo, cell);
		charMasterCell.insertCellNode(cellNode);
	}
	cout << charMasterCell << endl;

	while (cin >> buffer) {
		vectEmpty(operands);
		vectEmpty(operators);
		do {
			vectEmpty(charInfo);
			charInfo.add(buffer);
			do {
				cin.get(buffer);
				charInfo.add(buffer);
			} while (cin.peek() != '&' && cin.peek() != '|' && cin.peek() != '%' && cin.peek() != '\n');
			if (cin.peek() != '\n') {
				charInfo.remove(charInfo.size() - 1);
				operands.add(charInfo);
				cout << charInfo;
				cin.get(operate);
				cout << " " << operate;
				operators.add(operate);
				cin.get(comma);
				cout << " ";
				cin.get(buffer);
			}
			else {
				cout << charInfo << ":";
				operands.add(charInfo);
			}
		} while (cin.peek() != '\n');
		Vector<int> ids;
		switch (operators[0]) {
		case '&':
			ids = charMasterCell. and (operands[0], operands[1]);
			break;
		case '|':
			ids = charMasterCell. or (operands[0], operands[1]);
			break;
		case '%':
			ids = charMasterCell. xor (operands[0], operands[1]);
			break;
		default:
			cout << "Illegal operator" << endl;
			break;
		}

		for (int i = 2; i < operands.size(); ++i) {
			switch (operators[i - 1]) {
			case '&':
				ids = charMasterCell. and (ids, operands[i]);
				break;
			case '|':
				ids = charMasterCell. or (ids, operands[i]);
				break;
			case '%':
				ids = charMasterCell. xor (ids, operands[i]);
				break;
			default:
				cout << "Illegal operator" << endl;
				break;
			}
		}
		for (int i = 0; i < ids.size(); ++i) {
			cout << ids[i] << " ";
		}
		if (ids.size() == 0) {
			cout << "No relationship";
		}
		cout << endl;
	}


#pragma region Testing
	cout << endl << "TEST CASES:" << endl;
	char one[] = { 'D','i','s','c','r','e','t','e',' ','M','a','t','h','e','m','a','t','i','c','s','\0' };
	char two[] = { 'C','o','m','p','i','l','e','r',' ','C','o','n','s','t','r','u','c','t','i','o','n','\0' };
	Vector<char> oneTest(21);
	Vector<char> twoTest(21);
	for (int i = 0; one[i] != '\0'; ++i) {
		oneTest.add(one[i]);
	}
	for (int i = 0; two[i] != '\0'; ++i) {
		twoTest.add(two[i]);
	}
	Vector<int> test1 = charMasterCell. and (oneTest, twoTest);
	Vector<int> test2 = charMasterCell. or (oneTest, twoTest);
	Vector<int> test3 = charMasterCell. xor (oneTest, twoTest);

	cout << "Discrete Mathematics AND Compiler Construction: ";
	for (int i = 0; i < test1.size(); ++i) {
		cout << test1[i];
		if (i < test1.size() - 1) {
			cout << ",";
		}
	}
	cout << endl << "Discrete Mathematics OR Compiler Construction: ";
	for (int i = 0; i < test2.size(); ++i) {
		cout << test2[i];
		if (i < test2.size() - 1) {
			cout << ",";
		}
	}
	cout << endl << "Discrete Mathematics XOR Compiler Construction: ";
	for (int i = 0; i < test3.size(); ++i) {
		cout << test3[i];
		if (i < test3.size() - 1) {
			cout << ",";
		}
	}
	cout << endl;
	int testNumber = 30;
	cout << "Find classes with " << testNumber << ':';
	Vector<Vector<char>> strings = charMasterCell.findKeywords(testNumber);
	for (int i = 0; i < strings.size(); ++i) {
		cout << strings[i];
		if (i < strings.size() - 1) {
			cout << ",";
		}
	}
	cout << endl;
#pragma endregion HERE
}