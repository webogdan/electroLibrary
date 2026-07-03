#include "head.h"
#include <fstream>
#include <sstream>

using namespace std;

// ------------------------------------------------------------
// Небольшая утилита: разбить строку по разделителю.
// Нужна для чтения файла (сериализация Item в текстовый формат).
// ------------------------------------------------------------
static vector<string> splitString(const string& s, char delimiter) {
	vector<string> parts;
	stringstream ss(s);
	string part;
	while (getline(ss, part, delimiter)) {
		parts.push_back(part);
	}
	return parts;
}

//============================================
// Item

int Item::getYear() const { return year; }
string Item::getTitle() const { return title; }
bool Item::getIsAvailable() const { return isAvailable; }

void Item::setYear(int newYear) { year = newYear; }
void Item::setTitle(const string& newTitle) { title = newTitle; }
void Item::setIsAvailable(bool newIsAvailable) { isAvailable = newIsAvailable; }
//============================================


//========================================================================
// Book

string Book::getAuthor() const { return author; }
string Book::getGenre() const { return genre; }

void Book::setAuthor(const string& newAuthor) { author = newAuthor; }
void Book::setGenre(const string& newGenre) { genre = newGenre; }

void Book::print(ostream& os) const {
	os << "[Book] Year: " << getYear() << endl
		<< "Title: " << getTitle() << endl
		<< "Available: " << (getIsAvailable() ? "Yes" : "No") << endl
		<< "Author: " << getAuthor() << endl
		<< "Genre: " << getGenre() << endl;
}

string Book::getType() const { return "BOOK"; }

string Book::serialize() const {
	// BOOK|year|title|isAvailable|author|genre
	ostringstream oss;
	oss << "BOOK|" << getYear() << "|" << getTitle() << "|"
		<< (getIsAvailable() ? 1 : 0) << "|" << getAuthor() << "|" << getGenre();
	return oss.str();
}
//========================================================================

//===========================================================================
// Journal

int Journal::getIssueNum() const { return issueNum; }
string Journal::getPublisher() const { return publisher; }

void Journal::setIssueNum(int newIssueNum) { issueNum = newIssueNum; }
void Journal::setPublisher(const string& newPublisher) { publisher = newPublisher; }

void Journal::print(ostream& os) const {
	os << "[Journal] Year: " << getYear() << endl
		<< "Title: " << getTitle() << endl
		<< "Available: " << (getIsAvailable() ? "Yes" : "No") << endl
		<< "Issue number: #" << getIssueNum() << endl
		<< "Publisher: " << getPublisher() << endl;
}

string Journal::getType() const { return "JOURNAL"; }

string Journal::serialize() const {
	// JOURNAL|year|title|isAvailable|issueNum|publisher
	ostringstream oss;
	oss << "JOURNAL|" << getYear() << "|" << getTitle() << "|"
		<< (getIsAvailable() ? 1 : 0) << "|" << getIssueNum() << "|" << getPublisher();
	return oss.str();
}
//===========================================================================


//===============================================================
// User

int User::getUserId() const { return userId; }
string User::getName() const { return name; }
int User::getBorrowedItemsCount() const { return static_cast<int>(borrowedTitles.size()); }
const vector<string>& User::getBorrowedTitles() const { return borrowedTitles; }

void User::setUserId(int newUserId) { userId = newUserId; }
void User::setName(const string& newName) { name = newName; }

void User::addBorrowedTitle(const string& title) {
	borrowedTitles.push_back(title);
}

bool User::removeBorrowedTitle(const string& title) {
	auto it = find(borrowedTitles.begin(), borrowedTitles.end(), title);
	if (it != borrowedTitles.end()) {
		borrowedTitles.erase(it);
		return true;
	}
	return false;
}

ostream& operator<<(ostream& os, const User& user) {
	os << "Id: " << user.getUserId() << endl
		<< "Name: " << user.getName() << endl
		<< "Borrowed items (" << user.getBorrowedItemsCount() << "): ";
	if (user.borrowedTitles.empty()) {
		os << "none" << endl;
	}
	else {
		for (size_t i = 0; i < user.borrowedTitles.size(); ++i) {
			os << user.borrowedTitles[i];
			if (i + 1 < user.borrowedTitles.size()) os << ", ";
		}
		os << endl;
	}
	return os;
}
//===============================================================


//===========================================================================
// Library

void Library::addItem(unique_ptr<Item> item) {
	cout << item->getType() << " \"" << item->getTitle() << "\" was successfully added to the library!" << endl;
	items.push_back(move(item));
}

bool Library::removeItem(const string& title) {
	auto it = find_if(items.begin(), items.end(), [&](const unique_ptr<Item>& i) {
		return i->getTitle() == title;
		});
	if (it != items.end()) {
		items.erase(it);
		cout << "\"" << title << "\" was successfully removed from the library!" << endl;
		return true;
	}
	cout << "There is no such item in the library" << endl;
	return false;
}

void Library::printAllItems() const {
	if (items.empty()) {
		cout << "The library is empty" << endl;
		return;
	}
	for (const auto& item : items) {
		cout << *item << endl; // полиморфный вызов print() через operator<<
	}
}

Item* Library::findItem(const string& title) {
	auto it = find_if(items.begin(), items.end(), [&](const unique_ptr<Item>& i) {
		return i->getTitle() == title;
		});
	return (it != items.end()) ? it->get() : nullptr;
}

bool Library::borrowItem(User& user, const string& title) {
	Item* item = findItem(title);
	if (!item) {
		cout << "There is no such item in the library" << endl;
		return false;
	}
	if (!item->getIsAvailable()) {
		cout << "\"" << title << "\" is already borrowed" << endl;
		return false;
	}
	item->setIsAvailable(false);
	user.addBorrowedTitle(title);
	cout << user.getName() << " successfully borrowed \"" << title << "\"" << endl;
	return true;
}

bool Library::returnItem(User& user, const string& title) {
	Item* item = findItem(title);
	if (!item) {
		cout << "There is no such item in the library" << endl;
		return false;
	}
	if (!user.removeBorrowedTitle(title)) {
		cout << user.getName() << " does not have \"" << title << "\" borrowed" << endl;
		return false;
	}
	item->setIsAvailable(true);
	cout << user.getName() << " successfully returned \"" << title << "\"" << endl;
	return true;
}

void Library::saveToFile(const string& filename) const {
	ofstream file(filename);
	if (!file.is_open()) {
		cout << "Could not open file for writing: " << filename << endl;
		return;
	}
	for (const auto& item : items) {
		file << item->serialize() << "\n";
	}
	cout << "Library saved to \"" << filename << "\"" << endl;
}

void Library::loadFromFile(const string& filename) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Could not open file for reading: " << filename << endl;
		return;
	}

	items.clear();
	string line;
	while (getline(file, line)) {
		if (line.empty()) continue;
		vector<string> parts = splitString(line, '|');
		if (parts.empty()) continue;

		if (parts[0] == "BOOK" && parts.size() == 6) {
			int year = stoi(parts[1]);
			string title = parts[2];
			bool available = (parts[3] == "1");
			string author = parts[4];
			string genre = parts[5];
			items.push_back(make_unique<Book>(year, title, available, author, genre));
		}
		else if (parts[0] == "JOURNAL" && parts.size() == 6) {
			int year = stoi(parts[1]);
			string title = parts[2];
			bool available = (parts[3] == "1");
			int issueNum = stoi(parts[4]);
			string publisher = parts[5];
			items.push_back(make_unique<Journal>(year, title, available, issueNum, publisher));
		}
	}
	cout << "Library loaded from \"" << filename << "\"" << endl;
}
//===========================================================================


int main() {
	Library library;
	User user(1, "Bogdan");

	library.addItem(make_unique<Book>(1972, "Green Mile", true, "Stephen King", "Thriller"));
	library.addItem(make_unique<Journal>(1990, "Iron Man", true, 1, "Marvel Studios"));

	cout << endl << "--- All items ---" << endl;
	library.printAllItems();

	cout << endl << "--- Borrowing ---" << endl;
	library.borrowItem(user, "Green Mile");

	cout << endl << "--- User info ---" << endl;
	cout << user;

	cout << endl << "--- Save & reload ---" << endl;
	library.saveToFile("library.txt");
	library.loadFromFile("library.txt");

	cout << endl << "--- All items after reload ---" << endl;
	library.printAllItems();

	cout << endl << "--- Returning ---" << endl;
	library.returnItem(user, "Green Mile");
	cout << user;
}
