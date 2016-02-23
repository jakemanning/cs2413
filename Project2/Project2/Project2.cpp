#include <iostream>
using namespace std;

class Exception { };												// Generic, all exceptions derive from this
class IncorrectAction : public Exception { };						// In case the user asked for an action that won't work
class ArrayException : public Exception { };						// Genric array exception, all array exceptions derive from
class ArrayMemoryException : public ArrayException { };				// In case the array created causes an error
class ArrayBoundsException : public ArrayException { };				// In case the user asked for an index that was out of bounds

#pragma region array
// Purely virtual data type from which arrays derive
template <class DataType>
class AbstractArrayClass {						
public:
	virtual int size() const = 0;									// Abstract template for size
	virtual DataType& operator[] (int k) = 0;						// Abstract template for array index overloading
};
template <class DataType>
class ArrayClass : virtual public AbstractArrayClass<DataType> {	// Encapsulation of a DataType array
protected:
	DataType *paObject;												// Pointer to an array of DataType
	int _size;														// Capacity of the array
	void copy(const ArrayClass<DataType>& ac);						// Allows for a copy constructor to take data from an external ArrayClass
public:
	ArrayClass();													// Default constructor, creates a array of size 1
	ArrayClass(int n);												// Initializer, creates an array of size n
	ArrayClass(int n, const DataType& val);							// Initializer, fills an array of size n with val
	ArrayClass(const ArrayClass<DataType>& ac);						// Copy constructor, transfers data from an external ArrayClass, and copies data into self
	virtual ~ArrayClass();											// Destructor
	virtual int size() const;										// Encapsulated method to access capacity
	virtual DataType& operator [] (int k);							// Overloads bracket operator to access data at index of k
	void operator=(const ArrayClass<DataType>& ac);					// Overloads equals operator to copy information from the given ArrayClass
};

// Purely virtual vector template
template <class DataType>
class AbstractVector : virtual public AbstractArrayClass<DataType> {
public:
	virtual void insert(const DataType& item, int index) = 0; 		// Insert a new object at position index in the vector
	virtual void remove(int index) = 0; 							// Removes the object at position index of the vector
	virtual void add(const DataType& item) = 0; 					// Adds item at end of the vector
};

// Encapsulation of a DataType Vector, allows for dynamically sized array
template <class DataType>
class Vector : virtual public ArrayClass<DataType>, virtual public AbstractVector<DataType> { 
protected:
	int _currSize;													// Active size of the array	
	int _incFactor;													// Index at which the size of the array will be doubled
public:
	Vector();														// Default constructor, calls underlying ArrayClass' default constructor, and sets current size to 0
	Vector(int n);													// Initializer, calls underlying ArrayClass' initializer, and sets increment factor to default
	Vector(int n, DataType& val);									// Initializer, calls underlying ArrayClass' initializer, and sets increment factor to default
	Vector(const Vector<DataType>& v);								// Copy constructor, transfers data from an external Vector, and copies data into self
	Vector(const ArrayClass<DataType>& ac);							// Copy constructor, transfers data from an external ArrayClass, and copies data into underlying Array
	virtual ~Vector();												// Destructor
	void operator= (const Vector<DataType>& v);						// Overloads equals operator to copy information from the given Vector
	void operator= (const ArrayClass<DataType>& ac);				// Overloads equals operator to copy information from the given ArrayClass
	virtual void insert(const DataType& item, int index);			// Shifts all items up to the current array from the given index, and inserts item into given index
	virtual void remove(int index);									// Removes item at given index, shifts down other objects, and decrements active array size
	virtual void add(const DataType& item);							// Appends an object to the underlying Array
	virtual int size() const; 										// Returns size of underlying array
	virtual int capacity() const;									// Returns capacity of underlying array
	virtual int incFactor() const;									// Returns current increment factor
	virtual void setIncFactor(int f);								// Resets the incedent factor to necessary size
	void setCapacity(int c);										// Resizes underlying array to the specified capacity
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

// Encapsulation of a URL string
class webAddressInfo 
{
	friend ostream& operator<< (ostream& s, webAddressInfo& info);	// Overloaded cstream operator, signified as friend so is able to access the info's url
private:
	Vector<char> *url;						// Stores the contents of a URL
public:
	webAddressInfo();						// Default constructor
	webAddressInfo(const Vector<char>& info);			// Initializer constructor, copies contents of supplied vector into underlying vector
	webAddressInfo(const webAddressInfo& info);			// Copy constructor, copies contents of info's underlying url into current vector
	virtual ~webAddressInfo();
	void setWebAddressInfo(const Vector<char>& url);		// Assigns url to given vector
	Vector<char>& getWebAddressInfo();				// Returns the box of the url
	void operator= (const webAddressInfo& info);			// Overloaded equals operator, calls vector copy constructor
};

// Contains any amount of webAddresses, allows output and transitions between urls, as well as removal and changing
class browserTab { 
	friend ostream& operator<< (ostream& s, browserTab& info);		// Overloaded cstream operator, signified as friend so is able to access the underlying webAddress
protected:
	int numAddress;													// Current capacity of web addresses in this tab
	Vector<webAddressInfo> *webAddresses;							// Web addresses in this tab
	int currentAddress;												// index of current location in webAddresses
	int getNumAddress();											// Resets the current capacity integer
	int resetCurrentAddress();										// Resets the current address integer
public:
	browserTab();													// Default constructor
	browserTab(const Vector<char>& inputString);					// Initializer, creates a webAddressInfo object with a url, and adds it to vector
	browserTab(const browserTab& tab);								// Copy constructor, copies contents of supplied tab to current tab
	virtual ~browserTab();											// Destructor
	webAddressInfo& forward();										// Returns 'box' for either the the next url, or the current one if on most recent url
	webAddressInfo& backward();										// Returns 'box' for either the previous url, or the current one if on least recent url
	void addAddress(const Vector<char>& inputString);				// Instantiates a webAddressInfo object and adds it to the current webAddresses vector; sets current index to numAddress - 1, and resets capacity; prints added url
	void changeCurrentAddress(const Vector<char>& newAddress);		// Changes the webAddressInfo object from newAddress to the current index of webAddresses
	void operator= (const browserTab& tab);							// Overloaded equals operator, calls vector copy constructor
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
void webAddressInfo::operator=(const webAddressInfo& info) {
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
		numAddress = getNumAddress();
		currentAddress = resetCurrentAddress();
	}
	catch (ArrayMemoryException memory) {
		cout << "Help, I need somebody" << endl;
	}
}
browserTab::browserTab(const Vector<char>& inputURL) {
	try {
		webAddresses = new Vector<webAddressInfo>(20);
		numAddress = getNumAddress();
		currentAddress = resetCurrentAddress();
		addAddress(inputURL);
	}
	catch (ArrayException memory) {
		cout << "Now you've done it" << endl;
	}
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
int browserTab::resetCurrentAddress() {
	currentAddress = 0;
	if (webAddresses != NULL) {
		currentAddress = (*webAddresses).size() - 1;
	}
	else {
		throw ArrayMemoryException();
	}
	return currentAddress;
}
int browserTab::getNumAddress() {
	numAddress = 0;
	if (webAddresses != NULL) {
		numAddress = (*webAddresses).capacity();
	}
	else {
		throw ArrayMemoryException();
	}
	return numAddress;
}
webAddressInfo& browserTab::forward() {
	if (currentAddress + 1 < getNumAddress()) {
		++currentAddress;
		return (*webAddresses)[currentAddress];
	}
	else {
		resetCurrentAddress(); // In case current address index is somehow greater than or equal to numAddress index
		throw ArrayBoundsException();
	}
	return (*webAddresses)[currentAddress];
}

webAddressInfo& browserTab::backward() {
	if (currentAddress > 0) {
		--currentAddress;
		return (*webAddresses)[currentAddress];
	}
	else {
		if (webAddresses == NULL) { throw ArrayBoundsException(); }
		currentAddress = 0; // In case current address index is somehow less than zero
	}
	return (*webAddresses)[currentAddress];
}
void browserTab::changeCurrentAddress(const Vector<char>& newAddress) {
	webAddressInfo *newUrl = new webAddressInfo(newAddress);
	cout << *newUrl;
	(*webAddresses)[currentAddress] = (*newUrl);
}
void browserTab::addAddress(const Vector<char>& inputURL) {
	webAddressInfo *url = new webAddressInfo(inputURL);
	(*webAddresses).add(*url);
	resetCurrentAddress();
	getNumAddress();
	cout << (*url) << endl;
}
ostream& operator<< (ostream& s, browserTab& info) {
	s << *info.webAddresses;
	return s;
}
void browserTab::operator= (const browserTab& tab) {
	try {
		*webAddresses = (*tab.webAddresses);
		resetCurrentAddress();
		getNumAddress();
	}
	catch (ArrayMemoryException memory) {
		cout << "Ruh-roh" << endl;
	}
}
#pragma endregion Methods

// Removes all characters in a vector
void strEmpty(Vector<char>& str) {
	for (int i = str.size() - 1; i >= 0; --i) {
		str.remove(i);
	}

}

int main()
{
	char command;													// Given command, e.g. New tab, forward, backward, or print
	char blank;														// Offload variable, junk
	char aChar;														// Reads in url to char, for safety
	Vector<char> *webAddress = new Vector<char>(201);				// Vector web address to be wrapped into object
	Vector<browserTab> *myTabs = new Vector<browserTab>(20);		// Creates a browserTab vector with capacity of 20
	int tabNumber;													// Tab number on whic the action will take place
	bool shouldTakeAction;											// Check if the program should follow the command

	// While end of line is not reached
	// Skips blank space like Taylor Swift
	while (cin >> tabNumber)						
	{
		cin.get(blank);
		cin.get(command);
		strEmpty(*webAddress);

		try {
			switch (command) {
			case 'N': { // New url
				cin.get(blank);
				shouldTakeAction = false;
				// Reads given url to the webAddress vector
				do {
					cin.get(aChar);
					if (aChar != '\n') {
						try {
							(*webAddress).add(aChar);
							shouldTakeAction = true;
						}
						catch (ArrayBoundsException bounds) {
							cout << "Pshhhh as if" << endl;
						}
					}
				} while ((aChar != '\n') && !cin.eof());
				if (shouldTakeAction) {
					cout << "Adding address to tab #" << tabNumber << " - ";
					try {
						// Should create a new tab
						if (tabNumber - 1 == (*myTabs).size()) {
							browserTab *tab = new browserTab(*webAddress);
							(*myTabs).add(*tab);
						}
						// Should add a url to an existing tab
						else if (tabNumber - 1 < (*myTabs).size()) {
							(*myTabs)[tabNumber - 1].addAddress(*webAddress);
						}
						// Supplied tab number is out of bounds
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
				try {
					// Should move forward
					if (tabNumber - 1 < (*myTabs).size()) {
						cout << (*myTabs)[tabNumber - 1].forward() << endl;
					}
					// Suplied tab number is out of bounds
					else {
						throw ArrayBoundsException();
					}
				}
				catch (ArrayBoundsException bounds) {
					cout << "Already on most current tab" << endl;
				}
				break;
			}
			case 'B': { // Backward
				cout << "Attempting to move backwards in tab #" << tabNumber << " - ";
				try {
					// Should move backward
					if (tabNumber - 1 < (*myTabs).size()) {
						cout << (*myTabs)[tabNumber - 1].backward() << endl;
					}
					// Supplied tab number is out of bounds
					else {
						throw ArrayBoundsException();
					}

				}
				catch (ArrayException arrayException) {
					cout << "Already moved back as far as possible" << endl;
				}
				break;
			}
			case 'P': { // Print current
				cout << "Printing contents of tab #" << tabNumber << " - ";
				try {
					// Should print tab contents
					if (tabNumber - 1 < (*myTabs).size()) {
						cout << (*myTabs)[tabNumber - 1] << endl;
					}
					// Supplied tab number is out of bounds
					else {
						throw ArrayBoundsException();
					}
				}
				catch (ArrayBoundsException bounds) {
					cout << "Erm...this is embarrassing, it didn't work" << endl;
				}
				break;
			}
			case 'M': {
				int otherTabNumber; // The second tab number
				cin >> otherTabNumber;
				try {
					cout << "Attempting to move tab #" << tabNumber << " before tab #" << otherTabNumber << " - ";
					// Should move tabNumber before otherTabNumber
					if (tabNumber > otherTabNumber) {
						browserTab info = (*myTabs)[tabNumber];
						(*myTabs).insert(info, otherTabNumber);
					}
					else {
						cout << "Tab is already before other tab";
					}
				}
				catch (ArrayException) {
					cout << "Incorrect Access";
				}
				cout << endl;
				break;
			}
			case 'R': {
				cout << "Attempting to remove tab #" << tabNumber << " - ";
				try {
					// Should remove tab
					if (tabNumber - 1 < (*myTabs).size()) {
						cout << (*myTabs)[tabNumber - 1];
						(*myTabs).remove(tabNumber - 1);
					}
					// Supplied tab number is out of bounds
					else {
						throw ArrayBoundsException();
					}
				}
				catch (ArrayBoundsException bounds) {
					cout << "You're out of bounds";
				}
				cout << endl;
				break;
			}
			case 'C': {
				cin.get(blank);
				shouldTakeAction = false;
				do {
					// Reads given url into the webAddressInfo vector
					cin.get(aChar);
					if (aChar != '\n') {
						try {
							shouldTakeAction = true;
							(*webAddress).add(aChar);
						}
						catch (ArrayBoundsException outOfBounds) {
							cout << "You so sillyyy" << endl;
						}
					}
				} while ((aChar != '\n') && !cin.eof());
				if (shouldTakeAction) {
					cout << "Changing the current address in tab #" << tabNumber << " - ";
					try {
						// Should change currentAddress
						if (tabNumber - 1 < (*myTabs).size()) {
							(*myTabs)[tabNumber - 1].changeCurrentAddress(*webAddress);
						}
						// Supplied tab number is out of bounds
						else {
							throw ArrayBoundsException();
						}
					}
					catch (ArrayBoundsException bounds) {
						cout << "You're out of bounds";
					}
					cout << endl;
				}
				break;
			}
			default: { // illegal action 
				// Move to end of line in case extra information
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
