#include <iostream>
using namespace std;

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
				myTabs[tabNumber - 1].addAddress(webAddress);
			}
			break; }
		case 'F': { // Forward
			cout << "Attempting to move forwards in tab #" << tabNumber << " - ";
			myTabs[tabNumber - 1].forward().display();
			break; }
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
		default: { // illegal action 

			break;
		}

		}
	}
	return 0;
}