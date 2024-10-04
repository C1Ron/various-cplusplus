#include <iostream>
#include <iomanip>
#include <ctime>
#include <sqlite3.h>
#include "person.pb.h"
#include <google/protobuf/timestamp.pb.h>

using namespace std;

void formatTimestamp(const std::string& timestamp_str) {
    std::tm tm = {};
    std::istringstream ss(timestamp_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        cerr << "Failed to parse timestamp" << endl;
        return;
    }
    std::time_t t = std::mktime(&tm);
    std::tm* tm_ptr = std::localtime(&t);
    std::cout << std::put_time(tm_ptr, "%d'th %B, %Y - %H:%M:%S") << endl;
}

void loadPersonFromDatabase(sqlite3* db, int id) {
    const char* sql = "SELECT id, uid, fname, lname, bdate, lang, proto_data FROM persons WHERE id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int uid = sqlite3_column_int(stmt, 1);
        const unsigned char* fname = sqlite3_column_text(stmt, 2);
        const unsigned char* lname = sqlite3_column_text(stmt, 3);
        const unsigned char* bdate = sqlite3_column_text(stmt, 4);
        const unsigned char* lang = sqlite3_column_text(stmt, 5);

        // Read and deserialize the Protobuf data
        const void* blob = sqlite3_column_blob(stmt, 6);
        int blob_size = sqlite3_column_bytes(stmt, 6);
        string serialized_data(reinterpret_cast<const char*>(blob), blob_size);
        Person person;
        person.ParseFromString(serialized_data);

        cout << "ID: " << id << endl;
        cout << "UID: " << uid << endl;
        cout << "First Name: " << fname << endl;
        cout << "Last Name: " << lname << endl;
        cout << "Birthday: ";
        formatTimestamp(reinterpret_cast<const char*>(bdate));
        cout << "Language: " << lang << endl;

        // Display the deserialized Protobuf message (for verification)
        cout << "Deserialized Protobuf Data:" << endl;
        cout << "First Name: " << person.fname() << endl;
        cout << "Last Name: " << person.lname() << endl;
        cout << "ID: " << person.id() << endl;
        cout << "UID: " << person.uid() << endl;
        cout << "Birthday: " << person.bdate().seconds() << endl;
        cout << "Language: " << person.lang() << endl;
    } else {
        cerr << "No record found with ID: " << id << endl;
    }

    sqlite3_finalize(stmt);
}

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <id>" << endl;
        return 1;
    }

    // Parse command-line argument
    int id = stoi(argv[1]);

    // Open SQLite database
    sqlite3* db;
    if (sqlite3_open("persons.db", &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Load the person from the database
    loadPersonFromDatabase(db, id);

    // Close the database
    sqlite3_close(db);

    return 0;
}
