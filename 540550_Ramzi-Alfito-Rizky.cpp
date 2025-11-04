#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Member;
class Shelf;

class Slot {
private:
    int index;
    string placedItemId;
    
public:
    Slot(int idx) : index(idx), placedItemId("") {}
    
    bool isEmpty() const { return placedItemId.empty(); }
    void placeItem(string itemId) { placedItemId = itemId; }
    void removeItem() { placedItemId = ""; }
    string getPlacedItemId() const { return placedItemId; }
};

class Item {
protected:
    string id;
    string title;
    string author;
    Shelf* placedOnShelf;
    
public:
    Item(string itemId, string itemTitle, string itemAuthor) 
        : id(itemId), title(itemTitle), author(itemAuthor), placedOnShelf(nullptr) {}
    
    virtual ~Item() {}
    
    string getId() const { return id; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    Shelf* getPlacedOnShelf() const { return placedOnShelf; }
    
    void setPlacedOnShelf(Shelf* shelf) { placedOnShelf = shelf; }
    void removeFromShelf() { placedOnShelf = nullptr; }
    
    virtual void showInfo() = 0;
    virtual string getType() const = 0;
};

class Book : public Item {
private:
    int pages;
    
public:
    Book(string id, string title, string author, int pageCount) 
        : Item(id, title, author), pages(pageCount) {}
    
    void showInfo() override {
        cout << "ID: " << id << ", Title: " << title 
             << ", Author: " << author << ", Pages: " << pages << endl;
    }
    
    string getType() const override { return "Book"; }
};

class Magazine : public Item {
private:
    string publisher;
    int issueNumber;
    
public:
    Magazine(string id, string title, string pub, int issue) 
        : Item(id, title, pub), publisher(pub), issueNumber(issue) {}
    
    void showInfo() override {
        cout << "ID: " << id << ", Title: " << title 
             << ", Publisher: " << publisher << ", Issue: " << issueNumber << endl;
    }
    
    string getType() const override { return "Magazine"; }
    
    string getPublisher() const { return publisher; }
};

class Shelf {
private:
    string name;
    int index;
    vector<Slot*> slots;
    
public:
    Shelf(string shelfName, int shelfIndex, int capacity) 
        : name(shelfName), index(shelfIndex) {
        for (int i = 0; i < capacity; i++) {
            slots.push_back(new Slot(i));
        }
    }
    
    ~Shelf() {
        for (Slot* slot : slots) delete slot;
    }
    
    bool placeItem(string itemId) {
        for (Slot* slot : slots) {
            if (slot->isEmpty()) {
                slot->placeItem(itemId);
                return true;
            }
        }
        return false;
    }
    
    bool removeItem(string itemId) {
        for (Slot* slot : slots) {
            if (slot->getPlacedItemId() == itemId) {
                slot->removeItem();
                return true;
            }
        }
        return false;
    }
    
    int emptyCount() const {
        int count = 0;
        for (Slot* slot : slots) {
            if (slot->isEmpty()) count++;
        }
        return count;
    }
    
    vector<string> getPlacedItemIds() const {
        vector<string> itemIds;
        for (Slot* slot : slots) {
            if (!slot->isEmpty()) {
                itemIds.push_back(slot->getPlacedItemId());
            }
        }
        return itemIds;
    }
    
    int getCapacity() const { return slots.size(); }
    string getName() const { return name; }
};

class Member {
private:
    string memberId;
    string name;
    string alias;
    vector<Item*> borrowedItems;
    
public:
    Member(string id, string memberName, string memberAlias) 
        : memberId(id), name(memberName), alias(memberAlias) {}
    
    string getName() const { return name; }
    string getAlias() const { return alias; }
    
    bool borrowItem(Item* item) {
        borrowedItems.push_back(item);
        return true;
    }
    
    bool returnItem(Item* item) {
        auto it = find(borrowedItems.begin(), borrowedItems.end(), item);
        if (it != borrowedItems.end()) {
            borrowedItems.erase(it);
            return true;
        }
        return false;
    }
    
    vector<Item*> getBorrowedItems() const { return borrowedItems; }
};

class Library {
private:
    vector<Item*> items;
    vector<Shelf*> shelves;
    
public:
    ~Library() {
        for (Item* item : items) delete item;
        for (Shelf* shelf : shelves) delete shelf;
    }
    
    void addBook(string id, string title, string author, int pages) {
        items.push_back(new Book(id, title, author, pages));
        cout << "Book '" << title << "' added to the library." << endl;
    }
    
    void addMagazine(string id, string title, string publisher, int issueNumber) {
        items.push_back(new Magazine(id, title, publisher, issueNumber));
        cout << "Magazine '" << title << "' added to the library." << endl;
    }
    
    void addShelf(string name, int capacity) {
        shelves.push_back(new Shelf(name, shelves.size(), capacity));
        cout << "Shelf '" << name << "' with capacity " << capacity << " added." << endl;
    }
    
    bool placeItemOnShelf(string itemId, int shelfIdx) {
        Item* item = findById(itemId);
        if (item && shelfIdx >= 0 && shelfIdx < shelves.size()) {
            if (shelves[shelfIdx]->placeItem(itemId)) {
                item->setPlacedOnShelf(shelves[shelfIdx]);
                cout << "Item '" << item->getTitle() << "' successfully placed on shelf '" 
                     << shelves[shelfIdx]->getName() << "'." << endl;
                return true;
            }
        }
        return false;
    }
    
    Item* findByTitle(string title) {
        for (Item* item : items) {
            if (item->getTitle() == title) return item;
        }
        return nullptr;
    }
    
    Item* findById(string id) {
        for (Item* item : items) {
            if (item->getId() == id) return item;
        }
        return nullptr;
    }
    
    bool removeItemFromShelf(string itemId) {
        Item* item = findById(itemId);
        if (item && item->getPlacedOnShelf()) {
            Shelf* shelf = item->getPlacedOnShelf();
            if (shelf->removeItem(itemId)) {
                item->removeFromShelf();
                return true;
            }
        }
        return false;
    }
    
    bool returnItemToShelf(Item* item, int shelfIdx) {
        if (shelfIdx >= 0 && shelfIdx < shelves.size()) {
            if (shelves[shelfIdx]->placeItem(item->getId())) {
                item->setPlacedOnShelf(shelves[shelfIdx]);
                return true;
            }
        }
        return false;
    }
    
    int getTotalItems() const { return items.size(); }
    vector<Shelf*> getShelves() const { return shelves; }
    vector<Item*> getItems() const { return items; }
};

int main() {
    // 1. Setup Library
    cout << "1. Setup Library" << endl;
    Library* library = new Library();
    
    library->addShelf("Shelf A", 5);
    library->addShelf("Shelf B", 4);
    library->addShelf("Shelf C", 3);
    
    cout << "Adding 4 Books..." << endl;
    library->addBook("Book 1", "Harry Potter and the Philosopher's Stone", "J.K. Rowling", 223);
    library->addBook("Book 2", "The Hobbit", "J.R.R. Tolkien", 310);
    library->addBook("Book 3", "To Kill a Mockingbird", "Harper Lee", 281);
    library->addBook("Book 4", "The Great Gatsby", "F. Scott Fitzgerald", 180);
    
    cout << "Adding 6 Magazines..." << endl;
    library->addMagazine("Magazine 1", "Forbes Indonesia", "Forbes Media", 245);
    library->addMagazine("Magazine 2", "Kompas", "Kompas Gramedia", 312);
    library->addMagazine("Magazine 3", "Reader's Digest", "Trusted Media", 178);
    library->addMagazine("Magazine 4", "Elle Indonesia", "Hearst Magazine", 189);
    library->addMagazine("Magazine 5", "GQ Indonesia", "Conde Nast", 156);
    library->addMagazine("Magazine 6", "Business Week", "Bloomberg", 203);
    
    // 2. Penempatan
    cout << "\n2. Penempatan" << endl;
    library->placeItemOnShelf("Book 1", 0);
    library->placeItemOnShelf("Book 2", 0);
    library->placeItemOnShelf("Book 3", 0);
    library->placeItemOnShelf("Book 4", 0);
    library->placeItemOnShelf("Magazine 1", 1);
    library->placeItemOnShelf("Magazine 2", 1);
    library->placeItemOnShelf("Magazine 3", 1);
    library->placeItemOnShelf("Magazine 4", 1);
    library->placeItemOnShelf("Magazine 5", 2);
    library->placeItemOnShelf("Magazine 6", 2);
    
    cout << "Checking empty slots :" << endl;
    for (Shelf* shelf : library->getShelves()) {
        cout << "Shelf '" << shelf->getName() << "' empty slots: " 
             << shelf->emptyCount() << endl;
    }
    
    // 3. Pencarian
    cout << "\n3. Pencarian" << endl;
    
    cout << "Searching ID:  'Book 3'..." << endl;
    Item* found = library->findById("Book 3");
    if (found) {
        found->showInfo();
    }
    
    cout << "Searching Title:  'The Great Gatsby'..." << endl;
    found = library->findByTitle("The Great Gatsby");
    if (found) {
        found->showInfo();
    }
    
    cout << "Searching ID:  'Book 750'..." << endl;
    found = library->findById("Book 750");
    if (found) {
        found->showInfo();
    } else {
        cout << "Item 'Book 750' not found." << endl;
    }
    
    // 4. Peminjaman dan Pengembalian
    cout << "\n4. Peminjaman dan Pengembalian" << endl;
    Member* memberA = new Member("M001", "Member A", "Member A");
    Member* memberB = new Member("M002", "Member B", "Member B");
    cout << "Member Member A dan Member B created." << endl;
    
    Item* book1 = library->findById("Book 1");
    Item* book2 = library->findById("Book 2");
    
    cout << "Member A borrows two books, Harry Potter and the Philosopher's Stone and The Hobbit" << endl;
    if (book1 && book1->getPlacedOnShelf()) {
        library->removeItemFromShelf(book1->getId());
        memberA->borrowItem(book1);
        cout << "Borrowing succeeded: Member A borrowed '" << book1->getTitle() << "'." << endl;
    }
    
    if (book2 && book2->getPlacedOnShelf()) {
        library->removeItemFromShelf(book2->getId());
        memberA->borrowItem(book2);
        cout << "Borrowing succeeded: Member A borrowed '" << book2->getTitle() << "'." << endl;
    }
    
    cout << "Member B borrows the same book, Harry Potter and the Philosopher's Stone" << endl;
    if (book1 && !book1->getPlacedOnShelf()) {
        cout << "Searching failed: item Book 1 is not on any shelf." << endl;
    }
    
    cout << "Member A returns one book (Harry Potter and the Philosopher's Stone)" << endl;
    if (book1) {
        memberA->returnItem(book1);
        library->returnItemToShelf(book1, 0);
        cout << "Item '" << book1->getTitle() << "' successfully placed on shelf 'Shelf A'." << endl;
        cout << "Returning succeeded: Item Book 1 returned to shelf 'Shelf A'." << endl;
    }
    
    cout << "Member B borrows the book again (Harry Potter and the Philosopher's Stone)" << endl;
    if (book1 && book1->getPlacedOnShelf()) {
        library->removeItemFromShelf(book1->getId());
        memberB->borrowItem(book1);
        cout << "Borrowing succeeded: Member B borrowed '" << book1->getTitle() << "'." << endl;
    }
    
    cout << "Final loan lists:" << endl;
    cout << "List loan for Member A: " << endl;
    for (Item* item : memberA->getBorrowedItems()) {
        item->showInfo();
    }
    
    cout << "List loan for Member B: " << endl;
    for (Item* item : memberB->getBorrowedItems()) {
        item->showInfo();
    }
    
    // 5. Final Summary
    cout << "\n5. Final Summary" << endl;
    cout << "Library Summary:" << endl;
    cout << "Total Item in Library: " << library->getTotalItems() << endl;
    
    cout << "Shelf status:" << endl;
    for (Shelf* shelf : library->getShelves()) {
        cout << "Shelf Summary: " << shelf->getName() << endl;
        cout << "Capacity: " << shelf->getCapacity() << " slots" << endl;
        cout << "Empty Slots: " << shelf->emptyCount() << " slots" << endl;
        cout << "Item List:" << endl;
        
        for (string itemId : shelf->getPlacedItemIds()) {
            Item* item = library->findById(itemId);
            if (item) item->showInfo();
        }
    }
    
    cout << "Borrowed Item(s):" << endl;
    for (Item* item : library->getItems()) {
        if (!item->getPlacedOnShelf()) {
            item->showInfo();
        }
    }
    
    cout << "Report finished." << endl;

    return 0;
}