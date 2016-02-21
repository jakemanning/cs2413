#include <iostream>
using namespace std;

class Exception { };
class IncorrectAction : public Exception { };
class ArrayException : public Exception { };
class ArrayMemoryException : public ArrayException { };
class ArrayBoundsException : public ArrayException { };

#pragma region array
template <class DataType>
class AbstractArrayClass {
public:
	virtual int size() const = 0;
	virtual DataType& operator[] (int k) = 0;
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
	virtual void insert(const DataType& item, int index) = 0; // insert a new object at position index in the vector
	virtual void remove(int index) = 0; // removes the object at position index of the vector
	virtual void add(const DataType& item) = 0; // adds item at end of the vector
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
	friend ostream& operator<< (ostream& s, webAddressInfo& info);
private:
	Vector<char> *url;								// allow a maximum of 200 characters
public:
	webAddressInfo();								// empty constructor; automatically called by statically defined variable in browserTab class
	webAddressInfo(const Vector<char>& info);				// never called in project, due to the fact all URLs are statically instantiated
	webAddressInfo(const webAddressInfo& info);
	virtual ~webAddressInfo();
	void setWebAddressInfo(const Vector<char>& url);		// reads in the input string to url, the extraneous space is already filled with string terminators
	Vector<char>& getWebAddressInfo();						// returns a pointer to the first element of url
	void display();									// displays full url to the console, stops on string terminator
	void operator= (const webAddressInfo info);
};

/* Contains up to twenty url objects; allows output and transitions between urls*/
class browserTab {
	friend ostream& operator<< (ostream& s, browserTab& info);
protected:
	int numAddress;									// Current number of web addresses in this tab
	Vector<webAddressInfo> *webAddresses;			// Web addresses in this tab
	int currentAddress;								// index of current location in webAddresses
public:
	browserTab();									// empty constructor; automatically called by statically defined variable in main class
	browserTab(const Vector<char>& inputString);					// creates a new tab with the inputString; never called in project due to static instantiation
	browserTab(const browserTab& tab);
	virtual ~browserTab();
	webAddressInfo& forward();						// returns the 'box' for either the the next url, or the current one if on most recent url
	webAddressInfo& backward();						// returns the 'box' for either the previous url, or the current one if on least recent url
	void addAddress(const Vector<char>& inputString);				// creates url (webAddressInfo), sets current index to numAddress - 1, becuase numAddress size is not zero-indexed, prints url
	void display();									// displays each webAddressInfo url in the browserTab
};

#pragma region webAddressInfo
webAddressInfo::webAddressInfo() {
	try {
		url = new Vector<char>(201);
	}
	catch (ArrayMemoryException memory) {
		cout << "Jinkies!" << endl;
	}
}
webAddressInfo::webAddressInfo(const Vector<char>& newUrl) {
	try {
		url = new Vector<char>(newUrl);
	}
	catch (ArrayMemoryException memory) {
		cout << "Gadzooks!" << endl;
	}
}
webAddressInfo::webAddressInfo(const webAddressInfo& info) {
	try {
		url = info.url;
	}
	catch (ArrayMemoryException memory) {
		cout << "Oh no!" << endl;
	}
}
webAddressInfo::~webAddressInfo() {
	if (url != NULL) {}
}
void webAddressInfo::setWebAddressInfo(const Vector<char>& newUrl) {
	try {
		if (url != NULL) {
			*url = newUrl;
		}
	}
	catch (ArrayMemoryException memory) {
		cout << "You've lost your marbles!" << endl;
	}
}
Vector<char>& webAddressInfo::getWebAddressInfo() {
	return *url;
}
/*
void webAddressInfo::display() {

}*/
void webAddressInfo::operator=(const webAddressInfo info) {
	try {
		url = info.url;
	}
	catch (ArrayMemoryException memory) {
		cout << "Ruh-roh" << endl;
	}
}
ostream& operator<< (ostream& s, webAddressInfo& info) {
	try {
		for (int i = 0; i < info.getWebAddressInfo().size(); ++i) {
			s << info.getWebAddressInfo()[i];
		}
	}
	catch (ArrayBoundsException bounds) {
		s << "You've crossed a line";
	}
	return s;
}
#pragma endregion Methods

#pragma region browserTab
browserTab::browserTab() {
	try {
		webAddresses = new Vector<webAddressInfo>(20);
	}
	catch (ArrayMemoryException memory) {
		cout << "Help, I need somebody" << endl;
	}
	numAddress = (*webAddresses).capacity();
	currentAddress = (*webAddresses).size();
}
browserTab::browserTab(const Vector<char>& inputURL) {
	try {
		webAddresses = new Vector<webAddressInfo>(20);
	}
	catch (ArrayMemoryException memory) {
		cout << "Now you've done it" << endl;
	}
	numAddress = 0; // default in case fails
	currentAddress = 0; // default in case fails
	addAddress(inputURL);
}
browserTab::browserTab(const browserTab& info) {
	webAddresses = info.webAddresses;
	numAddress = (*info.webAddresses).capacity();
	currentAddress = (*info.webAddresses).size() - 1;
}
browserTab::~browserTab() {
	if (webAddresses != NULL) { delete webAddresses; }
	numAddress = 0;
	currentAddress = 0;
}
//webAddressInfo& browserTab::forward() {
//	/*if (currentAddress + 1 < numAddress) {
//		++currentAddress;
//		return webAddresses[currentAddress];
//	}
//	else {
//		cout << "Already on most current tab; printing - ";
//		currentAddress = numAddress - 1;			// in case current address index is somehow greater than or equal to numAddress index
//	}
//	return webAddresses[currentAddress];*/
//}
//webAddressInfo& browserTab::backward() {
//	//if (currentAddress > 0) {
//	//	--currentAddress;
//	//	return webAddresses[currentAddress];
//	//}
//	//else {
//	//	cout << "Already moved back as far as possible; printing - ";
//	//	currentAddress = 0;							// in case current address index is somehow less than zero
//	//}
//	//return webAddresses[currentAddress];
//}
void browserTab::addAddress(const Vector<char>& inputURL) {
	try {
		webAddressInfo *url = new webAddressInfo(inputURL);
		(*webAddresses).add(*url);
		numAddress = (*webAddresses).capacity();
		currentAddress = (*webAddresses).size() - 1;

	}
	catch (ArrayException arrayException) {
		cout << "I can't do this at all " << endl;
	}
}
/*
void browserTab::display() {

}*/
ostream& operator<< (ostream& s, browserTab& info) {
	s << *info.webAddresses;
	return s;
}
#pragma endregion Methods

// sets each character in a string for a given length to string terminators
void strEmpty(Vector<char>& str) {
	for (int i = str.size() - 1; i >= 0; --i) {
		str.remove(i);
	}

}

int main()
{
	char command;									// the given command, e.g. New tab, forward, backward, or print
	char blank;										// offload variable, junk
	char aChar;										// reads in url to char, for safety
	Vector<char> *webAddress = new Vector<char>(201);					// the web address to be wrapped into object
	Vector<browserTab> *myTabs = new Vector<browserTab>(20);					// statically creates 20 tabs, which statically creates 400 webAddressInfo objects
	int tabNumber;									// the browserTab object to manipulate
	int i;											// loop variable

	/*
	(*webAddress).add('a');
	(*webAddress).add('a');
	(*webAddress).add('a');
	(*webAddress).add('a');
	(*webAddress).add('a');

	Vector<char> *otherWebAddress = new Vector<char>(201);
	(*otherWebAddress).add('b');
	(*otherWebAddress).add('b');
	(*otherWebAddress).add('b');
	(*otherWebAddress).add('b');
	(*otherWebAddress).add('b');

	browserTab *tabTest = new browserTab(*webAddress);*/



	while (cin >> tabNumber)						// while end of line is not reached
	{
		cin.get(blank);
		cin.get(command);
		strEmpty(*webAddress);

		try {
			switch (command) {
			case 'N': { // New url
				cin.get(blank);
				i = 0;
				do {
					cin.get(aChar);
					if (aChar != '\n') {
						try {
							i = 1;
							(*webAddress).add(aChar);
						}
						catch (ArrayBoundsException bounds) {
							cout << "Pshhhh as if" << endl;
						}
					}
				} while ((aChar != '\n') && !cin.eof());
				if (i > 0) {
					cout << "Adding address to tab #" << tabNumber;
					try {
						if (tabNumber - 1 == (*myTabs).size()) {
							browserTab *tab = new browserTab(*webAddress);
							(*myTabs).add(*tab);
							cout << (*myTabs)[tabNumber - 1] << endl;
						}
						else if (tabNumber - 1 < (*myTabs).size()) {
							(*myTabs)[tabNumber - 1].addAddress(*webAddress);
							cout << (*myTabs)[tabNumber - 1] << endl;
						}
						else {
							throw ArrayBoundsException();
						}
					}
					catch (ArrayBoundsException bounds) {
						cout << "Uh...no" << endl;
					}
				}
				break; }
			case 'F': { // Forward
				cout << "Attempting to move forwards in tab #" << tabNumber << " - ";
				/*myTabs[tabNumber - 1].forward().display();*/
				break;
			}
			case 'B': { // Backward
				cout << "Attempting to move backwards in tab #" << tabNumber << " - ";
				/*myTabs[tabNumber - 1].backward().display();*/
				break;
			}
			case 'P': { // Print current
				cout << "Printing contents of tab #" << tabNumber << endl;
				//myTabs[tabNumber - 1].display();
				break;
			}
			case 'M': {
				cin.get(blank);
				cin.get(aChar);
				cout << "Moving tab #" << tabNumber << " before tab #" << aChar << endl;
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
						try {
							i = 1;
							(*webAddress).add(aChar);
						}
						catch (ArrayBoundsException outOfBounds) {
							cout << "You so sillyyy" << endl;
						}
					}
				} while ((aChar != '\n') && !cin.eof());
				if (i > 0) {
					cout << "Changing the current address in tab #" << tabNumber << endl;
					// TODO: something
				}
				break;
			}
			default: { // illegal action 
				i = 0;
				do {
					cin.get(aChar);
					if (aChar != '\n') {
						try {
							(*webAddress).add(aChar);
						}
						catch (ArrayBoundsException bounds) {
							cout << "Alohamora" << endl;
						}
					}
				} while ((aChar != '\n') && !cin.eof());
				throw IncorrectAction();
				break;
			}
			}
		}
		catch (IncorrectAction incorrect) {
			cout << "Illegal Action" << endl;
		}
	}

	return 0;
}