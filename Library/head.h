#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <string>

// ============================================================
// Item — абстрактный базовый класс.
// Нельзя создать "просто Item" — только Book или Journal.
// ============================================================
class Item {
protected:
	int year;
	std::string title;
	bool isAvailable;
public:
	Item(int _year, const std::string& _title, bool _isAvailable) :
		year(_year), title(_title), isAvailable(_isAvailable) {
	}
	virtual ~Item() = default;

	int getYear() const;
	std::string getTitle() const;
	bool getIsAvailable() const;

	void setYear(int newYear);
	void setTitle(const std::string& newTitle);
	void setIsAvailable(bool newIsAvailable);

	// Чисто виртуальные — каждый наследник обязан их реализовать.
	virtual void print(std::ostream& os) const = 0;
	virtual std::string getType() const = 0;      // "BOOK" / "JOURNAL" — нужно для сохранения в файл
	virtual std::string serialize() const = 0;     // строка для записи в файл

	friend std::ostream& operator<<(std::ostream& os, const Item& item) {
		item.print(os);
		return os;
	}
};

class Book : public Item {
private:
	std::string author;
	std::string genre;
public:
	Book(int _year, const std::string& _title, bool _isAvailable, const std::string& _author, const std::string& _genre) :
		Item(_year, _title, _isAvailable), author(_author), genre(_genre) {
	}

	std::string getAuthor() const;
	std::string getGenre() const;

	void setAuthor(const std::string& newAuthor);
	void setGenre(const std::string& newGenre);

	void print(std::ostream& os) const override;
	std::string getType() const override;
	std::string serialize() const override;
};

class Journal : public Item {
private:
	int issueNum;
	std::string publisher;
public:
	Journal(int _year, const std::string& _title, bool _isAvailable, int _issueNum, const std::string& _publisher) :
		Item(_year, _title, _isAvailable), issueNum(_issueNum), publisher(_publisher) {
	}

	int getIssueNum() const;
	std::string getPublisher() const;

	void setIssueNum(int newIssueNum);
	void setPublisher(const std::string& newPublisher);

	void print(std::ostream& os) const override;
	std::string getType() const override;
	std::string serialize() const override;
};

// ============================================================
// User — теперь реально хранит список названий книг на руках,
// а не просто число.
// ============================================================
class User {
private:
	int userId;
	std::string name;
	std::vector<std::string> borrowedTitles;
public:
	User(int _userId, const std::string& _name) :
		userId(_userId), name(_name) {
	}

	int getUserId() const;
	std::string getName() const;
	int getBorrowedItemsCount() const;
	const std::vector<std::string>& getBorrowedTitles() const;

	void setUserId(int newUserId);
	void setName(const std::string& newName);

	void addBorrowedTitle(const std::string& title);
	bool removeBorrowedTitle(const std::string& title);

	friend std::ostream& operator<<(std::ostream& os, const User& user);
};

// ============================================================
// Library — единое полиморфное хранилище всех Item.
// ============================================================
class Library {
private:
	std::vector<std::unique_ptr<Item>> items;

public:
	void addItem(std::unique_ptr<Item> item);
	bool removeItem(const std::string& title);
	void printAllItems() const;

	Item* findItem(const std::string& title);

	bool borrowItem(User& user, const std::string& title);
	bool returnItem(User& user, const std::string& title);

	void saveToFile(const std::string& filename) const;
	void loadFromFile(const std::string& filename);
};
