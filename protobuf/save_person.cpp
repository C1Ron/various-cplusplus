#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>
#include "person.pb.h"
#include <google/protobuf/timestamp.pb.h>
#include <sqlite3.h>

using namespace std;

// Function to generate a unique ID based on existing rows in the database
int generateUniqueId(sqlite3* db) {
    const char* sql = "SELECT MAX(id) FROM persons";
    sqlite3_stmt* stmt;
    int id = 1;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0) + 1;
        }
    }
    sqlite3_finalize(stmt);
    return id;
}

// Function to generate a unique UID
uint32_t generateUniqueUid() {
    srand(time(0) + getpid());
    uint32_t uid = static_cast<uint32_t>(time(0)) ^ static_cast<uint32_t>(rand());
    return uid;
}

void savePersonToDatabase(sqlite3* db, const Person& person) {
    const char* sql = "INSERT INTO persons (id, uid, fname, lname, bdate, lang, proto_data) VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, person.id());
    sqlite3_bind_int(stmt, 2, person.uid());
    sqlite3_bind_text(stmt, 3, person.fname().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, person.lname().c_str(), -1, SQLITE_STATIC);

    // Format the timestamp to a readable date string
    std::time_t t = person.bdate().seconds();
    std::tm* tm_ptr = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
    string bdate_str = oss.str();
    sqlite3_bind_text(stmt, 5, bdate_str.c_str(), -1, SQLITE_STATIC);

    sqlite3_bind_text(stmt, 6, person.lang().c_str(), -1, SQLITE_STATIC);

    // Serialize the Protobuf message to binary format
    string serialized_data;
    person.SerializeToString(&serialized_data);
    sqlite3_bind_blob(stmt, 7, serialized_data.data(), serialized_data.size(), SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << endl;
    }

    sqlite3_finalize(stmt);
}

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 7) {
        cerr << "Usage: " << argv[0] << " <fname> <lname> <lang> <bday> <bmonth> <byear>" << endl;
        return 1;
    }

    // Parse command-line arguments
    string fname = argv[1];
    string lname = argv[2];
    string lang = argv[3];
    int bday = stoi(argv[4]);
    int bmonth = stoi(argv[5]);
    int byear = stoi(argv[6]);

    // Initialize the Protocol Buffers library
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // Open SQLite database
    sqlite3* db;
    if (sqlite3_open("persons.db", &db) != SQLITE_OK) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return 1;
    }

    // Create table if it doesn't exist
    const char* create_table_sql = "CREATE TABLE IF NOT EXISTS persons ("
                                   "id INTEGER PRIMARY KEY, "
                                   "uid INTEGER, "
                                   "fname TEXT, "
                                   "lname TEXT, "
                                   "bdate TEXT, "
                                   "lang TEXT, "
                                   "proto_data BLOB)";
    char* errmsg;
    if (sqlite3_exec(db, create_table_sql, 0, 0, &errmsg) != SQLITE_OK) {
        cerr << "Failed to create table: " << errmsg << endl;
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return 1;
    }

    // Create a new person
    Person person;
    person.set_id(generateUniqueId(db));
    person.set_uid(generateUniqueUid());
    person.set_fname(fname);
    person.set_lname(lname);
    person.set_lang(lang);

    // Set the birthday (bdate) to 12:00:00
    std::tm tm = {};
    tm.tm_mday = bday;
    tm.tm_mon = bmonth - 1; // tm_mon is 0-based
    tm.tm_year = byear - 1900; // tm_year is years since 1900
    tm.tm_hour = 12;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    std::time_t t = std::mktime(&tm);

    google::protobuf::Timestamp* bdate = new google::protobuf::Timestamp();
    bdate->set_seconds(t);
    person.set_allocated_bdate(bdate);

    // Format the timestamp to a readable date string
    std::tm* tm_ptr = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
    string bdate_str = oss.str();

    // Save the person to the database
    savePersonToDatabase(db, person);

    // Display the person's details
    cout << "Person saved with the following details:" << endl;
    cout << "First Name: " << person.fname() << endl;
    cout << "Last Name: " << person.lname() << endl;
    cout << "ID: " << person.id() << endl;
    cout << "UID: " << person.uid() << endl;
    cout << "Birthday: " << bdate_str << endl;
    cout << "Language: " << person.lang() << endl;

    // Close the database
    sqlite3_close(db);

    // Clean up Protocol Buffers library
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
