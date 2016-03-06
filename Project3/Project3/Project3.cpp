#include <iostream>
using namespace std;

class Exception { };												// Generic, all exceptions derive from this
class IncorrectTab : public Exception { };							// In case the user asked for a browser tab that doesn't exist yet
class IncorrectAction : public Exception { };						// In case the user asked for an action that won't work
class ArrayException : public Exception { };						// Genric array exception, all array exceptions derive from
class ArrayMemoryException : public ArrayException { };				// In case the array created causes an error
class ArrayBoundsException : public ArrayException { };				// In case the user asked for an index that was out of bounds

#pragma region array
template <class DataType>
class AbstractArrayClass {
public:
	virtual int size() const = NULL;
	virtual DataType& operator[] (int k) = NULL;
};
template <class DataType>
class ArrayClass : virtual public AbstractArrayClass<DataType> {
protected:
	DataType *paObject;
	int _size;
	void copy(const ArrayClass<DataType>& ac);
public:
	ArrayClass();
	ArrayClass(int n);
	ArrayClass(int n, const DataType& val);
	ArrayClass(const ArrayClass<DataType>& ac);
	virtual ~ArrayClass();
	virtual int size() const;
	virtual DataType& operator [] (int k);
	void operator=(const ArrayClass<DataType>& ac);
};
template <class DataType>
class AbstractVector : virtual public AbstractArrayClass<DataType> {
public:
	virtual void insert(const DataType& item, int index) = NULL; // insert a new object at position index in the vector
	virtual void remove(int index) = NULL; // removes the object at position index of the vector
	virtual void add(const DataType& item) = NULL; // adds item at end of the vector
};
template <class DataType>
class Vector : virtual public ArrayClass<DataType>, virtual public AbstractVector<DataType> {
protected:
	int _currSize;
	int _incFactor;
public:
	Vector();
	Vector(int n);
	Vector(int n, DataType& val);
	Vector(const Vector<DataType>& v);
	Vector(const ArrayClass<DataType>& ac);
	virtual ~Vector();
	void operator= (const Vector<DataType>& v);
	void operator= (const ArrayClass<DataType>& ac);
	virtual void insert(const DataType& item, int index);
	virtual void remove(int index);
	virtual void add(const DataType& item);
	virtual int size() const; // similar to size method as it returns size of underlying array
	virtual int capacity() const;
	virtual int incFactor() const;
	virtual void setIncFactor(int f);
	void setCapacity(int c);
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
	_size = 0; // default in case allocation fails
	paObject = new DataType[n];
	if (paObject == NULL) { throw ArrayMemoryException(); }
	_size = n;
}
template <class DataType>
ArrayClass<DataType>::ArrayClass(int n, const DataType& val) {
	_size = 0; // default in case allocation fails
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
	_size = 0;
}
template <class DataType>
void ArrayClass<DataType>::copy(const ArrayClass<DataType>& ac) {
	_size = 0; // default in case allocation fails
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
	if (paObject != NULL) { delete[] paObject; } // the already existing array is deleted and copied in to the new array
	copy(ac);
}
template <class DataType>
ostream& operator << (ostream& s, AbstractArrayClass<DataType>& ac) {
	s << "[";
	for (int i = 0; i < ac.size(); ++i) {
		if (i > 0) {
			s << ',';
		}
		s << ac[i];

	}
	s << "]";
	return s;
}
#pragma endregion Methods

#pragma region Vector
template <class DataType>
Vector<DataType>::Vector() : ArrayClass<DataType>() {
	// default values
	_currSize = 0;
	_incFactor = 5;
}
template <class DataType>
Vector<DataType>::Vector(int n) : ArrayClass<DataType>(n) {
	_currSize = 0;
	_incFactor = (n + 1) / 2; // arbitrary
}
template <class DataType>
Vector<DataType>::Vector(int n, DataType& val) : ArrayClass<DataType>(n, val) {
	_currSize = 0;
	_incFactor = n / 2; // arbitrary
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
#pragma endregion Methods

#pragma endregion Classes

template <class DT>
class AbstractLinkedList {
	friend ostream& operator<< (ostream& s, AbstractLinkedList<DT>& info);
public:
	virtual DT& info() = 0; // Returns the object in the head of the linked list
	virtual AbstractLinkedList<DT>* next() = 0; // Returns the LinkedLIst pointed by this LinkedList
	virtual bool isEmpty() = 0; // Returns true if the list is empty
	virtual void add(const DT& object) = 0; // Adds object to the beginning of the list
	virtual AbstractLinkedList<DT>* setNext(AbstractLinkedList<DT>* next) = 0; // Attaches next as _next field of list; returns old _next fiel
	virtual void insertAt(const DT& newObj, int position) = 0; // Inserts newObj so that it will be at node number position (counting the head node as 0)
	virtual DT& infoAt(int position) = 0; // Return the object in the linked list at the location specified by position
	virtual DT& find(const DT& key) = 0; // Returns a node matching key
	virtual DT remove() = 0; // Deletes the first node of the linked list, if any, and returns it
	virtual DT removeAt(int position) = 0; // Deletes the node matching key, if any, and returns it
	virtual int size() = 0; // Returns the number of nodes in the list
	virtual void display(ostream& s); // Display the nodes of the linked list
};

template <class DT>
void AbstractLinkedList<DT>::display(ostream& s) {

}

template <class DT>
class Cell : virtual public AbstractLinkedList<DT> {
	friend ostream& operator<< (ostream& s, Cell<DT>& info);
protected:
	DT* _value;
	Cell<DT>* _right;
	void Cell<DT>::copy(const Cell<DT>& linkedList);
public:
	Cell();
	Cell(const DT& info);
	Cell(const DT& info, Cell<DT>* cell);
	Cell(const Cell<DT>& linkedList);
	AbstractLinkedList<DT>* next();
	DT& info();
	bool isEmpty();
	void add(const DT& object);
	AbstractLinkedList<DT>* setNext(AbstractLinkedList<DT>* next);
	void insertAt(const DT& newObj, int position);
	DT& infoAt(int position);
	DT& find(const DT& key);
	DT remove();
	DT removeAt(int position);
	int size();
	~Cell();
	void operator= (const Cell<DT>& cell);
};

template <class FirstDT, class SecondDT>
class CellNode {
	friend ostream& operator<< (ostream& s, CellNode<FirstDT, SecondDT>& cellNode);
protected:
	FirstDT* _info;
	Cell<SecondDT>* _myCell;
public:
	CellNode();
	CellNode(const FirstDT& info);
	CellNode(const CellNode<FirstDT, SecondDT>& cellNode);
	CellNode<FirstDT, SecondDT>(const FirstDT& info, Cell<SecondDT>* myCell);
	~CellNode();
	void copy(const CellNode<FirstDT, SecondDT>& cellNode);
	Cell<SecondDT>& returnMyCell();
	void operator= (const CellNode<FirstDT, SecondDT>& tab);
};

template <class FirstDT, class SecondDT>
class MasterCell {
protected:
	Vector<CellNode<FirstDT, SecondDT>*> _myCellNodes;
public:
	MasterCell();
	MasterCell(int vectorSize);
	MasterCell(const CellNode<FirstDT, SecondDT>& cellNode);
	~MasterCell();
	void MasterCell<FirstDT, SecondDT>::insertCellNode(const CellNode<FirstDT, SecondDT>& cellNode);
	void operator= (const MasterCell<FirstDT, SecondDT>& masterCell);
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
		if (_right == NULL) {

		}
		return _right->find(key);
	}
	else {

	}
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
template <class DT>
ostream& operator<< (ostream& s, Cell<DT>& info) {
	return s;
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
	_info = info;
	_myCell = NULL;
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode(const CellNode<FirstDT, SecondDT>& cellNode) {
	copy(cellNode);
}
template <class FirstDT, class SecondDT>
CellNode<FirstDT, SecondDT>::CellNode<FirstDT, SecondDT>(const FirstDT& info, Cell<SecondDT>* myCell) {
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
		_info = new DT(*(cellNode._info));

	}
	if (cellNode._myCell == NULL) {
		if (_info != NULL) {
			_myCell = NULL;
		}
	}
	else {
		_myCell = new Cell<DT>(*(cellNode._myCell));
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
ostream& operator<< (ostream& s, CellNode<FirstDT, SecondDT>& cellNode) {
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

}
template <class FirstDT, class SecondDT>
void MasterCell<FirstDT, SecondDT>::insertCellNode(const CellNode<FirstDT, SecondDT>& cellNode) {
	
}
template<class FirstDT, class SecondDT>
void MasterCell<FirstDT, SecondDT>::operator= (const MasterCell<FirstDT, SecondDT>& masterCell) {
	for (int i = 0; i < _myCellNodes.size(); ++i) {
		
	}
}
#pragma endregion Methods

int main() {

	int intInfo;
	int noItems;
	int id;
	char charInfo;
	char comma;

	
	/*intInfo = 5;
	int test[] = { 1,2,3 };
	Cell<int>* mainCell = new Cell<int>(test[0]);
	for (int i = 1; i < 3; ++i) {
		
	}
	CellNode<int, int> example(intInfo, testCell);
	for (int i = 0; i < 3; ++i) {
		cout << (*testCell).infoAt(i) << endl;
	}*/
#pragma region Character
	// Project3_InputFile_Part1
	//while (cin >> charInfo) {
	//	cout << charInfo;
	//	do {
	//		cin.get(charInfo);
	//		cout << charInfo;
	//	} while (cin.peek() != ',');
	//	cin.get(comma);
	//	cout << comma;
	//	cin >> noItems;
	//	cout << " " << noItems;
	//	for (int i = 0; i < noItems; ++i) {
	//		cin >> id;
	//		// Do stuff with id
	//		cout << " " << id;
	//	}
	//	cout << endl;
	//}
#pragma endregion Input

#pragma region Integer
	// Project3_InputFile_Part2
	MasterCell<int, int> masterCell;
	while (cin >> intInfo) {
		cin.get(comma);
		cin >> noItems;
		Cell<int>* cell = new Cell<int>();
		for (int i = 0; i < noItems; ++i) {
			cin >> id;
			// Do stuff with id
			(*cell).insertAt(id, i);
		}
		CellNode<int, int> cellNode(intInfo, cell);
		masterCell.insertCellNode(cellNode);
		
	}
	
#pragma endregion Input
}
