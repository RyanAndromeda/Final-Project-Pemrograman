#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <ctime>

using namespace std;

struct Book {
    int id;
    string title;
    string author;
    string category;
    string status;
    int priority;
    string dateAdded;
};

class MiniLibrary {
private:
    vector<Book> books;
    int nextId;
    const string filename = "library_data.txt";

    string getCurrentDate() {
        time_t now = time(0);
        tm* localTime = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%d-%m-%Y", localTime);
        return string(buffer);
    }

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string priorityToString(int priority) {
        switch(priority) {
            case 1: return "Tinggi";
            case 2: return "Sedang";
            case 3: return "Rendah";
        }
        return "Tidak Diketahui";
    }

public:
    MiniLibrary() : nextId(1) {
        loadData();
    }

    void loadData() {
        ifstream file(filename);
        if (!file.is_open()) {
            return;
        }

        books.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            Book book;
            string temp;
            stringstream ss(line);

            if (!getline(ss, temp, '|')) continue;
            try {
                book.id = stoi(temp);
            } catch (...) { continue; }

            if (!getline(ss, book.title, '|')) continue;
            if (!getline(ss, book.author, '|')) continue;
            if (!getline(ss, book.category, '|')) continue;
            if (!getline(ss, book.status, '|')) continue;

            if (!getline(ss, temp, '|')) continue;
            try {
                book.priority = stoi(temp);
            } catch (...) { book.priority = 3; }

            if (!getline(ss, book.dateAdded)) book.dateAdded = "";

            books.push_back(book);
            nextId = max(nextId, book.id + 1);
        }

        cout << "Data berhasil dimuat dari file.\n";
    }

    void saveData() {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "Gagal menyimpan data.\n";
            return;
        }

        for (const auto &book : books) {
            string title = book.title;
            string author = book.author;
            string category = book.category;
            string status = book.status;
            auto sanitize = [](string s) {
                for (char &c : s) if (c == '|') c = '/';
                return s;
            };
            title = sanitize(title);
            author = sanitize(author);
            category = sanitize(category);
            status = sanitize(status);

            file << book.id << "|"
                 << title << "|"
                 << author << "|"
                 << category << "|"
                 << status << "|"
                 << book.priority << "|"
                 << book.dateAdded << "\n";
        }

        cout << "Data berhasil disimpan ke file.\n";
    }

    void addBook() {
        Book newBook;
        newBook.id = nextId++;

        cout << "\n=== TAMBAH BUKU BARU ===\n";

        cout << "Judul Buku: ";
        getline(cin, newBook.title);

        cout << "Penulis: ";
        getline(cin, newBook.author);

        cout << "Kategori: ";
        getline(cin, newBook.category);

        int statusChoice;
        while (true) {
            cout << "Status Baca (1: Belum Dibaca, 2: Sedang Dibaca, 3: Selesai): ";
            if (!(cin >> statusChoice)) {
                clearInputBuffer();
                cout << "Input tidak valid. Masukkan angka 1-3.\n";
                continue;
            }
            clearInputBuffer();
            if (statusChoice == 1) { newBook.status = "Belum Dibaca"; break; }
            if (statusChoice == 2) { newBook.status = "Sedang Dibaca"; break; }
            if (statusChoice == 3) { newBook.status = "Selesai"; break; }
            cout << "Pilihan tidak valid.\n";
        }

        int priorityChoice;
        while (true) {
            cout << "Prioritas (1: Tinggi, 2: Sedang, 3: Rendah): ";
            if (!(cin >> priorityChoice)) {
                clearInputBuffer();
                cout << "Input tidak valid. Masukkan angka 1-3.\n";
                continue;
            }
            clearInputBuffer();
            if (priorityChoice >= 1 && priorityChoice <= 3) {
                newBook.priority = priorityChoice;
                break;
            }
            cout << "Pilihan tidak valid.\n";
        }

        newBook.dateAdded = getCurrentDate();
        books.push_back(newBook);

        cout << "Buku berhasil ditambahkan dengan ID: " << newBook.id << "\n";
    }

    void displayAllBooks() {
        if (books.empty()) {
            cout << "\nTidak ada buku dalam perpustakaan.\n";
            return;
        }

        cout << "\n=== DAFTAR SEMUA BUKU ===\n";
        cout << "=============================================================================================================\n";
        cout << left << setw(5) << "ID" << setw(25) << "Judul"
             << setw(20) << "Penulis" << setw(15) << "Kategori"
             << setw(15) << "Status" << setw(10) << "Prioritas"
             << setw(12) << "Tanggal" << "\n";
        cout << "=============================================================================================================\n";

        for (const auto &book : books) {
            cout << left << setw(5) << book.id
                 << setw(25) << (book.title.length() > 24 ? book.title.substr(0, 24) + "." : book.title)
                 << setw(20) << (book.author.length() > 19 ? book.author.substr(0, 19) + "." : book.author)
                 << setw(15) << book.category
                 << setw(15) << book.status
                 << setw(10) << priorityToString(book.priority)
                 << setw(12) << book.dateAdded << "\n";
        }
        cout << "=============================================================================================================\n";
    }

    void searchBooks() {
        if (books.empty()) {
            cout << "\nTidak ada buku dalam perpustakaan.\n";
            return;
        }

        cout << "\n=== PENCARIAN BUKU ===\n";
        cout << "Cari berdasarkan: \n1. Judul\n2. Penulis\n3. Kategori\nPilihan: ";

        int choice;
        if (!(cin >> choice)) {
            clearInputBuffer();
            cout << "Input tidak valid. Kembali ke menu.\n";
            return;
        }
        clearInputBuffer();

        if (choice < 1 || choice > 3) {
            cout << "Pilihan tidak valid! Kembali ke menu...\n";
            return;
        }

        cout << "Masukkan kata kunci: ";
        string keyword;
        getline(cin, keyword);
        if (keyword.empty()) {
            cout << "Kata kunci kosong. Kembali ke menu...\n";
            return;
        }

        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

        vector<Book> results;
        for (const auto &book : books) {
            string field;
            if (choice == 1) field = book.title;
            else if (choice == 2) field = book.author;
            else field = book.category;

            string lowerField = field;
            transform(lowerField.begin(), lowerField.end(), lowerField.begin(), ::tolower);

            if (lowerField.find(keyword) != string::npos) {
                results.push_back(book);
            }
        }

        if (results.empty()) {
            cout << "Tidak ditemukan buku dengan kata kunci '" << keyword << "'.\n";
        } else {
            cout << "\nHasil Pencarian (" << results.size() << "):\n";
            for (const auto &book : results) {
                cout << "ID: " << book.id << " | " << book.title
                     << " | " << book.author << " | "
                     << book.status << " | "
                     << priorityToString(book.priority) << "\n";
            }
        }
    }

    void updateBookStatusAndPriority() {
        if (books.empty()) {
            cout << "\nTidak ada buku.\n";
            return;
        }

        displayAllBooks();
        cout << "\nMasukkan ID buku yang ingin diupdate: ";
        int id;
        if (!(cin >> id)) {
            clearInputBuffer();
            cout << "Input tidak valid.\n";
            return;
        }
        clearInputBuffer();

        auto it = find_if(books.begin(), books.end(), [id](const Book &b) { return b.id == id; });
        if (it == books.end()) {
            cout << "Buku tidak ditemukan.\n";
            return;
        }

        cout << "Buku: " << it->title << "\n";

        int status;
        while (true) {
            cout << "Status baru (1: Belum Dibaca, 2: Sedang Dibaca, 3: Selesai): ";
            if (!(cin >> status)) {
                clearInputBuffer();
                cout << "Input tidak valid. Masukkan 1-3.\n";
                continue;
            }
            clearInputBuffer();
            if (status == 1) { const_cast<Book&>(*it).status = "Belum Dibaca"; break; }
            if (status == 2) { const_cast<Book&>(*it).status = "Sedang Dibaca"; break; }
            if (status == 3) { const_cast<Book&>(*it).status = "Selesai"; break; }
            cout << "Pilihan tidak valid.\n";
        }

        int priority;
        while (true) {
            cout << "Prioritas baru (1: Tinggi, 2: Sedang, 3: Rendah): ";
            if (!(cin >> priority)) {
                clearInputBuffer();
                cout << "Input tidak valid. Masukkan 1-3.\n";
                continue;
            }
            clearInputBuffer();
            if (priority >= 1 && priority <= 3) { const_cast<Book&>(*it).priority = priority; break; }
            cout << "Pilihan tidak valid.\n";
        }

        cout << "Status dan prioritas berhasil diupdate!\n";
    }

    void deleteBook() {
        if (books.empty()) {
            cout << "\nTidak ada buku.\n";
            return;
        }

        displayAllBooks();
        cout << "\nMasukkan ID buku yang ingin dihapus: ";
        int id;
        if (!(cin >> id)) {
            clearInputBuffer();
            cout << "Input tidak valid.\n";
            return;
        }
        clearInputBuffer();

        auto it = find_if(books.begin(), books.end(), [id](const Book &b) { return b.id == id; });

        if (it == books.end()) {
            cout << "Buku tidak ditemukan.\n";
            return;
        }

        cout << "Yakin hapus \"" << it->title << "\"? (y/n): ";
        char c;
        if (!(cin >> c)) {
            clearInputBuffer();
            cout << "Input tidak valid.\n";
            return;
        }
        clearInputBuffer();

        if (c == 'y' || c == 'Y') {
            books.erase(it);
            cout << "Buku dihapus.\n";
        } else {
            cout << "Penghapusan dibatalkan.\n";
        }
    }

    void showStatistics() {
        if (books.empty()) {
            cout << "\nTidak ada buku.\n";
            return;
        }

        int unread = 0, reading = 0, finished = 0;
        int high = 0, medium = 0, low = 0;

        for (const auto &b : books) {
            if (b.status == "Belum Dibaca") unread++;
            else if (b.status == "Sedang Dibaca") reading++;
            else if (b.status == "Selesai") finished++;

            if (b.priority == 1) high++;
            else if (b.priority == 2) medium++;
            else if (b.priority == 3) low++;
        }

        int total = books.size();
        double progress = total == 0 ? 0 : finished * 100.0 / total;

        cout << "\n=== STATISTIK PERPUSTAKAAN ===\n";
        cout << "Total Buku: " << total << "\n\n";
        cout << "Status:\n";
        cout << "- Belum Dibaca: " << unread << "\n";
        cout << "- Sedang Dibaca: " << reading << "\n";
        cout << "- Selesai: " << finished << "\n\n";
        cout << "Prioritas:\n";
        cout << "- Tinggi: " << high << "\n";
        cout << "- Sedang: " << medium << "\n";
        cout << "- Rendah: " << low << "\n\n";
        cout << fixed << setprecision(1);
        cout << "Progress Membaca: " << progress << "%\n";
    }

    void showMenu() {
        cout << "\n=== MINI LIBRARY MANAGEMENT SYSTEM ===\n";
        cout << "1. Tambah Buku Baru\n";
        cout << "2. Tampilkan Semua Buku\n";
        cout << "3. Cari Buku\n";
        cout << "4. Update Status & Prioritas\n";
        cout << "5. Hapus Buku\n";
        cout << "6. Statistik\n";
        cout << "7. Simpan Data\n";
        cout << "8. Muat Data Ulang\n";
        cout << "0. Keluar\n";
        cout << "Pilihan: ";
    }

    void run() {
        int choice;
        do {
            showMenu();
            if (!(cin >> choice)) {
                clearInputBuffer();
                cout << "Input tidak valid. Masukkan angka menu.\n";
                choice = -1;
                continue;
            }
            clearInputBuffer();
            switch(choice) {
                case 1: addBook(); break;
                case 2: displayAllBooks(); break;
                case 3: searchBooks(); break;
                case 4: updateBookStatusAndPriority(); break;
                case 5: deleteBook(); break;
                case 6: showStatistics(); break;
                case 7: saveData(); break;
                case 8: loadData(); break;
                case 0:
                    saveData();
                    cout << "Terima kasih telah menggunakan Mini Library!\n";
                    break;
                default:
                    cout << "Pilihan tidak valid.\n";
                    choice = -1;
                    break;
            }
        } while (choice != 0);
    }
};

int main() {
    cout << "Selamat datang di Mini Library Management System!\n";
    MiniLibrary library;
    library.run();
    return 0;

}
