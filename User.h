#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include "HashMap.h"
#include "Vector.h"

using namespace std;

struct User {
    string username;
    string password_hash;
    string salt;
    string email;
    string full_name;
    int age;
    string credit_card_number;
    string credit_card_name;
    double credit_limit;
    bool credit_card_linked;

    User() : age(0), credit_limit(5000.0), credit_card_linked(false) {}

    User(const string& uname, const string& pwd_hash, const string& s,
         const string& e, const string& name, int a)
        : username(uname), password_hash(pwd_hash), salt(s),
          email(e), full_name(name), age(a), credit_limit(5000.0), credit_card_linked(false) {}
};

class UserManager {
private:
    HashMap<string, User> users;
    Vector<string> username_list;
    static const string USER_DATA_FILE;


    string generate_salt() {
        static bool seeded = false;
        if (!seeded) {
            srand((unsigned int)time(nullptr));
            seeded = true;
        }

        int random_num = 100000 + (rand() % 900000);
        return to_string(random_num);
    }



    string hash_password(const string& password, const string& salt) {

        string combined = password + salt;



        unsigned long long hash = 0;
        unsigned long long prime = 2147483647ULL;


        for (int i = 0; i < (int)combined.size(); ++i) {
            hash = (hash * 31ULL + (unsigned char)combined[i]) % prime;
        }



        unsigned long long salt_hash = 0;
        for (int i = 0; i < (int)salt.size(); ++i) {
            salt_hash = (salt_hash * 37ULL + (unsigned char)salt[i]) % prime;
        }


        hash = (hash * salt_hash) % prime;


        for (int i = (int)combined.size() - 1; i >= 0; --i) {
            hash = (hash * 41ULL + (unsigned char)combined[i]) % prime;
        }


        stringstream ss;
        ss << hex << hash;
        string result = ss.str();


        while (result.size() < 16) {
            result = "0" + result;
        }

        return result;
    }


    string escape_csv(const string& s) {
        string result = "";
        for (int i = 0; i < (int)s.size(); ++i) {
            if (s[i] == ',') {
                result += "\\c";
            } else if (s[i] == '\n') {
                result += "\\n";
            } else if (s[i] == '\\') {
                result += "\\\\";
            } else {
                result += s[i];
            }
        }
        return result;
    }

    string unescape_csv(const string& s) {
        string result = "";
        for (int i = 0; i < (int)s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < (int)s.size()) {
                if (s[i + 1] == 'c') {
                    result += ',';
                    ++i;
                } else if (s[i + 1] == 'n') {
                    result += '\n';
                    ++i;
                } else if (s[i + 1] == '\\') {
                    result += '\\';
                    ++i;
                } else {
                    result += s[i];
                }
            } else {
                result += s[i];
            }
        }
        return result;
    }


    string normalize_username(const string& username) {
        int start = 0;
        while (start < (int)username.size() && (username[start] == ' ' || username[start] == '\t' || username[start] == '\r' || username[start] == '\n')) {
            start++;
        }
        int end = (int)username.size() - 1;
        while (end >= start && (username[end] == ' ' || username[end] == '\t' || username[end] == '\r' || username[end] == '\n')) {
            end--;
        }
        if (end < start) {
            return "";
        }
        return username.substr(start, end - start + 1);
    }

public:
    UserManager() {
        load_users();
    }

    ~UserManager() {
        save_users();
    }


    bool load_users() {

        users = HashMap<string, User>();
        username_list.clear();

        ifstream in(USER_DATA_FILE.c_str());
        if (!in.is_open()) {

            return false;
        }

        string line;
        bool first = true;
        int line_count = 0;
        int loaded_count = 0;
        while (getline(in, line)) {
            line_count++;
            if (line.size() == 0) {
                continue;
            }
            if (first) {
                first = false;
                continue;
            }

            Vector<string> tokens;
            string current = "";
            bool escaped = false;
            for (int i = 0; i < (int)line.size(); ++i) {
                char c = line[i];
                if (escaped) {
                    current += c;
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == ',') {
                    tokens.push_back(current);
                    current = "";
                } else {
                    current += c;
                }
            }
            tokens.push_back(current);


            if (tokens.size() < 6) {

                continue;
            }

            if (tokens.size() >= 6) {
                User u;
                u.username = normalize_username(unescape_csv(tokens[0]));


                if (u.username.empty()) {
                    continue;
                }

                u.password_hash = unescape_csv(tokens[1]);
                u.salt = unescape_csv(tokens[2]);
                u.email = unescape_csv(tokens[3]);
                u.full_name = unescape_csv(tokens[4]);
                stringstream ss(tokens[5]);
                ss >> u.age;


                if (tokens.size() >= 10) {
                    u.credit_card_number = unescape_csv(tokens[6]);
                    u.credit_card_name = unescape_csv(tokens[7]);
                    stringstream ss2(tokens[8]);
                    ss2 >> u.credit_limit;
                    stringstream ss3(tokens[9]);
                    int linked;
                    ss3 >> linked;
                    u.credit_card_linked = (linked != 0);
                } else {
                    u.credit_card_number = "";
                    u.credit_card_name = "";
                    u.credit_limit = 5000.0;
                    u.credit_card_linked = false;
                }


                if (!u.username.empty()) {
                    users.insert(u.username, u);

                    bool found = false;
                    for (int i = 0; i < username_list.size(); ++i) {
                        if (username_list[i] == u.username) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        username_list.push_back(u.username);
                    }
                    loaded_count++;
                }
            }
        }
        in.close();


        if (loaded_count == 0 && line_count > 1) {

            return false;
        }

        return true;
    }


    bool save_users() {
        ofstream out(USER_DATA_FILE.c_str());
        if (!out.is_open()) {
            return false;
        }

        out << "username,password_hash,salt,email,full_name,age,credit_card_number,credit_card_name,credit_limit,credit_card_linked\n";




        Vector<string> all_usernames;


        for (int i = 0; i < username_list.size(); ++i) {
            if (users.contains(username_list[i])) {
                bool found = false;
                for (int j = 0; j < all_usernames.size(); ++j) {
                    if (all_usernames[j] == username_list[i]) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    all_usernames.push_back(username_list[i]);
                }
            }
        }


        if (all_usernames.size() == 0) {


            if (users.size() > 0) {






                out.close();
                return false;
            }


            out.close();
            return true;
        }


        int saved_count = 0;
        for (int i = 0; i < all_usernames.size(); ++i) {
            if (users.contains(all_usernames[i])) {
                User& u = users[all_usernames[i]];
                out << escape_csv(u.username) << ","
                    << escape_csv(u.password_hash) << ","
                    << escape_csv(u.salt) << ","
                    << escape_csv(u.email) << ","
                    << escape_csv(u.full_name) << ","
                    << u.age << ","
                    << escape_csv(u.credit_card_number) << ","
                    << escape_csv(u.credit_card_name) << ","
                    << u.credit_limit << ","
                    << (u.credit_card_linked ? 1 : 0) << "\n";
                saved_count++;
            }
        }

        out.flush();
        out.close();



        ifstream verify(USER_DATA_FILE.c_str());
        bool verified = verify.is_open();
        if (verified) {
            int line_count = 0;
            string line;
            while (getline(verify, line)) {
                if (line.size() > 0) {
                    line_count++;
                }
            }


            verified = (line_count >= 2 && saved_count > 0);
        }
        verify.close();

        return verified;
    }


    bool signup(const string& username, const string& password,
                const string& email, const string& full_name, int age) {

        string normalized_username = normalize_username(username);

        if (normalized_username.empty() || password.empty()) {
            return false;
        }


        Vector<User> all_users;
        ifstream in(USER_DATA_FILE.c_str());
        if (in.is_open()) {
            string line;
            bool first = true;
            while (getline(in, line)) {
                if (line.size() == 0) continue;
                if (first) {
                    first = false;
                    continue;
                }


                Vector<string> tokens;
                string current = "";
                bool escaped = false;
                for (int i = 0; i < (int)line.size(); ++i) {
                    char c = line[i];
                    if (escaped) {
                        current += c;
                        escaped = false;
                    } else if (c == '\\') {
                        escaped = true;
                    } else if (c == ',') {
                        tokens.push_back(current);
                        current = "";
                    } else {
                        current += c;
                    }
                }
                tokens.push_back(current);

                if (tokens.size() >= 6) {
                    User u;
                    u.username = normalize_username(unescape_csv(tokens[0]));
                    u.password_hash = unescape_csv(tokens[1]);
                    u.salt = unescape_csv(tokens[2]);
                    u.email = unescape_csv(tokens[3]);
                    u.full_name = unescape_csv(tokens[4]);
                    stringstream ss(tokens[5]);
                    ss >> u.age;

                    if (tokens.size() >= 10) {
                        u.credit_card_number = unescape_csv(tokens[6]);
                        u.credit_card_name = unescape_csv(tokens[7]);
                        stringstream ss2(tokens[8]);
                        ss2 >> u.credit_limit;
                        stringstream ss3(tokens[9]);
                        int linked;
                        ss3 >> linked;
                        u.credit_card_linked = (linked != 0);
                    } else {
                        u.credit_card_number = "";
                        u.credit_card_name = "";
                        u.credit_limit = 5000.0;
                        u.credit_card_linked = false;
                    }

                    if (!u.username.empty()) {
                        all_users.push_back(u);
                    }
                }
            }
            in.close();
        }


        for (int i = 0; i < all_users.size(); ++i) {
            if (all_users[i].username == normalized_username) {
                return false;
            }
        }


        string normalized_email = "";
        int start = 0;
        while (start < (int)email.size() && (email[start] == ' ' || email[start] == '\t')) {
            start++;
        }
        int end = (int)email.size() - 1;
        while (end >= start && (email[end] == ' ' || email[end] == '\t')) {
            end--;
        }
        for (int i = start; i <= end; ++i) {
            char c = email[i];
            if (c >= 'A' && c <= 'Z') {
                c = (char)(c - 'A' + 'a');
            }
            normalized_email += c;
        }

        for (int i = 0; i < all_users.size(); ++i) {
            string user_email = "";
            for (int j = 0; j < (int)all_users[i].email.size(); ++j) {
                char c = all_users[i].email[j];
                if (c >= 'A' && c <= 'Z') {
                    c = (char)(c - 'A' + 'a');
                }
                user_email += c;
            }
            if (user_email == normalized_email) {
                return false;
            }
        }


        string salt = generate_salt();
        string hash = hash_password(password, salt);
        User new_user(normalized_username, hash, salt, email, full_name, age);
        all_users.push_back(new_user);


        ofstream out(USER_DATA_FILE.c_str());
        if (!out.is_open()) {
            return false;
        }


        out << "username,password_hash,salt,email,full_name,age,credit_card_number,credit_card_name,credit_limit,credit_card_linked\n";


        for (int i = 0; i < all_users.size(); ++i) {
            User& u = all_users[i];
            out << escape_csv(u.username) << ","
                << escape_csv(u.password_hash) << ","
                << escape_csv(u.salt) << ","
                << escape_csv(u.email) << ","
                << escape_csv(u.full_name) << ","
                << u.age << ","
                << escape_csv(u.credit_card_number) << ","
                << escape_csv(u.credit_card_name) << ","
                << u.credit_limit << ","
                << (u.credit_card_linked ? 1 : 0) << "\n";
        }

        out.flush();
        out.close();


        users.insert(normalized_username, new_user);
        bool found = false;
        for (int i = 0; i < username_list.size(); ++i) {
            if (username_list[i] == normalized_username) {
                found = true;
                break;
            }
        }
        if (!found) {
            username_list.push_back(normalized_username);
        }

        return true;
    }


    bool login(const string& username, const string& password) {
        string normalized_username = normalize_username(username);
        if (normalized_username.empty() || password.empty()) {
            return false;
        }


        ifstream in(USER_DATA_FILE.c_str());
        if (!in.is_open()) {
            return false;
        }

        string line;
        bool first = true;
        string found_username = "";
        string found_hash = "";
        string found_salt = "";

        while (getline(in, line)) {
            if (line.size() == 0) continue;
            if (first) {
                first = false;
                continue;
            }


            Vector<string> tokens;
            string current = "";
            bool escaped = false;
            for (int i = 0; i < (int)line.size(); ++i) {
                char c = line[i];
                if (escaped) {
                    current += c;
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == ',') {
                    tokens.push_back(current);
                    current = "";
                } else {
                    current += c;
                }
            }
            tokens.push_back(current);

            if (tokens.size() >= 3) {
                string file_username = normalize_username(unescape_csv(tokens[0]));
                if (file_username == normalized_username) {
                    found_username = file_username;
                    found_hash = unescape_csv(tokens[1]);
                    found_salt = unescape_csv(tokens[2]);
                    break;
                }
            }
        }
        in.close();

        if (found_username.empty()) {

            in.open(USER_DATA_FILE.c_str());
            if (in.is_open()) {
                first = true;
                string lower_input = "";
                for (int i = 0; i < (int)normalized_username.size(); ++i) {
                    char c = normalized_username[i];
                    if (c >= 'A' && c <= 'Z') {
                        c = (char)(c - 'A' + 'a');
                    }
                    lower_input += c;
                }

                while (getline(in, line)) {
                    if (line.size() == 0) continue;
                    if (first) {
                        first = false;
                        continue;
                    }

                    Vector<string> tokens;
                    string current = "";
                    bool escaped = false;
                    for (int i = 0; i < (int)line.size(); ++i) {
                        char c = line[i];
                        if (escaped) {
                            current += c;
                            escaped = false;
                        } else if (c == '\\') {
                            escaped = true;
                        } else if (c == ',') {
                            tokens.push_back(current);
                            current = "";
                        } else {
                            current += c;
                        }
                    }
                    tokens.push_back(current);

                    if (tokens.size() >= 3) {
                        string file_username = normalize_username(unescape_csv(tokens[0]));
                        string file_lower = "";
                        for (int i = 0; i < (int)file_username.size(); ++i) {
                            char c = file_username[i];
                            if (c >= 'A' && c <= 'Z') {
                                c = (char)(c - 'A' + 'a');
                            }
                            file_lower += c;
                        }
                        if (file_lower == lower_input) {
                            found_username = file_username;
                            found_hash = unescape_csv(tokens[1]);
                            found_salt = unescape_csv(tokens[2]);
                            break;
                        }
                    }
                }
                in.close();
            }
        }

        if (found_username.empty()) {
            return false;
        }


        string computed_hash = hash_password(password, found_salt);
        if (computed_hash != found_hash) {
            return false;
        }


        load_users();

        return true;
    }


    bool username_exists(const string& username) {
        string normalized_username = normalize_username(username);
        if (normalized_username.empty()) {
            return false;
        }


        ifstream in(USER_DATA_FILE.c_str());
        if (!in.is_open()) {
            return false;
        }

        string line;
        bool first = true;

        while (getline(in, line)) {
            if (line.size() == 0) continue;
            if (first) {
                first = false;
                continue;
            }


            Vector<string> tokens;
            string current = "";
            bool escaped = false;
            for (int i = 0; i < (int)line.size(); ++i) {
                char c = line[i];
                if (escaped) {
                    current += c;
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == ',') {
                    tokens.push_back(current);
                    current = "";
                } else {
                    current += c;
                }
            }
            tokens.push_back(current);

            if (tokens.size() >= 1) {
                string file_username = normalize_username(unescape_csv(tokens[0]));
                if (file_username == normalized_username) {
                    in.close();
                    return true;
                }
            }
        }
        in.close();


        in.open(USER_DATA_FILE.c_str());
        if (in.is_open()) {
            first = true;
            string lower_input = "";
            for (int i = 0; i < (int)normalized_username.size(); ++i) {
                char c = normalized_username[i];
                if (c >= 'A' && c <= 'Z') {
                    c = (char)(c - 'A' + 'a');
                }
                lower_input += c;
            }

            while (getline(in, line)) {
                if (line.size() == 0) continue;
                if (first) {
                    first = false;
                    continue;
                }

                Vector<string> tokens;
                string current = "";
                bool escaped = false;
                for (int i = 0; i < (int)line.size(); ++i) {
                    char c = line[i];
                    if (escaped) {
                        current += c;
                        escaped = false;
                    } else if (c == '\\') {
                        escaped = true;
                    } else if (c == ',') {
                        tokens.push_back(current);
                        current = "";
                    } else {
                        current += c;
                    }
                }
                tokens.push_back(current);

                if (tokens.size() >= 1) {
                    string file_username = normalize_username(unescape_csv(tokens[0]));
                    string file_lower = "";
                    for (int i = 0; i < (int)file_username.size(); ++i) {
                        char c = file_username[i];
                        if (c >= 'A' && c <= 'Z') {
                            c = (char)(c - 'A' + 'a');
                        }
                        file_lower += c;
                    }
                    if (file_lower == lower_input) {
                        in.close();
                        return true;
                    }
                }
            }
            in.close();
        }

        return false;
    }


    bool email_exists(const string& email) {

        string normalized_email = "";
        int start = 0;
        while (start < (int)email.size() && (email[start] == ' ' || email[start] == '\t')) {
            start++;
        }
        int end = (int)email.size() - 1;
        while (end >= start && (email[end] == ' ' || email[end] == '\t')) {
            end--;
        }
        for (int i = start; i <= end; ++i) {
            char c = email[i];
            if (c >= 'A' && c <= 'Z') {
                c = (char)(c - 'A' + 'a');
            }
            normalized_email += c;
        }


        for (int i = 0; i < username_list.size(); ++i) {
            if (users.contains(username_list[i])) {
                User& u = users[username_list[i]];
                string user_email = "";
                for (int j = 0; j < (int)u.email.size(); ++j) {
                    char c = u.email[j];
                    if (c >= 'A' && c <= 'Z') {
                        c = (char)(c - 'A' + 'a');
                    }
                    user_email += c;
                }
                if (user_email == normalized_email) {
                    return true;
                }
            }
        }
        return false;
    }


    bool get_user_info(const string& username, User& out_user) {
        string normalized_username = normalize_username(username);
        if (!users.contains(normalized_username)) {
            return false;
        }
        out_user = users[normalized_username];
        return true;
    }


    bool update_user_info(const string& username, const User& updated_user) {
        string normalized_username = normalize_username(username);
        if (!users.contains(normalized_username)) {
            return false;
        }

        User updated = updated_user;
        updated.username = normalized_username;
        users.insert(normalized_username, updated);


        bool found = false;
        for (int i = 0; i < username_list.size(); ++i) {
            if (username_list[i] == normalized_username) {
                found = true;
                break;
            }
        }
        if (!found) {
            username_list.push_back(normalized_username);
        }

        save_users();
        return true;
    }


    bool save() {
        return save_users();
    }


    bool reload() {
        return load_users();
    }


    int get_user_count() {
        return users.size();
    }


    Vector<string> get_all_usernames() {
        Vector<string> result;

        for (int i = 0; i < username_list.size(); ++i) {
            if (users.contains(username_list[i])) {
                result.push_back(username_list[i]);
            }
        }


        if (result.size() == 0) {
            ifstream in(USER_DATA_FILE.c_str());
            if (in.is_open()) {
                string line;
                bool first = true;
                while (getline(in, line)) {
                    if (line.size() == 0) {
                        continue;
                    }
                    if (first) {
                        first = false;
                        continue;
                    }


                    Vector<string> tokens;
                    string current = "";
                    bool escaped = false;
                    for (int i = 0; i < (int)line.size(); ++i) {
                        char c = line[i];
                        if (escaped) {
                            current += c;
                            escaped = false;
                        } else if (c == '\\') {
                            escaped = true;
                        } else if (c == ',') {
                            tokens.push_back(current);
                            current = "";
                            if (tokens.size() >= 1) {
                                break;
                            }
                        } else {
                            current += c;
                        }
                    }
                    if (tokens.size() >= 1) {
                        string username = normalize_username(unescape_csv(tokens[0]));
                        if (!username.empty()) {
                            bool found = false;
                            for (int j = 0; j < result.size(); ++j) {
                                if (result[j] == username) {
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) {
                                result.push_back(username);
                            }
                        }
                    }
                }
                in.close();
            }
        }
        return result;
    }
};

const string UserManager::USER_DATA_FILE = "users.dat";

#endif

