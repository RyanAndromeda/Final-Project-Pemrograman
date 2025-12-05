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

    string getLineTrim() {
        string s;
        getline(cin, s);
        while (!s.empty() && isspace(s.front())) s.erase(s.begin());
        while (!s.empty() && isspace(s.back())) s.pop_back();
        return s;
    }

    int getIntegerSafe(int minVal, int maxVal) {
        while (true) {
            string input;
            getline(cin, input);
            try {
                int value = stoi(input);
                if (value >= minVal && value <= maxVal) return value;
                cout << "Masukkan angka " << minVal << "-" << maxVal << ": ";
            } catch (...) {
                cout << "Input tidak valid. Masukkan angka: ";
            }
        }
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
    MiniLibrary() : nextId(1) { loadData(); }

    void loadData() {
        ifstream file(filename);
        if (!file.is_open()) return;

        books.clear();
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            Book book;
            string temp;
            stringstream ss(line);

            if (!getline(ss, temp, '|')) continue;
            try { book.id = stoi(temp); } catch (...) { continue; }

            getline(ss, book.title, '|');
            getline(ss, book.author, '|');
            getline(ss, book.category, '|');
            getline(ss, book.status, '|');

            getline(ss, temp, '|');
            try { book.priority = stoi(temp); } catch (...) { book.priority = 3; }

            getline(ss, book.dateAdded);

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

        auto sanitize = [](string s){
            for (char &c : s) if (c == '|') c = '/';
            return s;
        };

        for (const auto &book : books) {
            file << book.id << "|"
                 << sanitize(book.title) << "|"
                 << sanitize(book.author) << "|"
                 << sanitize(book.category) << "|"
                 << sanitize(book.status) << "|"
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
        newBook.title = getLineTrim();
        cout << "Penulis: ";
        newBook.author = getLineTrim();
        cout << "Kategori: ";
        newBook.category = getLineTrim();

        cout << "Status (1 Belum dibaca, 2 Sedang dibaca, 3 Selesai dibaca): ";
        int status = getIntegerSafe(1, 3);
        newBook.status = (status == 1 ? "Belum Dibaca" : status == 2 ? "Sedang Dibaca" : "Selesai");

        cout << "Prioritas (1 Tinggi, 2 Sedang, 3 Rendah): ";
        newBook.priority = getIntegerSafe(1, 3);

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

        for (const auto &b : books) {
            cout << left << setw(5) << b.id
                 << setw(25) << (b.title.length() > 24 ? b.title.substr(0, 24) + "." : b.title)
                 << setw(20) << (b.author.length() > 19 ? b.author.substr(0, 19) + "." : b.author)
                 << setw(15) << b.category
                 << setw(15) << b.status
                 << setw(10) << priorityToString(b.priority)
                 << setw(12) << b.dateAdded << "\n";
        }
        cout << "=============================================================================================================\n";
    }

    void searchBooks() {
        if (books.empty()) {
            cout << "\nTidak ada buku.\n";
            return;
        }

        cout << "\n=== PENCARIAN BUKU ===\n";
        cout << "Cari berdasarkan:\n1. Judul\n2. Penulis\n3. Kategori\nPilihan: ";
        int c = getIntegerSafe(1, 3);

        cout << "Masukkan kata kunci: ";
        string keyword = getLineTrim();
        transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

        vector<Book> results;
        for (auto &b : books) {
            string target = (c == 1 ? b.title : c == 2 ? b.author : b.category);
            transform(target.begin(), target.end(), target.begin(), ::tolower);
            if (target.find(keyword) != string::npos) results.push_back(b);
        }

        if (results.empty()) cout << "Tidak ditemukan buku.\n";
        else {
            cout << "\nHasil pencarian (" << results.size() << "):\n";
            for (auto &b : results)
                cout << "ID: " << b.id << " | " << b.title << " | " << b.author
                     << " | " << b.status << " | " << priorityToString(b.priority) << "\n";
        }
    }

    void updateBookStatusAndPriority() {
        if (books.empty()) { cout << "\nTidak ada buku.\n"; return; }

        displayAllBooks();
        cout << "\nMasukkan ID buku: ";
        int id = getIntegerSafe(1, 999999);

        auto it = find_if(books.begin(), books.end(), [id](auto &b){ return b.id == id; });
        if (it == books.end()) { cout << "Buku tidak ditemukan.\n"; return; }

        cout << "Status baru (1 Belum dibaca, 2 Sedang dibaca, 3 Selesai dibaca): ";
        int status = getIntegerSafe(1, 3);
        it->status = (status == 1 ? "Belum Dibaca" : status == 2 ? "Sedang Dibaca" : "Selesai");

        cout << "Prioritas baru (1 Tinggi, 2 Sedang, 3 Rendah): ";
        it->priority = getIntegerSafe(1, 3);

        cout << "Berhasil diperbarui!\n";
    }

    void deleteBook() {
        if (books.empty()) { cout << "\nTidak ada buku.\n"; return; }

        displayAllBooks();
        cout << "\nMasukkan ID buku: ";
        int id = getIntegerSafe(1, 999999);

        auto it = find_if(books.begin(), books.end(), [id](auto &b){ return b.id == id; });
        if (it == books.end()) { cout << "Buku tidak ditemukan.\n"; return; }

        cout << "Yakin hapus? (y/n): ";
        string c = getLineTrim();
        if (c == "y" || c == "Y") { books.erase(it); cout << "Buku dihapus.\n"; }
        else cout << "Dibatalkan.\n";
    }

    void showStatistics() {
        if (books.empty()) { cout << "\nTidak ada buku.\n"; return; }

        int unread = 0, reading = 0, finished = 0;
        int high = 0, med = 0, low = 0;

        for (auto &b : books) {
            if (b.status == "Belum Dibaca") unread++;
            if (b.status == "Sedang Dibaca") reading++;
            if (b.status == "Selesai") finished++;
            if (b.priority == 1) high++;
            if (b.priority == 2) med++;
            if (b.priority == 3) low++;
        }

        int total = books.size();
        cout << "\n=== STATISTIK ===\n";
        cout << "Total Buku: " << total << "\n";
        cout << "Belum Dibaca: " << unread << "\n";
        cout << "Sedang Dibaca: " << reading << "\n";
        cout << "Selesai: " << finished << "\n";
        cout << "Prioritas Tinggi: " << high << "\n";
        cout << "Prioritas Sedang: " << med << "\n";
        cout << "Prioritas Rendah: " << low << "\n";
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
        while (true) {
            showMenu();
            string input = getLineTrim();
            int choice;
            try { choice = stoi(input); } catch (...) {
                cout << "Input tidak valid. Masukkan angka menu.\n";
                continue;
            }

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
                    return;
                default:
                    cout << "Pilihan tidak valid.\n";
            }
        }
    }
};

int main() {
    cout << "Selamat datang di Mini Library Management System!\n";
    MiniLibrary library;
    library.run();
    return 0;
}
