#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <cstdlib>
#include <map>
#include <conio.h>
#include <windows.h>

using namespace std;

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void showMainMenu() {
    system("cls");
    setColor(11);
    cout << "==========================================\n";
    cout << "         DIGITAL LIBRARY SYSTEM\n";
    cout << "==========================================\n";
    setColor(14);
    cout << "  [1] Login\n";
    cout << "  [2] Register\n";
    cout << "  [3] Exit\n";
    setColor(8);
    cout << "==========================================\n";
    setColor(7);
    cout << "Choose option: ";
}

void showUserMenu() {
    setColor(10);
    cout << "============== USER MENU ================\n";
    cout << "  [1] Return Book\n";
    cout << "  [2] Borrow Book\n";
    cout << "  [3] View My Loans\n";
    cout << "  [4] Search Books\n";
    cout << "  [5] Exit\n";
    setColor(8);
    cout << "==========================================\n";
    setColor(7);
}

void showAdminMenu() {
    setColor(12);
    cout << "============= ADMIN MENU ================\n";
    cout << "  [1] View Most Borrowed Book\n";
    cout << "  [2] Add Book\n";
    cout << "  [3] View All Books\n";
    cout << "  [4] Search Books\n";
    cout << "  [5] Delete Book\n";
    cout << "  [6] View Overdue Books\n";
    cout << "  [7] Edit Book\n";
    cout << "  [8] View Loan History\n";
    cout << "  [9] Exit\n";
    setColor(8);
    cout << "==========================================\n";
    setColor(7);
}

class Book {
public:
    int id;
    string title, author, genre;
    bool isAvailable;
    Book(int i, string t, string a, string g, bool avail = true) {
        id = i;
        title = t;
        author = a;
        genre = g;
        isAvailable = avail;
    }
    string serialize() const {
        ostringstream ss;
        ss << id << "," << title << "," << author << "," << genre << "," << (isAvailable ? "1" : "0");
        return ss.str();
    }
};

string normalize(const string& input) {
    string temp = input;
    for (size_t i = 0; i < temp.size(); ++i) {
        temp[i] = tolower(temp[i]);
    }
    string result = "";
    for (size_t i = 0; i < temp.size(); ++i) {
        if (i + 1 < temp.size()) {
            string pair = temp.substr(i, 2);
            if (pair == "th") { result += "θ"; ++i; continue; }
            if (pair == "ps") { result += "ψ"; ++i; continue; }
            if (pair == "ks") { result += "ξ"; ++i; continue; }
        }
        char ch = temp[i];
        switch (ch) {
            case 'a': result += "α"; break;
            case 'b': result += "β"; break;
            case 'g': result += "γ"; break;
            case 'd': result += "δ"; break;
            case 'e': result += "ε"; break;
            case 'z': result += "ζ"; break;
            case 'h': result += "η"; break;
            case 'i': result += "ι"; break;
            case 'k': result += "κ"; break;
            case 'l': result += "λ"; break;
            case 'm': result += "μ"; break;
            case 'n': result += "ν"; break;
            case 'o': result += "ο"; break;
            case 'p': result += "π"; break;
            case 'r': result += "ρ"; break;
            case 's': result += "σ"; break;
            case 't': result += "τ"; break;
            case 'y': result += "υ"; break;
            case 'f': result += "φ"; break;
            case 'x': result += "χ"; break;
            case 'w': result += "ω"; break;
            default: result += ch; break;
        }
    }
    return result;
};

class User {
public:
    int id;
    string username, password;
    bool isAdmin;
    User(int i, string u, string p, bool admin) {
        id = i;
        username = u;
        password = p;
        isAdmin = admin;
    }
    string serialize() const {
        ostringstream ss;
        ss << id << "," << username << "," << password << "," << (isAdmin ? "1" : "0");
        return ss.str();
    }
};

class Loan {
public:
    int id, userId, bookId;
    string loanDate, returnDate;
    bool returned;
    Loan(int i, int u, int b, string lDate, string rDate, bool ret) {
        id = i;
        userId = u;
        bookId = b;
        loanDate = lDate;
        returnDate = rDate;
        returned = ret;
    }
    string serialize() const {
        ostringstream ss;
        ss << id << "," << userId << "," << bookId << "," << loanDate << "," << returnDate << "," << (returned ? "1" : "0");
        return ss.str();
    }
};

void saveUser(User user) {
    ofstream file("users.txt", ios::app);
    if (file.is_open()) {
        file << user.serialize() << endl;
        file.close();
    } else {
        cerr << "Failed to open users.txt" << endl;
    }
}

vector<User> loadUsers() {
    vector<User> users;
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string idStr, username, password, isAdminStr;
        if (getline(ss, idStr, ',') && getline(ss, username, ',') && getline(ss, password, ',') && getline(ss, isAdminStr)) {
            int id = atoi(idStr.c_str());
            bool isAdmin = (isAdminStr == "1");
            User u(id, username, password, isAdmin);
            users.push_back(u);
        }
    }
    return users;
}

void ensureDefaultAdmins() {
    vector<User> users = loadUsers();
    bool found1 = false, found2 = false;
    for (size_t i = 0; i < users.size(); i++) {
        if (users[i].username == "sakaros") found1 = true;
        if (users[i].username == "gregory") found2 = true;
    }
}

void returnBook(int userId) {
    string loanIdStr;
    cout << "Enter Loan ID to return: ";
    cin >> loanIdStr;
    int loanId = atoi(loanIdStr.c_str());
    ifstream loansIn("loans.txt");
    ofstream loansOut("temp_loans.txt");
    string line;
    bool found = false;
    int bookIdToMakeAvailable = -1;
    while (getline(loansIn, line)) {
        istringstream ss(line);
        string id, uid, bid, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, uid, ',') && getline(ss, bid, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            if (atoi(id.c_str()) == loanId && atoi(uid.c_str()) == userId && returned == "0") {
                found = true;
                bookIdToMakeAvailable = atoi(bid.c_str());
                loansOut << id << "," << uid << "," << bid << "," << loanDate << "," << returnDate << ",1" << endl;
            } else {
                loansOut << line << endl;
            }
        }
    }
    loansIn.close();
    loansOut.close();
    remove("loans.txt");
    rename("temp_loans.txt", "loans.txt");
    if (!found) {
        cout << "Loan not found or already returned." << endl;
        return;
    }
    ifstream booksIn("books.txt");
    ofstream booksOut("temp_books.txt");
    while (getline(booksIn, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') && getline(ss, author, ',') && getline(ss, genre, ',') && getline(ss, available)) {
            if (atoi(id.c_str()) == bookIdToMakeAvailable) {
                booksOut << id << "," << title << "," << author << "," << genre << ",1" << endl;
            } else {
                booksOut << line << endl;
            }
        }
    }
    booksIn.close();
    booksOut.close();
    remove("books.txt");
    rename("temp_books.txt", "books.txt");
    cout << "Book returned successfully." << endl;
}

void mostBorrowedBook() {
    ifstream file("loans.txt");
    map<int, int> bookCounts;
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string id, userId, bookId, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, userId, ',') && getline(ss, bookId, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            int bId = atoi(bookId.c_str());
            bookCounts[bId]++;
        }
    }
    file.close();
    int maxBorrows = 0;
    int mostBorrowedId = -1;
    for (map<int, int>::iterator it = bookCounts.begin(); it != bookCounts.end(); ++it) {
        if (it->second > maxBorrows) {
            maxBorrows = it->second;
            mostBorrowedId = it->first;
        }
    }
    if (mostBorrowedId == -1) {
        cout << "No book borrow records found." << endl;
        return;
    }
    ifstream books("books.txt");
    while (getline(books, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') && getline(ss, author, ',') &&
            getline(ss, genre, ',') && getline(ss, available)) {
            if (atoi(id.c_str()) == mostBorrowedId) {
                cout << "Most Borrowed Book: " << title << " by " << author << " (" << genre << ") - Borrowed " << maxBorrows << " times." << endl;
                break;
            }
        }
    }
    books.close();
}

void viewMyLoans(int userId) {
    ifstream file("loans.txt");
    string line;
    bool found = false;
    cout << "Your Current Loans: " << endl;
    while (getline(file, line)) {
        istringstream ss(line);
        string id, uid, bookId, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, uid, ',') && getline(ss, bookId, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            if (atoi(uid.c_str()) == userId && returned == "0") {
                ifstream bookFile("books.txt");
                string bookLine;
                while (getline(bookFile, bookLine)) {
                    istringstream bss(bookLine);
                    string bid, title, author, genre, available;
                    if (getline(bss, bid, ',') && getline(bss, title, ',') && getline(bss, author, ',') &&
                        getline(bss, genre, ',') && getline(bss, available)) {
                        if (bid == bookId) {
                            cout << "Loan ID: " << id << " | Book: " << title << " | Author: " << author << " | Return By: " << returnDate << endl;
                            found = true;
                            break;
                        }
                    }
                }
                bookFile.close();
            }
        }
    } 
	if (!found) {
        cout << "No active loans found." << endl;
    }
    file.close();
}

void viewBooks();

void requestLoan(int userId) {
    viewBooks();
    cout << "Enter the ID of the book you want to borrow: ";
    int bookId;
    cin >> bookId;
    ifstream booksIn("books.txt");
    ofstream booksOut("temp_books.txt");
    string line;
    bool found = false;
    while (getline(booksIn, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') && getline(ss, author, ',') &&
            getline(ss, genre, ',') && getline(ss, available)) {
            int currentId = atoi(id.c_str());
            if (currentId == bookId && available == "1") {
                found = true;
                booksOut << id << "," << title << "," << author << "," << genre << ",0" << endl;
            } else {
                booksOut << line << endl;
            }
        }
    }
    booksIn.close();
    booksOut.close();
    if (!found) {
        cout << "Book not available or not found." << endl;
        remove("temp_books.txt");
        return;
    }
    remove("books.txt");
    rename("temp_books.txt", "books.txt");
    int newLoanId = 1;
    ifstream loanFile("loans.txt");
    while (getline(loanFile, line)) {
        istringstream ss(line);
        string id;
        if (getline(ss, id, ',')) {
            int lid = atoi(id.c_str());
            if (lid >= newLoanId) newLoanId = lid + 1;
        }
    }
    loanFile.close();
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char loanDate[11], returnDate[11];
    strftime(loanDate, sizeof(loanDate), "%d/%m/%Y", ltm);
    time_t future = now + 14 * 86400;
    tm* due = localtime(&future);
    strftime(returnDate, sizeof(returnDate), "%d/%m/%Y", due);
    Loan newLoan(newLoanId, userId, bookId, loanDate, returnDate, false);
    ofstream loanOut("loans.txt", ios::app);
    if (loanOut.is_open()) {
        loanOut << newLoan.serialize() << endl;
        loanOut.close();
        cout << "Loan request successful." << endl;
    } else {
        cerr << "Failed to save loan." << endl;
    }
}

void addBook() {
    string title, author, genre;
    cin.ignore(); 
    do {        
	cout << "Enter book title: ";
	getline(cin, title);
	if (title.empty()) cout << "Title cannot be empty.\n";    
	} while (title.empty());    
	do {        
	cout << "Enter author: ";        
	getline(cin, author);        
	if (author.empty()) 
	cout << "Author cannot be empty.\n";    
	} while (author.empty());    
	do {        
	cout << "Enter genre: ";        
	getline(cin, genre);        
	if (genre.empty()) 
	cout << "Genre cannot be empty.\n";    
	} while (genre.empty());    
	cin.ignore();
	ifstream check("books.txt");
    string existingLine;
    bool duplicate = false;
    while (getline(check, existingLine)) {
        istringstream ss(existingLine);
        string idCheck, titleCheck, authorCheck, genreCheck, availableCheck;
        if (getline(ss, idCheck, ',') && getline(ss, titleCheck, ',') && getline(ss, authorCheck, ',')) {
            if (titleCheck == title && authorCheck == author) {
                duplicate = true;
                break;
            }
        }
    }
    check.close();
    if (duplicate) {
        cout << "Book with the same title and author already exists.\n";
        return;
    }
    while (getline(check, existingLine)) {
        istringstream ss(existingLine);
        string idCheck, titleCheck, authorCheck, genreCheck, availableCheck;
        if (getline(ss, idCheck, ',') && getline(ss, titleCheck, ',') && getline(ss, authorCheck, ',')) {
            if (titleCheck == title && authorCheck == author) {
                duplicate = true;
                break;
            }
        }
    }
    check.close();
    if (duplicate) {
        cout << "Book with the same title and author already exists.\n";
        return;
    }
    getline(cin, title);
    getline(cin, author);
    getline(cin, genre);
    ifstream file("books.txt");
    string line;
    int maxId = 0;
    while (getline(file, line)) {
        istringstream ss(line);
        string id;
        if (getline(ss, id, ',')) {
            int currentId = atoi(id.c_str());
            if (currentId > maxId) maxId = currentId;
        }
    }
    file.close();
    int newId = maxId + 1;
    Book newBook(newId, title, author, genre);
    ofstream out("books.txt", ios::app);
    if (out.is_open()) {
        out << newBook.serialize() << endl;
        out.close();
        cout << "Book added successfully." << endl;
    } else {
        cerr << "Failed to open books.txt" << endl;
    }
}

void viewBooks() {
    ifstream file("books.txt");
    string line;
    cout << "Available Books: " << endl;
    while (getline(file, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') && getline(ss, author, ',') &&
            getline(ss, genre, ',') && getline(ss, available)) {
            cout << "ID: " << id << ", Title: " << title << ", Author: " << author
                 << ", Genre: " << genre << ", Available: " << (available == "1" ? "Yes" : "No") << endl;
        }
    }
    file.close();
}

void searchBooks() {
    ifstream file("books.txt");
    string line, keyword;
    cout << "Enter search keyword (title,author,genre): ";
    cin.ignore();
    getline(cin, keyword);
    bool found = false;
    while (getline(file, line)) {
        if (line.find(keyword) != string::npos) {
            istringstream ss(line);
            string id, title, author, genre, available;
            if (getline(ss, id, ',') && getline(ss, title, ',') &&
                getline(ss, author, ',') && getline(ss, genre, ',') &&
                getline(ss, available)) {
                cout << "ID: " << id << ", Title: " << title << ", Author: " << author
                     << ", Genre: " << genre << ", Available: "
                     << (available == "1" ? "Yes" : "No") << endl;
                found = true;
            }
        }
    }
    if (!found) {
        cout << "No matching books found." << endl;
    }
    file.close();
}

void deleteBook() {
    int targetId;
    cout << "Enter ID of the book to delete: ";
    cin >> targetId;
    ifstream in("books.txt");
    ofstream out("temp_books.txt");
    string line;
    bool deleted = false;
    while (getline(in, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') && getline(ss, author, ',') &&
            getline(ss, genre, ',') && getline(ss, available)) {
            if (atoi(id.c_str()) == targetId) {
                deleted = true;
                continue;
            }
        }
        out << line << endl;
    }
    in.close();
    out.close();
    remove("books.txt");
    rename("temp_books.txt", "books.txt");
    if (deleted) {
        cout << "Book deleted successfully." << endl;
    } else {
        cout << "Book with ID " << targetId << " not found." << endl;
    }
}

void mostOverdueUser() {
    ifstream file("loans.txt");
    map<int, int> overdueCount;
    string line;
    time_t now = time(0);
    tm now_tm = *localtime(&now);
    while (getline(file, line)) {
        istringstream ss(line);
        string id, userId, bookId, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, userId, ',') && getline(ss, bookId, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            if (returned == "1") continue;
            tm due = {};
            istringstream dateStream(returnDate);
            string day, mon, yr;
            if (getline(dateStream, day, '/') && getline(dateStream, mon, '/') && getline(dateStream, yr)) {
                due.tm_mday = atoi(day.c_str());
                due.tm_mon = atoi(mon.c_str()) - 1;
                due.tm_year = atoi(yr.c_str()) - 1900;
                time_t due_time = mktime(&due);
                if (difftime(now, due_time) > 0) {
                    overdueCount[atoi(userId.c_str())]++;
                }
            }
        }
    }
    file.close();
    int maxOverdue = 0, userId = -1;
    for (map<int, int>::iterator it = overdueCount.begin(); it != overdueCount.end(); ++it) {
        if (it->second > maxOverdue) {
            maxOverdue = it->second;
            userId = it->first;
        }
    }
    if (userId == -1) {
        cout << "No overdue users found." << endl;
        return;
    }
    ifstream users("users.txt");
    while (getline(users, line)) {
        istringstream ss(line);
        string id, username, password, admin;
        if (getline(ss, id, ',') && getline(ss, username, ',') &&
            getline(ss, password, ',') && getline(ss, admin)) {
            if (atoi(id.c_str()) == userId) {
                cout << "User with most overdue books: " << username
                     << " (" << maxOverdue << " overdue loans)" << endl;
                break;
            }
        }
    }
    users.close();
}

void viewOverdueBooks() {
    ifstream file("loans.txt");
    string line;
    time_t now = time(0);
    tm now_tm = *localtime(&now);
    bool found = false;
    cout << "Overdue Books:" << endl;
    while (getline(file, line)) {
        istringstream ss(line);
        string id, userId, bookId, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, userId, ',') && getline(ss, bookId, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            if (returned == "1") continue;
            tm due = {};
            istringstream ds(returnDate);
            string d, m, y;
            if (getline(ds, d, '/') && getline(ds, m, '/') && getline(ds, y)) {
                due.tm_mday = atoi(d.c_str());
                due.tm_mon = atoi(m.c_str()) - 1;
                due.tm_year = atoi(y.c_str()) - 1900;
                time_t due_time = mktime(&due);
                if (difftime(now, due_time) > 0) {
                    cout << "Loan ID: " << id << ", Book ID: " << bookId << ", Due: " << returnDate << endl;
                    found = true;
                }
            }
        }
    }
    file.close();
    if (!found) {
        cout << "No overdue books found." << endl;
    }
}

void editBook() {
    int targetId;
    cout << "Enter ID of the book to edit: ";
    cin >> targetId;
    cin.ignore();
    ifstream in("books.txt");
    ofstream out("temp_books.txt");
    string line;
    bool edited = false;
    while (getline(in, line)) {
        istringstream ss(line);
        string id, title, author, genre, available;
        if (getline(ss, id, ',') && getline(ss, title, ',') &&
            getline(ss, author, ',') && getline(ss, genre, ',') && getline(ss, available)) {
            if (atoi(id.c_str()) == targetId) {
                string newTitle, newAuthor, newGenre;
                cout << "Enter new title (current: " << title << "): ";
                getline(cin, newTitle);
                cout << "Enter new author (current: " << author << "): ";
                getline(cin, newAuthor);
                cout << "Enter new genre (current: " << genre << "): ";
                getline(cin, newGenre);
                out << id << "," << (newTitle.empty() ? title : newTitle) << ","
                    << (newAuthor.empty() ? author : newAuthor) << ","
                    << (newGenre.empty() ? genre : newGenre) << "," << available << endl;
                edited = true;
                continue;
            }
        }
        out << line << endl;
    }
    in.close();
    out.close();
    remove("books.txt");
    rename("temp_books.txt", "books.txt");
    if (edited) {
        cout << "Book edited successfully." << endl;
    } else {
        cout << "Book with ID " << targetId << " not found." << endl;
    }
}

void viewLoanHistory() {
    ifstream file("loans.txt");
    string line;
    cout << "Loan History: " << endl;
    while (getline(file, line)) {
        istringstream ss(line);
        string id, userId, bookId, loanDate, returnDate, returned;
        if (getline(ss, id, ',') && getline(ss, userId, ',') && getline(ss, bookId, ',') &&
            getline(ss, loanDate, ',') && getline(ss, returnDate, ',') && getline(ss, returned)) {
            cout << "Loan ID: " << id << ", User ID: " << userId << ", Book ID: " << bookId
                 << ", Loan Date: " << loanDate << ", Return Date: " << returnDate
                 << ", Returned: " << (returned == "1" ? "Yes" : "No") << endl;
        }
    }
    file.close();
}

vector<User> loadUsers();
vector<User> loadUsers();
	string getHiddenInput() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password = password.substr(0, password.length() - 1);
            cout << "\b \b";
        } else if (ch != '\b') {
            password += ch;
            cout << '*';
        }
    }
    cout << endl;
    return password;
}
string xorEncryptDecrypt(const string& input, char key) {
    string output = input;
    for (size_t i = 0; i < input.length(); ++i)
        output[i] = input[i] ^ key;
    return output;
}

int main() {
	while (true) { 
    	ensureDefaultAdmins();
    	showMainMenu();
    	int choice;
    	cin >> choice;
    	string username, password;
    	bool isAdmin;
    	if (choice == 1) {
        	cout << "Login" << endl;
        	cout << "Username: ";
        	cin >> username;
        	cout << "Password: ";
        	password = getHiddenInput();
        	vector<User> users = loadUsers();
        	bool loggedIn = false;
        	int userId = -1;
        	for (size_t i = 0; i < users.size(); i++) {
            	if (users[i].username == username && users[i].password == password) {
                	isAdmin = users[i].isAdmin;
                	loggedIn = true;
                	userId = users[i].id;
                	break;
            	}
        	}
        	if (loggedIn) {
            	cout << "Login successful. Welcome, " << username << (isAdmin ? " (Admin)" : " (User)") << endl;
            	if (isAdmin) {
                	while (true) {
                    	showAdminMenu();
                    	cout << "Choose option: ";
                    	int adminChoice;
                    	cin >> adminChoice;
                    	if (adminChoice == 1) {
                        	mostBorrowedBook();
                    	} else if (adminChoice == 2) {
                        	addBook();
                    	} else if (adminChoice == 3) {
                        	viewBooks();
                    	} else if (adminChoice == 4) {
                        	searchBooks();
                    	} else if (adminChoice == 5) {
                        	deleteBook();
                    	} else if (adminChoice == 6) {
                        	viewOverdueBooks();
                    	} else if (adminChoice == 7) {
                        	editBook();
                    	} else if (adminChoice == 8) {
                        	viewLoanHistory();
                    	} else if (adminChoice == 9) {
                        	cout << "Exiting admin menu." << endl;
                        	break;
                    	} else {
        					cin.clear();
       						cin.ignore(10000, '\n');
        					cout << "Incorrect admin user details, please try again." << endl;
                    	}
                	}
            	} else {
                	while (true) {
                    	showUserMenu();
                    	cout << "Choose option: ";
                    	int userChoice;
                    	cin >> userChoice;
                    	if (userChoice == 1) {
                        	returnBook(userId);
                    	} else if (userChoice == 2) {
                        	requestLoan(userId);
                    	} else if (userChoice == 3) {
                        	viewMyLoans(userId);
                    	} else if (userChoice == 4) {
                        	searchBooks();
                    	} else if (userChoice == 5) {
                        	cout << "Exiting user menu." << endl;
                        	break;
                    	} else {
        					cin.clear();
        					cin.ignore(10000, '\n');
        					cout << "Invalid option. Please try again." << endl;
                    	}
                	}
            	}
        	} else {
            	cout << "Incorrect user details, please try again." << endl;
        	}
    	} else if (choice == 2) {
        	cout << "Register New User" << endl;
        	vector<User> users = loadUsers();
        	bool validUsername = false;
        	while (!validUsername) {
            	cout << "Username: ";
            	cin >> username;
            	validUsername = true;
            	for (int i = 0; i < users.size(); i++) {
                	if (users[i].username == username) {
                    	cout << "Username already exists. Please try again.";
                    	validUsername = false;
                    	break;
                	}
            	}
            	if (username.empty()) {
                	cout << "Username is empty, please try agian.";
                	validUsername = false;
            	}
        	}
        	bool validPassword = false;
        	while (!validPassword) {      
    			cout << "Password: ";
    			password = getHiddenInput();
    			cout << "You entered username: " << username << " and a hidden password." << endl;
            	validPassword = true;
            	for (int i = 0; i < users.size(); i++) {
                	if (users[i].password == password) {
                    	cout << "Password already used. Please try again.";
                    	validPassword = false;
                    	break;
                	}
            	}
            	if (password.empty()) {
                	cout << "Password is empty, please try again.";
                	validPassword = false;
            	}
        	}
        	User newUser(users.size() + 1, username, password, false);
        	saveUser(newUser);
        	cout << "Registration successful.\n";
			cout << "Press any key to continue...";
			_getch();
			continue;
		} else if (choice == 3) {
			cout << "Exiting program." << endl;
            break;
    	} else {
			cin.clear();
    		cin.ignore(10000, '\n');
    		cout << "Invalid option. Please enter a valid number (1-3)." << endl;
    	}
    	cout << "Choose option: ";
    	cin >> choice;
	}

	return 0;
}
