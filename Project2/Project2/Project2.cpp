#include <iostream>
using namespace std;

class Exception { };
class ArrayException : public Exception { };
class ArrayMemoryException : public ArrayException { };
class ArrayBoundsException : public ArrayException { };

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

/* Encapsulates a URL string*/
class webAddressInfo
{
private:
	char url[201];									// allow a maximum of 200 characters
public:
	webAddressInfo();								// empty constructor; automatically called by statically defined variable in browserTab class
	webAddressInfo(char* inputString);				// never called in project, due to the fact all URLs are statically instantiated
	void setWebAddressInfo(char* inputString);		// reads in the input string to url, the extraneous space is already filled with string terminators
	char* getWebAddressInfo();						// returns a pointer to the first element of url
	void display();									// displays full url to the console, stops on string terminator
};

/* Contains up to twenty url objects; allows output and transitions between urls*/
class browserTab {
protected:
	int numAddress;									// Current number of web addresses in this tab
	webAddressInfo webAddresses[20];				// Web addresses in this tab
	int currentAddress;								// index of current location in webAddresses
public:
	browserTab();									// empty constructor; automatically called by statically defined variable in main class
	browserTab(char* inputString);					// creates a new tab with the inputString; never called in project due to static instantiation
	webAddressInfo& forward();						// returns the 'box' for either the the next url, or the current one if on most recent url
	webAddressInfo& backward();						// returns the 'box' for either the previous url, or the current one if on least recent url
	void addAddress(char* inputString);				// creates url (webAddressInfo), sets current index to numAddress - 1, becuase numAddress size is not zero-indexed, prints url
	void display();									// displays each webAddressInfo url in the browserTab
};

#pragma region webAddressInfo
webAddressInfo::webAddressInfo() {

}
webAddressInfo::webAddressInfo(char* inputString) {
	setWebAddressInfo(inputString);
}

void webAddressInfo::setWebAddressInfo(char* inputString) {
	for (int i = 0; i < 201; ++i) {
		url[i] = inputString[i];
	}
}

char* webAddressInfo::getWebAddressInfo() {
	return url;
}

void webAddressInfo::display() {
	// terminates when the value of url at the index is either string terminator, or max size of url array
	for (int i = 0; url[i] != '\0' && i < 201; ++i) {
		cout << url[i];
	}
	cout << endl;
}
#pragma endregion Methods

#pragma region browserTab
browserTab::browserTab() {
	numAddress = 0;
	currentAddress = 0;
}

browserTab::browserTab(char* inputString) {
	numAddress = 0;
	currentAddress = 0;
	addAddress(inputString);
}

webAddressInfo& browserTab::forward() {
	if (currentAddress + 1 < numAddress) {
		++currentAddress;
		return webAddresses[currentAddress];
	}
	else {
		cout << "Already on most current tab; printing - ";
		currentAddress = numAddress - 1;			// in case current address index is somehow greater than or equal to numAddress index
	}
	return webAddresses[currentAddress];
}

webAddressInfo& browserTab::backward() {
	if (currentAddress > 0) {
		--currentAddress;
		return webAddresses[currentAddress];
	}
	else {
		cout << "Already moved back as far as possible; printing - ";
		currentAddress = 0;							// in case current address index is somehow less than zero
	}
	return webAddresses[currentAddress];
}

void browserTab::addAddress(char* inputString) {
	++numAddress;
	currentAddress = numAddress - 1;				// sets currentAddress to most recently added address
	webAddresses[currentAddress].setWebAddressInfo(inputString);
	webAddresses[currentAddress].display();
}

void browserTab::display() {
	for (int i = 0; i < numAddress; ++i) {
		webAddresses[i].display();
	}
}
#pragma endregion Methods

// sets each character in a string for a given length to string terminators
void strEmpty(char* someString, int length) {
	for (int i = 0; i < length; ++i) {
		someString[i] = '\0';
	}
}

int main()
{
	char command;									// the given command, e.g. New tab, forward, backward, or print
	char blank;										// offload variable, junk
	char aChar;										// reads in url to char, for safety
	char webAddress[201];							// the web address to be wrapped into object
	browserTab myTabs[20];							// statically creates 20 tabs, which statically creates 400 webAddressInfo objects
	int tabNumber;									// the browserTab object to manipulate
	int i;											// loop variable


										// other local variables used to store data temporally
	while (cin >> tabNumber)						// while end of line is not reached
	{
		cin.get(blank);
		cin.get(command);
		strEmpty(webAddress, 201);
		
		switch (command) {
		case 'N': { // New url
			cin.get(blank);
			i = 0;
			do {
				cin.get(aChar);
				if (aChar != '\n') {
					webAddress[i++] = aChar;
				}
			} while ((aChar != '\n') && (i < 201) && !cin.eof());
			if (i > 0) {
				cout << "Adding address to tab #" << tabNumber << endl;
			}
			break; } 
		case 'F': { // Forward
			cout << "Attempting to move forwards in tab #" << tabNumber << " - ";
			myTabs[tabNumber - 1].forward().display();
			break; 
		}
		case 'B': { // Backward
			cout << "Attempting to move backwards in tab #" << tabNumber << " - ";
			myTabs[tabNumber - 1].backward().display();
			break; 
		}
		case 'P': { // Print current
			cout << "Printing contents of tab #" << tabNumber << endl;
			myTabs[tabNumber - 1].display();
			break; 
		}
		case 'M': {
			cin.get(blank);
			char toTab;
			cin.get(toTab);
			cout << "Moving tab #" << tabNumber << " before tab #" << toTab << endl;
			break;
		}
		case 'R': {
			cout << "Removing tab #" << tabNumber << endl;
			break;
		}
		case 'C': {
			cin.get(blank);
			i = 0;
			do {
				cin.get(aChar);
				if (aChar != '\n') {
					webAddress[i++] = aChar;
				}
			} while ((aChar != '\n') && (i < 201) && !cin.eof());
			if (i > 0) {
				cout << "Changing address in tab #" << tabNumber << endl;
				// myTabs[tabNumber - 1].addAddress(webAddress);
			}
			break;
		}
		default: { // illegal action 
			cout << "Illegal Action" << endl;
			i = 0;
			do {
				cin.get(aChar);
				if (aChar != '\n') {
					webAddress[i++] = aChar;
				}
			} while ((aChar != '\n') && !cin.eof());
			break;
		}

		}
	}
	return 0;
}