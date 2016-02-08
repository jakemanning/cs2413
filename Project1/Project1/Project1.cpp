#include <iostream>
using namespace std;

class webAddressInfo
{
private:
	char url[201]; //allow a maximum of 200 characters
	// other private methods if necessary for this class
public:
	webAddressInfo();
	webAddressInfo(char* inputString);
	void setWebAddressInfo(char* inputString);
	char* getWebAddressInfo();
	void display();
	// and other public methods if necessary
};

class browserTab {
protected:
	int numAddress; //Current number of web addresses in this tab
	webAddressInfo webAddresses[20]; //Web addresses in this tab
	int currentAddress; //index of current location in webAddresses
						// other private methods if necessary for this class
public:
	browserTab();
	browserTab(char* inputString); //creates a new tab with the inputString
	webAddressInfo& forward();
	webAddressInfo& backward();
	void addAddress(char* inputString);
	void display();
	// and other public methods if necessary
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
	for (int i = 0; url[i] != '\0'; ++i) {
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
		cout << "Already on most current tab; printing: ";
		currentAddress = numAddress - 1;
	}
	return webAddresses[currentAddress];
}

webAddressInfo& browserTab::backward() {
	if (currentAddress > 0) {
		--currentAddress;
		return webAddresses[currentAddress];
	}
	else {
		cout << "Already moved back as far as possible; printing: ";
		currentAddress = 0;
	}
	return webAddresses[currentAddress];
}

void browserTab::addAddress(char* inputString) {
	++numAddress;
	currentAddress = numAddress - 1;
	webAddresses[currentAddress].setWebAddressInfo(inputString);
	webAddresses[currentAddress].display();
}

void browserTab::display() {
	for (int i = 0; i < numAddress; ++i) {
		webAddresses[i].display();
	}
}
#pragma endregion Methods

void strEmpty(char* someString, int length) {
	for (int i = 0; i < length; ++i) {
		someString[i] = '\0';
	}
}

int main()
{
	char command;
	char blank;
	char aChar;
	char webAddress[201];
	browserTab myTabs[20];
	int tabNumber;
	int i;

	// other local variables used to store data temporally
	while (cin >> tabNumber) // while end of line is not reached
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