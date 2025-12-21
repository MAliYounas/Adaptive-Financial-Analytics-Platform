#include <iostream>
#include <string>
#include <conio.h>
#include <limits>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "DataStore.h"
#include "DoublyLinkedList.h"
#include "User.h"
#include <unordered_map>
#include <vector>

using namespace std;

struct Currency {
    string code;
    string name;
    double rate_to_usd;
};

static const string COLOR_RESET = "\033[0m";
static const string COLOR_HIGHLIGHT = "\033[1;32m";
static const string COLOR_DIM = "\033[2m";
static const string COLOR_TITLE = "\033[1;36m";
static const string COLOR_WARN = "\033[1;33m";
static const string COLOR_ERROR = "\033[1;31m";
static const string COLOR_SUCCESS = "\033[1;32m";

void print_stock(Stock* s, double current_price = -1.0) {
    if (s == nullptr) {
        cout << COLOR_WARN << "Not found" << COLOR_RESET << endl;
        return;
    }
    cout << COLOR_HIGHLIGHT << s->company_name << COLOR_RESET
         << " | Sector: " << s->sector;
    if (current_price > 0.0 && abs(current_price - s->price) > 0.01) {
        cout << " | Current Price: " << COLOR_HIGHLIGHT << "$" << fixed << setprecision(2) << current_price << COLOR_RESET
             << " (Base: $" << fixed << setprecision(2) << s->price << ")";
    } else {
        cout << " | Price: " << s->price;
    }
    cout << " | P/E: " << s->pe
         << " | ROE: " << s->roe
         << " | DivYield: " << s->dividend_yield
         << endl;
}

void list_stocks(Vector<Stock*>& v, int limit = 10) {
    int n = v.size();
    if (limit > n) {
        limit = n;
    }
    for (int i = 0; i < limit; ++i) {
        print_stock(v[i]);
    }
}

string read_line(const string& prompt) {
    cout << prompt;
    string s;
    if (cin.peek() == '\n') {
        cin.ignore();
    }
    getline(cin, s);
    return s;
}


string normalize_username(const string& username) {
    int start = 0;
    while (start < (int)username.size() && (username[start] == ' ' || username[start] == '\t')) {
        start++;
    }
    int end = (int)username.size() - 1;
    while (end >= start && (username[end] == ' ' || username[end] == '\t')) {
        end--;
    }
    if (start <= end) {
        return username.substr(start, end - start + 1);
    }
    return "";
}

void clear_input_buffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

Vector<Stock*> all_ptrs(DataStore& store) {
    Vector<Stock*> v;
    for (int i = 0; i < store.all_stocks.size(); ++i) {
        v.push_back(&store.all_stocks[i]);
    }
    return v;
}

Vector<Stock*> unique_latest_companies(DataStore& store) {
    Vector<Stock*> result;
    for (int i = 0; i < store.all_stocks.size(); ++i) {
        Stock* s = &store.all_stocks[i];
        bool found = false;
        for (int j = 0; j < result.size(); ++j) {
            if (result[j]->company_name == s->company_name) {
                if (s->year > result[j]->year) {
                    result[j] = s;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            result.push_back(s);
        }
    }
    return result;
}

int pick_company_index(DataStore& store, const string& title);

void collect_company_history(DataStore& store, int base_index, Vector<Stock*>& series) {
    series.clear();
    if (base_index < 0 || base_index >= store.all_stocks.size()) {
        return;
    }
    string name = store.all_stocks[base_index].company_name;
    for (int i = 0; i < store.all_stocks.size(); ++i) {
        if (store.all_stocks[i].company_name == name) {
            series.push_back(&store.all_stocks[i]);
        }
    }
    int n = series.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (series[j]->year < series[i]->year) {
                Stock* tmp = series[i];
                series[i] = series[j];
                series[j] = tmp;
            }
        }
    }
}

void export_company_history_to_csv(DataStore& store, int base_index, int predicted_year, double predicted_price, const string& file_path) {
    Vector<Stock*> history;
    collect_company_history(store, base_index, history);
    std::ofstream out(file_path.c_str());
    if (!out.is_open()) {
        return;
    }
    out << "year,price,kind\n";
    for (int i = 0; i < history.size(); ++i) {
        out << history[i]->year << "," << history[i]->price << ",actual\n";
    }
    out << predicted_year << "," << predicted_price << ",predicted\n";
    out.close();
}

void render_header() {
    system("cls");
    cout
        << "\033[1;35m=== "
        << "\033[1;36mAdaptive "
        << "\033[1;32mFinancial "
        << "\033[1;34mAnalytics "
        << "\033[1;31mPlatform "
        << "\033[1;35m===\033[0m"
        << endl;
}

void render_menu(Vector<string>& items, int selected) {
    render_header();
    cout << COLOR_DIM << "Use Up/Down arrows, Enter to select, q to quit" << COLOR_RESET << endl;
    for (int i = 0; i < items.size(); ++i) {
        if (i == selected) {
            cout << COLOR_HIGHLIGHT << "> " << items[i] << COLOR_RESET << endl;
        } else {
            cout << "  " << items[i] << endl;
        }
    }
}

void browse_companies(DataStore& store) {
    Vector<Stock*> companies = unique_latest_companies(store);
    if (companies.size() == 0) {
        cout << COLOR_WARN << "No companies loaded. Please load CSV first." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    int selected = 0;
    int offset = 0;
    int page_size = 12;
    bool browsing = true;

    while (browsing) {
        render_header();
        cout << COLOR_DIM << "Browse Companies (Up/Down to move, Enter to view, q to go back)" << COLOR_RESET << endl;
        cout << endl;

        int total = companies.size();
        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            Stock* s = companies[i];
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << s->company_name << "  (" << s->sector << ")" << COLOR_RESET << endl;
            } else {
                cout << "  " << s->company_name << "  (" << s->sector << ")" << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            browsing = false;
        } else if (ch == 13) {
            render_header();
            cout << COLOR_DIM << "Company details (press any key to return)" << COLOR_RESET << endl << endl;
            print_stock(companies[selected]);
            cout << "\nPress any key to go back to list...";
            _getch();
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
}

void browse_sorted_list(Vector<Stock*>& companies, const string& title) {
    if (companies.size() == 0) {
        cout << COLOR_WARN << "No companies to show." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    int selected = 0;
    int offset = 0;
    int page_size = 12;
    bool browsing = true;

    while (browsing) {
        render_header();
        cout << COLOR_DIM << title << " (Up/Down to move, Enter to view, q to go back)" << COLOR_RESET << endl;
        cout << endl;

        int total = companies.size();
        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            Stock* s = companies[i];
            int order = i + 1;
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << order << ". " << s->company_name << "  (" << s->sector << ")" << COLOR_RESET << endl;
            } else {
                cout << " " << order << ". " << s->company_name << "  (" << s->sector << ")" << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            browsing = false;
        } else if (ch == 13) {
            render_header();
            cout << COLOR_DIM << "Company details (press any key to return)" << COLOR_RESET << endl << endl;
            print_stock(companies[selected]);
            cout << "\nPress any key to go back to list...";
            _getch();
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
}

void show_advanced_company_analysis(DataStore& store) {
    int idx = pick_company_index(store, "Select Company for Advanced Analysis");
    if (idx < 0) {
        return;
    }
    Vector<Stock*> history;
    collect_company_history(store, idx, history);
    if (history.size() == 0) {
        cout << COLOR_WARN << "No history available for this company." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    Stock* latest = history[history.size() - 1];
    string trend = store.trend_flag(latest);

    double first_price = history[0]->price;
    double last_price = latest->price;
    int first_year = history[0]->year;
    int last_year = latest->year;
    int years_span = last_year - first_year;

    double cagr = 0.0;
    if (years_span > 0 && first_price > 0.0 && last_price > 0.0) {
        double ratio = last_price / first_price;
        double step = 1.0 / (double)years_span;
        double val = 1.0;
        double low = 0.0;
        double high = ratio;
        for (int it = 0; it < 40; ++it) {
            double mid = (low + high) * 0.5;
            double pow_val = 1.0;
            for (int k = 0; k < years_span; ++k) {
                pow_val = pow_val * (1.0 + mid);
            }
            if (pow_val > ratio) {
                high = mid;
            } else {
                low = mid;
            }
        }
        cagr = (low + high) * 0.5;
    }

    int ret_count = history.size() - 1;
    double sum_ret = 0.0;
    double sum_ret_sq = 0.0;
    double min_ret = 0.0;
    double max_ret = 0.0;
    if (ret_count > 0) {
        for (int i = 0; i < ret_count; ++i) {
            double p0 = history[i]->price;
            double p1 = history[i + 1]->price;
            if (p0 <= 0.0) {
                continue;
            }
            double r = (p1 - p0) / p0;
            sum_ret = sum_ret + r;
            sum_ret_sq = sum_ret_sq + r * r;
            if (i == 0 || r < min_ret) {
                min_ret = r;
            }
            if (i == 0 || r > max_ret) {
                max_ret = r;
            }
        }
    }

    double avg_ret = 0.0;
    double vol_ret = 0.0;
    if (ret_count > 0) {
        avg_ret = sum_ret / (double)ret_count;
        double mean_sq = sum_ret_sq / (double)ret_count;
        double sq_mean = avg_ret * avg_ret;
        double var = mean_sq - sq_mean;
        if (var < 0.0) {
            var = 0.0;
        }
        double x = var;
        double g = x;
        double eps = 0.000001;
        if (x > 0.0) {
            double guess = x;
            for (int it = 0; it < 30; ++it) {
                guess = 0.5 * (guess + x / guess);
            }
            g = guess;
        } else {
            g = 0.0;
        }
        vol_ret = g;
    }

    double sum_div_yield = 0.0;
    for (int i = 0; i < history.size(); ++i) {
        sum_div_yield = sum_div_yield + history[i]->dividend_yield;
    }
    double avg_div_yield = sum_div_yield / (double)history.size();

    render_header();
    cout << COLOR_DIM << "Advanced Company Analysis" << COLOR_RESET << endl << endl;
    cout << "Company: " << COLOR_HIGHLIGHT << latest->company_name << COLOR_RESET << endl;
    cout << "Sector : " << latest->sector << endl;
    cout << "Years  : " << first_year << " - " << last_year << " (" << history.size() << " points)" << endl;
    cout << "Trend  : " << trend << endl << endl;

    cout << "Price first / last : " << first_price << " -> " << last_price << endl;
    cout << "CAGR (approx)      : " << cagr * 100.0 << "% per year" << endl;
    cout << "Avg YoY return     : " << avg_ret * 100.0 << "% per year" << endl;
    cout << "Volatility (stddev): " << vol_ret * 100.0 << "% per year" << endl;
    cout << "Best / worst year  : " << max_ret * 100.0 << "% / " << min_ret * 100.0 << "%\n";
    cout << "Avg dividend yield : " << avg_div_yield << "%\n";

    cout << "\nPress any key to continue...";
    _getch();
}

int pick_option(Vector<string>& items, const string& title) {
    if (items.size() == 0) {
        return -1;
    }
    int selected = 0;
    int offset = 0;
    int page_size = 12;
    bool choosing = true;

    while (choosing) {
        render_header();
        cout << COLOR_DIM << title << " (Up/Down to move, Enter to select, q to cancel)" << COLOR_RESET << endl;
        cout << endl;

        int total = items.size();
        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            string& label = items[i];
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << label << COLOR_RESET << endl;
            } else {
                cout << "  " << label << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            return -1;
        } else if (ch == 13) {
            return selected;
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
    return -1;
}

bool pick_sector(DataStore& store, string& out_sector, const string& title) {
    Vector<Stock*> companies = unique_latest_companies(store);
    Vector<string> sectors;
    for (int i = 0; i < companies.size(); ++i) {
        string sec = companies[i]->sector;
        bool exists = false;
        for (int j = 0; j < sectors.size(); ++j) {
            if (sectors[j] == sec) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            sectors.push_back(sec);
        }
    }
    if (sectors.size() == 0) {
        cout << COLOR_WARN << "No sectors available. Please load CSV first." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }
    int idx = pick_option(sectors, title);
    if (idx < 0) {
        return false;
    }
    out_sector = sectors[idx];
    return true;
}

int pick_company_index(DataStore& store, const string& title) {
    Vector<Stock*> companies = unique_latest_companies(store);
    if (companies.size() == 0) {
        cout << COLOR_WARN << "No companies loaded. Please load CSV first." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return -1;
    }

    int selected = 0;
    int offset = 0;
    int page_size = 12;

    while (true) {
        render_header();
        cout << COLOR_DIM << title << " (Up/Down to move, Enter to select, q to cancel)" << COLOR_RESET << endl;
        cout << endl;

        int total = companies.size();
        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            Stock* s = companies[i];
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << s->company_name << "  (" << s->sector << ")" << COLOR_RESET << endl;
            } else {
                cout << "  " << s->company_name << "  (" << s->sector << ")" << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            return -1;
        } else if (ch == 13) {
            Stock* s = companies[selected];
            int idx = (int)(s - &store.all_stocks[0]);
            return idx;
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
    return -1;
}

int pick_company_index_subset(DataStore& store, Vector<int>& indices, const string& title) {
    int total = indices.size();
    if (total == 0) {
        cout << COLOR_WARN << "No companies available." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return -1;
    }

    int selected = 0;
    int offset = 0;
    int page_size = 12;

    while (true) {
        render_header();
        cout << COLOR_DIM << title << " (Up/Down to move, Enter to select, q to cancel)" << COLOR_RESET << endl;
        cout << endl;

        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            int idx = indices[i];
            Stock* s = &store.all_stocks[idx];
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << s->company_name << "  (" << s->sector << ")" << COLOR_RESET << endl;
            } else {
                cout << "  " << s->company_name << "  (" << s->sector << ")" << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            return -1;
        } else if (ch == 13) {
            return indices[selected];
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
    return -1;
}

Vector<Currency> load_currencies(const string& path) {
    Vector<Currency> currencies;
    ifstream in(path.c_str());
    if (!in.is_open()) {
        return currencies;
    }
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
        for (int i = 0; i < (int)line.size(); ++i) {
            char c = line[i];
            if (c == ',') {
                tokens.push_back(current);
                current = "";
            } else {
                current.push_back(c);
            }
        }
        tokens.push_back(current);
        if (tokens.size() >= 3) {
            Currency curr;
            curr.code = tokens[0];
            curr.name = tokens[1];
            stringstream ss(tokens[2]);
            ss >> curr.rate_to_usd;
            if (curr.rate_to_usd > 0.0) {
                currencies.push_back(curr);
            }
        }
    }
    in.close();
    return currencies;
}

int pick_currency(Vector<Currency>& currencies, const string& title) {
    if (currencies.size() == 0) {
        return -1;
    }
    int selected = 0;
    int offset = 0;
    int page_size = 12;
    bool choosing = true;

    while (choosing) {
        render_header();
        cout << COLOR_DIM << title << " (Up/Down to move, Enter to select, q to cancel)" << COLOR_RESET << endl;
        cout << endl;

        int total = currencies.size();
        if (page_size > total) {
            page_size = total;
        }
        if (offset < 0) {
            offset = 0;
        }
        if (offset > total - page_size) {
            offset = total - page_size;
        }

        for (int i = offset; i < offset + page_size; ++i) {
            Currency& c = currencies[i];
            string label = c.code + " - " + c.name;
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << label << COLOR_RESET << endl;
            } else {
                cout << "  " << label << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            return -1;
        } else if (ch == 13) {
            return selected;
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                if (selected > 0) {
                    selected--;
                    if (selected < offset) {
                        offset = selected;
                    }
                } else {
                    selected = total - 1;
                    if (total > page_size) {
                        offset = total - page_size;
                    } else {
                        offset = 0;
                    }
                }
            } else if (arrow == 80) {
                if (selected < total - 1) {
                    selected++;
                    if (selected >= offset + page_size) {
                        offset = selected - page_size + 1;
                    }
                } else {
                    selected = 0;
                    offset = 0;
                }
            }
        }
    }
    return -1;
}

double convert_currency(double amount, Currency& from, Currency& to) {
    double usd_amount = amount / from.rate_to_usd;
    return usd_amount * to.rate_to_usd;
}

void export_conversion_to_csv(double amount, Currency& from, Currency& to, double result, const string& file_path) {
    ofstream out(file_path.c_str());
    if (!out.is_open()) {
        return;
    }
    out << "currency,amount\n";
    out << from.code << "," << amount << "\n";
    out << to.code << "," << result << "\n";
    out.close();
}


string read_password(const string& prompt) {
    cout << prompt;
    string password = "";
    char ch;
    while ((ch = _getch()) != 13) {
        if (ch == 8) {
            if (password.size() > 0) {
                password.pop_back();
                cout << "\b \b";
            }
        } else if (ch >= 32 && ch <= 126) {
            password += ch;
            cout << "*";
        }
    }
    cout << endl;
    return password;
}

bool signup_user(UserManager& user_manager, string& logged_in_username) {
    render_header();
    cout << COLOR_DIM << "User Signup" << COLOR_RESET << endl << endl;

    string username = read_line("Enter username: ");
    if (username.empty()) {
        cout << COLOR_ERROR << "Username cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }


    string normalized_username = normalize_username(username);
    if (normalized_username.empty()) {
        cout << COLOR_ERROR << "Username cannot be empty or only whitespace!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }


    if (user_manager.username_exists(normalized_username)) {
        cout << COLOR_ERROR << "Username already exists! Please choose another." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string password = read_password("Enter password: ");
    if (password.empty()) {
        cout << COLOR_ERROR << "Password cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    if (password.size() < 4) {
        cout << COLOR_ERROR << "Password must be at least 4 characters long!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string confirm_password = read_password("Confirm password: ");
    if (password != confirm_password) {
        cout << COLOR_ERROR << "Passwords do not match!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string email = read_line("Enter email: ");
    if (email.empty()) {
        cout << COLOR_ERROR << "Email cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }


    if (user_manager.email_exists(email)) {
        cout << COLOR_ERROR << "Email already registered! Please use a different email." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string full_name = read_line("Enter full name: ");
    if (full_name.empty()) {
        cout << COLOR_ERROR << "Full name cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string age_str = read_line("Enter age: ");
    stringstream ss(age_str);
    int age = 0;
    ss >> age;
    if (age <= 0 || ss.fail()) {
        cout << COLOR_WARN << "Invalid age. Setting to 0." << COLOR_RESET << endl;
        age = 0;
    }

    bool signup_result = user_manager.signup(normalized_username, password, email, full_name, age);
    if (signup_result) {

        User user_info;
        if (user_manager.get_user_info(normalized_username, user_info)) {
            cout << COLOR_SUCCESS << "\nSignup successful! Account created for " << user_info.full_name << "!" << COLOR_RESET << endl;
        } else {
            cout << COLOR_SUCCESS << "\nSignup successful! Account created for " << normalized_username << "!" << COLOR_RESET << endl;
        }
        cout << COLOR_DIM << "Please login to continue." << COLOR_RESET << endl;
        cout << "\nPress any key to return to login page...";
        _getch();
        return false;
    } else {

        if (user_manager.username_exists(normalized_username)) {
            cout << COLOR_ERROR << "\nSignup failed! Username '" << normalized_username << "' already exists." << COLOR_RESET << endl;
        } else {
            cout << COLOR_ERROR << "\nSignup failed! Could not save user data. Please try again." << COLOR_RESET << endl;
        }
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }
}

bool login_user(UserManager& user_manager, string& logged_in_username) {
    render_header();
    cout << COLOR_DIM << "User Login" << COLOR_RESET << endl << endl;

    string username = read_line("Enter username: ");
    if (username.empty()) {
        cout << COLOR_ERROR << "Username cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }

    string password = read_password("Enter password: ");
    if (password.empty()) {
        cout << COLOR_ERROR << "Password cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }


    int start = 0;
    while (start < (int)username.size() && (username[start] == ' ' || username[start] == '\t')) {
        start++;
    }
    int end = (int)username.size() - 1;
    while (end >= start && (username[end] == ' ' || username[end] == '\t')) {
        end--;
    }
    if (start <= end) {
        username = username.substr(start, end - start + 1);
    }


    string normalized_username = normalize_username(username);


    if (user_manager.login(normalized_username, password)) {

        User user_info;
        if (user_manager.get_user_info(normalized_username, user_info)) {
            logged_in_username = user_info.username;
            cout << COLOR_SUCCESS << "\nLogin successful! Welcome back, " << user_info.full_name << "!" << COLOR_RESET << endl;
        } else {
            logged_in_username = normalized_username;
            cout << COLOR_SUCCESS << "\nLogin successful! Welcome back, " << normalized_username << "!" << COLOR_RESET << endl;
        }
        cout << "\nPress any key to continue...";
        _getch();
        return true;
    } else {

        if (!user_manager.username_exists(normalized_username)) {
            cout << COLOR_ERROR << "\nLogin failed! Username '" << normalized_username << "' not found." << COLOR_RESET << endl;
            cout << "Debug: Total users in system: " << user_manager.get_user_count() << endl;


            Vector<string> all_usernames = user_manager.get_all_usernames();
            if (all_usernames.size() > 0) {
                cout << COLOR_DIM << "Available usernames in system:" << COLOR_RESET << endl;
                for (int i = 0; i < all_usernames.size(); ++i) {
                    cout << COLOR_DIM << "  - '" << all_usernames[i] << "' (length: " << all_usernames[i].size() << ")" << COLOR_RESET << endl;

                    cout << COLOR_DIM << "    Characters: ";
                    for (int j = 0; j < (int)all_usernames[i].size(); ++j) {
                        if (all_usernames[i][j] >= 32 && all_usernames[i][j] <= 126) {
                            cout << all_usernames[i][j];
                        } else {
                            cout << "[" << (int)(unsigned char)all_usernames[i][j] << "]";
                        }
                    }
                    cout << COLOR_RESET << endl;
                }
            } else {
                cout << COLOR_WARN << "Warning: No usernames found. Users file might be empty or corrupted." << COLOR_RESET << endl;


                ifstream check_file("users.dat");
                if (check_file.is_open()) {
                    cout << COLOR_DIM << "File 'users.dat' exists. Checking contents..." << COLOR_RESET << endl;
                    string first_line;
                    getline(check_file, first_line);
                    cout << COLOR_DIM << "First line: " << first_line << COLOR_RESET << endl;
                    int line_count = 1;
                    while (getline(check_file, first_line)) {
                        line_count++;
                        if (line_count <= 3) {
                            cout << COLOR_DIM << "Line " << line_count << ": " << first_line.substr(0, 50) << "..." << COLOR_RESET << endl;
                        }
                    }
                    check_file.close();
                    cout << COLOR_DIM << "Total lines in file: " << line_count << COLOR_RESET << endl;
                } else {
                    cout << COLOR_WARN << "File 'users.dat' does not exist!" << COLOR_RESET << endl;
                    cout << COLOR_DIM << "This means no users have been saved yet. Please sign up first." << COLOR_RESET << endl;
                }

                cout << COLOR_DIM << "\nPlease try signing up again, or check if 'users.dat' file exists and has data." << COLOR_RESET << endl;
            }

            cout << COLOR_DIM << "\nNote: Make sure you're using the exact username you signed up with." << COLOR_RESET << endl;
        } else {
            cout << COLOR_ERROR << "\nLogin failed! Invalid password for username '" << normalized_username << "'." << COLOR_RESET << endl;
        }
        cout << "\nPress any key to continue...";
        _getch();
        return false;
    }
}

bool authenticate_user(UserManager& user_manager, string& logged_in_username) {
    Vector<string> auth_menu;
    auth_menu.push_back("Login");
    auth_menu.push_back("Sign Up");
    auth_menu.push_back("Exit");

    int selected = 0;
    bool authenticated = false;

    while (!authenticated) {
        render_header();
        cout << COLOR_DIM << "Welcome! Please login or sign up to continue." << COLOR_RESET << endl;
        cout << COLOR_DIM << "Use Up/Down arrows, Enter to select, q to quit" << COLOR_RESET << endl << endl;

        for (int i = 0; i < auth_menu.size(); ++i) {
            if (i == selected) {
                cout << COLOR_HIGHLIGHT << "> " << auth_menu[i] << COLOR_RESET << endl;
            } else {
                cout << "  " << auth_menu[i] << endl;
            }
        }

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            return false;
        }
        if (ch == 13 || ch == 10) {
            switch (selected) {
                case 0:
                    authenticated = login_user(user_manager, logged_in_username);
                    break;
                case 1:
                    authenticated = signup_user(user_manager, logged_in_username);
                    break;
                case 2:
                    return false;
            }
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                selected--;
                if (selected < 0) {
                    selected = auth_menu.size() - 1;
                }
            } else if (arrow == 80) {
                selected++;
                if (selected >= auth_menu.size()) {
                    selected = 0;
                }
            }
        } else if (ch == 'w' || ch == 'W') {

            selected--;
            if (selected < 0) {
                selected = auth_menu.size() - 1;
            }
        } else if (ch == 's' || ch == 'S') {
            selected++;
            if (selected >= auth_menu.size()) {
                selected = 0;
            }
        }
    }

    return authenticated;
}





struct SimStock {
    string name;
    double price;
};

struct SimTransaction {
    int step;
    string action;
    string company;
    double price;
    int quantity;
    double cash_after;
    int shares_after;
    double portfolio_value;
};

class TradingSim {
public:
    TradingSim(const string& user, double start_cash)
        : username(user), cash(start_cash), step(0), active_index(0) {
        srand((unsigned int)time(nullptr));
    }

    bool load_csv(const string& path) {
        ifstream in(path.c_str());
        if (!in.is_open()) return false;
        string line;
        bool header = true;
        while (getline(in, line)) {
            if (line.empty()) continue;
            if (header) { header = false; continue; }
            stringstream ss(line);
            vector<string> tokens;
            string tok;
            while (getline(ss, tok, ',')) tokens.push_back(tok);


            if (tokens.size() < 4) continue;
            string name = tokens[3];
            double price = 0.0;
            try { price = stod(tokens[1]); } catch (...) { price = 100.0; }
            double p = max(price, 0.01);
            stocks.push_back({name, p});
            prev_prices.push_back(p);
        }
        if (!stocks.empty()) active_index = 0;
        return !stocks.empty();
    }

    bool load_state() {
        string fname = "trading_state_" + username + ".txt";
        ifstream in(fname.c_str());
        if (!in.is_open()) return false;
        holdings.clear();
        string line;
        int line_no = 0;
        while (getline(in, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            if (line_no == 0) {
                ss >> cash;
            } else {
                string comp; int qty;
                if (ss >> comp >> qty) {
                    holdings[comp] = qty;
                }
            }
            line_no++;
        }
        return true;
    }

    bool save_state() {
        string fname = "trading_state_" + username + ".txt";
        ofstream out(fname.c_str());
        if (!out.is_open()) return false;
        out << cash << "\n";
        for (const auto& h : holdings) {
            out << h.first << " " << h.second << "\n";
        }
        return true;
    }

    void run_loop(int max_steps = 200) {
        if (stocks.empty()) {
            cout << COLOR_WARN << "No stocks loaded. Exiting simulation." << COLOR_RESET << endl;
            _getch();
            return;
        }

        load_state();

        int sel = active_index;
        for (step = 1; step <= max_steps; ++step) {
            update_prices();
            sel = trade_screen(sel);
            if (sel < 0) {

                liquidate_all();
                save_state();
                show_summary();
                cout << "\nPress any key to continue...";
                _getch();
                return;
            }
        }

        liquidate_all();
        save_state();
        show_summary();
        cout << "\nPress any key to continue...";
        _getch();
    }

private:
    double cash;
    int step;
    int active_index;
    string username;
    vector<SimStock> stocks;
    vector<double> prev_prices;
    unordered_map<string, int> holdings;
    vector<SimTransaction> history;
    DoublyLinkedList<string> recent_actions;

    void update_prices() {
        for (size_t i = 0; i < stocks.size(); ++i) {
            prev_prices[i] = stocks[i].price;
            double pct = (rand() % 101 - 50) / 1000.0;
            stocks[i].price = max(0.01, stocks[i].price * (1.0 + pct));
        }
    }

    string active_name() const {
        if (active_index >= 0 && active_index < (int)stocks.size()) return stocks[active_index].name;
        return "";
    }

    double current_price(const string& name) const {
        for (const auto& s : stocks) if (s.name == name) return s.price;
        return 0.0;
    }

    double last_change_pct(int idx) const {
        if (idx < 0 || idx >= (int)prev_prices.size()) return 0.0;
        double prev = prev_prices[idx];
        double cur = stocks[idx].price;
        if (prev <= 0.0) return 0.0;
        return ((cur - prev) / prev) * 100.0;
    }

    int trade_screen(int start_sel) {
        int selected = start_sel;
        int page_size = 10;
        int offset = 0;
        if (selected >= page_size) offset = selected - page_size / 2;
        while (true) {
            render_header();
            cout << COLOR_DIM << "Interactive Trading Simulation" << COLOR_RESET << endl << endl;
            cout << "Step: " << step << endl;
            cout << "Cash: $" << fixed << setprecision(2) << cash << endl;
            cout << "\nHoldings:\n";
            if (holdings.empty()) {
                cout << "  (none)\n";
            } else {
                for (const auto& h : holdings) {
                    double price = current_price(h.first);
                    cout << "  " << h.first << ": " << h.second
                         << " shares @ $" << fixed << setprecision(2) << price << endl;
                }
            }
            cout << "\nRecent actions (newest last):\n";
            if (recent_actions.size() == 0) {
                cout << "  (none)\n";
            } else {
                Vector<string> log = recent_actions.to_vector();
                int start = log.size() > 5 ? log.size() - 5 : 0;
                for (int i = start; i < log.size(); ++i) {
                    cout << "  " << log[i] << endl;
                }
            }
            cout << "\nUse Up/Down to browse, Enter/H to hold/next step, B buy, S sell, Q quit.\n\n";
            for (int i = 0; i < page_size && (offset + i) < (int)stocks.size(); ++i) {
                int idx = offset + i;
                auto& s = stocks[idx];
                double chg = last_change_pct(idx);
                bool is_sel = (idx == selected);
                cout << (is_sel ? COLOR_HIGHLIGHT + string("> ") : "  ");
                cout << "[" << idx << "] " << s.name
                     << " @ $" << fixed << setprecision(2) << s.price
                     << " (" << showpos << fixed << setprecision(2) << chg << "%" << noshowpos << ")";
                if (is_sel) cout << COLOR_RESET;
                cout << endl;
            }
            if (stocks.size() > (size_t)page_size) {
                cout << "\nShowing " << offset + 1 << "-" << min(offset + page_size, (int)stocks.size())
                     << " of " << stocks.size() << " companies.\n";
            }

            int ch = _getch();
            if (ch == 'q' || ch == 'Q') return -1;
            if (ch == 'h' || ch == 'H' || ch == 13) {
                return selected;
            }
            if (ch == 'b' || ch == 'B') {
                buy_selected(selected);
                continue;
            }
            if (ch == 's' || ch == 'S') {
                sell_selected(selected);
                continue;
            }
            if (ch == 224 || ch == 0) {
                int arrow = _getch();
                if (arrow == 72) {
                    selected = (selected - 1 + (int)stocks.size()) % (int)stocks.size();
                } else if (arrow == 80) {
                    selected = (selected + 1) % (int)stocks.size();
                }
                if (selected < offset) offset = selected;
                if (selected >= offset + page_size) offset = selected - page_size + 1;
            }
        }
        return selected;
    }

    void buy_selected(int sel) {
        if (sel < 0 || sel >= (int)stocks.size()) return;
        SimStock& s = stocks[sel];
        int qty = prompt_quantity("buy");
        if (qty <= 0) return;
        double cost = qty * s.price;
        if (cost > cash) {
            cout << COLOR_ERROR << "Not enough cash. Need $" << cost << ", have $" << cash << COLOR_RESET << endl;
            _getch();
            return;
        }
        cash -= cost;
        holdings[s.name] += qty;
        record("BUY", s.name, qty, s.price);
    }

    void sell_selected(int sel) {
        if (sel < 0 || sel >= (int)stocks.size()) return;
        const string& name = stocks[sel].name;
        int owned = holdings.count(name) ? holdings[name] : 0;
        if (owned <= 0) {
            cout << COLOR_WARN << "You own no shares of " << name << "." << COLOR_RESET << endl;
            _getch();
            return;
        }
        int qty = prompt_quantity("sell (max " + to_string(owned) + ")");
        if (qty <= 0 || qty > owned) {
            cout << COLOR_ERROR << "Invalid quantity." << COLOR_RESET << endl;
            _getch();
            return;
        }
        double price = current_price(name);
        double proceeds = qty * price;
        cash += proceeds;
        holdings[name] -= qty;
        if (holdings[name] == 0) holdings.erase(name);
        record("SELL", name, qty, price);
    }

    int prompt_quantity(const string& label) {
        cout << "Enter quantity to " << label << ": ";
        string in;
        getline(cin, in);
        stringstream ss(in);
        int q = 0;
        ss >> q;
        return q;
    }

    void record(const string& action, const string& company, int qty, double price) {
        double portfolio = cash;
        for (const auto& h : holdings) {
            portfolio += h.second * current_price(h.first);
        }
        int shares_after = holdings.count(company) ? holdings[company] : 0;
        history.push_back({step, action, company, price, qty, cash, shares_after, portfolio});
        stringstream ss;
        ss << action << " " << company << " x" << qty << " @ $" << fixed << setprecision(2) << price;
        recent_actions.push_back(ss.str());
        if (recent_actions.size() > 10) {
            recent_actions.pop_front();
        }
    }

    void show_summary() {
        double portfolio = cash;
        for (const auto& h : holdings) portfolio += h.second * current_price(h.first);
        render_header();
        cout << COLOR_DIM << "Simulation Summary" << COLOR_RESET << endl << endl;
        cout << "Final Cash: $" << fixed << setprecision(2) << cash << endl;
        cout << "Portfolio Value: $" << fixed << setprecision(2) << portfolio << endl;
        cout << "\nHoldings:\n";
        if (holdings.empty()) cout << "  (none)\n";
        else {
            for (const auto& h : holdings) {
                double price = current_price(h.first);
                cout << "  " << h.first << ": " << h.second << " shares @ $" << price << endl;
            }
        }
        cout << "\nTransactions: " << history.size() << endl;
        for (const auto& t : history) {
            cout << "Step " << t.step << " | " << t.action << " " << t.company
                 << " qty " << t.quantity << " @ $" << t.price
                 << " | Cash $" << t.cash_after
                 << " | Shares " << t.shares_after
                 << " | Port $" << t.portfolio_value << endl;
        }
    }

    void liquidate_all() {
        for (auto it = holdings.begin(); it != holdings.end(); ) {
            const string& name = it->first;
            int qty = it->second;
            double price = current_price(name);
            cash += qty * price;
            it = holdings.erase(it);
        }
    }
};


void run_stock_simulation(const string& username) {
    TradingSim sim(username, 10000.0);
    sim.load_csv("Pakistan_Stock_Exchange.csv");
    sim.run_loop(200);
}

void view_simulation_history(const string& username) {
    render_header();
    cout << COLOR_WARN << "History persistence not implemented for the interactive sim." << COLOR_RESET << endl;
    cout << "Run the simulation to see live transactions." << endl;
    cout << "\nPress any key to continue...";
    _getch();
}

void link_credit_card(UserManager& user_manager, const string& username) {
    render_header();
    cout << COLOR_DIM << "Link Credit Card" << COLOR_RESET << endl << endl;


    string normalized_username = normalize_username(username);
    if (normalized_username.empty()) {
        cout << COLOR_ERROR << "Invalid username!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    User user_info;
    if (!user_manager.get_user_info(normalized_username, user_info)) {
        cout << COLOR_ERROR << "User not found! Username: '" << normalized_username << "'" << COLOR_RESET << endl;
        cout << "Debug: User count in system: " << user_manager.get_user_count() << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    if (user_info.credit_card_linked) {
        cout << COLOR_WARN << "You already have a credit card linked." << COLOR_RESET << endl;
        cout << "Card: ****" << user_info.credit_card_number << endl;
        cout << "Name: " << user_info.credit_card_name << endl;
        cout << "Credit Limit: $" << fixed << setprecision(2) << user_info.credit_limit << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    string card_number = read_line("Enter credit card number (last 4 digits): ");
    if (card_number.size() != 4) {
        cout << COLOR_ERROR << "Please enter exactly 4 digits!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }


    bool valid = true;
    for (int i = 0; i < (int)card_number.size(); ++i) {
        if (card_number[i] < '0' || card_number[i] > '9') {
            valid = false;
            break;
        }
    }
    if (!valid) {
        cout << COLOR_ERROR << "Credit card number must contain only digits!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    string card_name = read_line("Enter name on card: ");
    if (card_name.empty()) {
        cout << COLOR_ERROR << "Card name cannot be empty!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }


    user_info.credit_card_number = card_number;
    user_info.credit_card_name = card_name;
    user_info.credit_limit = 5000.0;
    user_info.credit_card_linked = true;


    if (user_manager.update_user_info(normalized_username, user_info)) {
        cout << COLOR_SUCCESS << "\nCredit card linked successfully!" << COLOR_RESET << endl;
    } else {
        cout << COLOR_ERROR << "\nFailed to save credit card information!" << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }
    cout << "Basic Credit Limit: $" << fixed << setprecision(2) << user_info.credit_limit << endl;
    cout << "Visit a branch to extend your credit limit." << endl;
    cout << "\nPress any key to continue...";
    _getch();
}



void currency_converter() {
    Vector<Currency> currencies = load_currencies("currencies.csv");
    if (currencies.size() == 0) {
        cout << COLOR_WARN << "Failed to load currencies.csv. Please ensure the file exists." << COLOR_RESET << endl;
        cout << "\nPress any key to continue...";
        _getch();
        return;
    }

    while (true) {
        render_header();
        cout << COLOR_DIM << "Currency Converter (q to go back)" << COLOR_RESET << endl << endl;

        int from_idx = pick_currency(currencies, "Select Source Currency");
        if (from_idx < 0) {
            break;
        }
        Currency& from_curr = currencies[from_idx];

        int to_idx = pick_currency(currencies, "Select Target Currency");
        if (to_idx < 0) {
            continue;
        }
        Currency& to_curr = currencies[to_idx];

        if (from_idx == to_idx) {
            cout << COLOR_WARN << "Source and target currencies cannot be the same." << COLOR_RESET << endl;
            cout << "\nPress any key to continue...";
            _getch();
            continue;
        }

        render_header();
        cout << COLOR_DIM << "Currency Conversion" << COLOR_RESET << endl << endl;
        cout << "From: " << COLOR_HIGHLIGHT << from_curr.code << " - " << from_curr.name << COLOR_RESET << endl;
        cout << "To  : " << COLOR_HIGHLIGHT << to_curr.code << " - " << to_curr.name << COLOR_RESET << endl;
        cout << endl;

        string amount_str = read_line("Enter amount to convert: ");
        stringstream ss(amount_str);
        double amount = 0.0;
        ss >> amount;
        if (amount <= 0.0 || ss.fail()) {
            cout << COLOR_WARN << "Invalid amount. Please enter a positive number." << COLOR_RESET << endl;
            cout << "\nPress any key to continue...";
            _getch();
            continue;
        }

        double result = convert_currency(amount, from_curr, to_curr);

        render_header();
        cout << COLOR_DIM << "Conversion Result" << COLOR_RESET << endl << endl;
        cout << COLOR_HIGHLIGHT << amount << " " << from_curr.code << COLOR_RESET
             << " = " << COLOR_HIGHLIGHT << result << " " << to_curr.code << COLOR_RESET << endl;
        cout << endl;
        cout << "Exchange Rate: 1 " << from_curr.code << " = "
             << (to_curr.rate_to_usd / from_curr.rate_to_usd) << " " << to_curr.code << endl;
        cout << "Exchange Rate: 1 " << to_curr.code << " = "
             << (from_curr.rate_to_usd / to_curr.rate_to_usd) << " " << from_curr.code << endl;

        export_conversion_to_csv(amount, from_curr, to_curr, result, "currency_conversion_data.csv");
        cout << "\nLaunching matplotlib graph (requires Python + matplotlib)...";
        system("python plot_currency.py");

        cout << "\n\nPress any key to convert again (or q in menu to exit)...";
        _getch();
    }
}

int main() {

    UserManager user_manager;
    string logged_in_username = "";


    while (true) {

        if (!authenticate_user(user_manager, logged_in_username)) {
            cout << "\nExiting application. Goodbye!" << endl;
            return 0;
        }


        logged_in_username = normalize_username(logged_in_username);
        if (logged_in_username.empty()) {
            cout << COLOR_ERROR << "Invalid username! Exiting..." << COLOR_RESET << endl;
            return 1;
        }



        if (user_manager.get_user_count() == 0) {

            user_manager.reload();
        }



        DataStore store;
        string path = "Pakistan_Stock_Exchange.csv";
        store.load_csv(path);

        Vector<string> menu;
        menu.push_back("Load Data");
        menu.push_back("Search Company");
        menu.push_back("Filter by Sector");
        menu.push_back("Sort by Metric");
        menu.push_back("Predict Price (Linear Regression)");
        menu.push_back("Top N Recommendations");
        menu.push_back("Sector Analysis");
        menu.push_back("Advanced Company Analysis");
        menu.push_back("Currency Converter");
        menu.push_back("Stock Trading Simulation");
        menu.push_back("Logout");
        menu.push_back("Exit");

        int selected = 0;
        bool running = true;
        while (running) {
        render_menu(menu, selected);
        int ch = _getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        if (ch == 13) {
            switch (selected) {
                case 0: {
                    string p = read_line("CSV path (default Pakistan_Stock_Exchange.csv): ");
                    if (p.size() == 0) {
                        p = path;
                    }
                    if (!store.load_csv(p)) {
                        cout << COLOR_WARN << "Failed to load CSV" << COLOR_RESET << endl;
                    } else {
                        cout << COLOR_HIGHLIGHT << "Successfully loaded " << store.all_stocks.size() << " stocks" << COLOR_RESET << endl;
                    }
                    cout << "\nPress any key to continue...";
                    _getch();
                    break;
                }
                case 1: {
                    browse_companies(store);
                    break;
                }
                case 2: {
                    string sec;
                    if (!pick_sector(store, sec, "Select Sector for Filter")) {
                        break;
                    }
                    Vector<Stock*> raw = store.filter_by_sector(sec);
                    Vector<Stock*> v;
                    for (int i = 0; i < raw.size(); ++i) {
                        Stock* s = raw[i];
                        bool found = false;
                        for (int j = 0; j < v.size(); ++j) {
                            if (v[j]->company_name == s->company_name) {
                                if (s->year > v[j]->year) {
                                    v[j] = s;
                                }
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            v.push_back(s);
                        }
                    }
                    cout << "\nFound " << v.size() << " companies in sector: " << sec << endl;
                    if (v.size() > 0) {
                        list_stocks(v, 10);
                    }
                    cout << "\nPress any key to continue...";
                    _getch();
                    break;
                }
                case 3: {
                    Vector<string> metrics;
                    metrics.push_back("Price");
                    metrics.push_back("ROE");
                    metrics.push_back("P/E");
                    metrics.push_back("Expected Growth");
                    metrics.push_back("Dividend Yield");
                    int choice = pick_option(metrics, "Select Metric for Sorting");
                    if (choice < 0) {
                        break;
                    }
                    int m = 5;
                    string metric_name = "Price";
                    if (choice == 1) {
                        m = 3;
                        metric_name = "ROE";
                    } else if (choice == 2) {
                        m = 1;
                        metric_name = "P/E";
                    } else if (choice == 3) {
                        m = 6;
                        metric_name = "Expected Growth";
                    } else if (choice == 4) {
                        m = 7;
                        metric_name = "Dividend Yield";
                    }
                    Vector<Stock*> v = unique_latest_companies(store);
                    store.sort_by_metric(v, m);
                    string title = "Sorted by " + metric_name;
                    browse_sorted_list(v, title);
                    break;
                }
                case 4: {
                    Vector<Stock*> ds = all_ptrs(store);
                    if (ds.size() < 5) {
                        cout << COLOR_WARN << "Not enough data for linear regression (need at least 5 companies)." << COLOR_RESET << endl;
                        cout << "\nPress any key to continue...";
                        _getch();
                        break;
                    }
                    Vector<int> feats;
                    feats.push_back(0);
                    feats.push_back(1);
                    feats.push_back(2);
                    feats.push_back(3);
                    feats.push_back(4);
                    double r2 = 0.0;
                    double mse = 0.0;
                    double rmse = 0.0;
                    double mae = 0.0;
                    cout << "Training linear regression model on " << ds.size() << " records (multi-year data)...\n";
                    Vector<double> preds;
                    Vector<int> latest_indices;
                    LinearRegression<double> lr = store.train_linear_regression(feats, ds, 0.8, r2, mse, rmse, mae, latest_indices, preds);
                    cout << "R2 Score: " << r2 << " | MSE: " << mse << " | RMSE: " << rmse << " | MAE: " << mae << endl;
                    if (preds.size() == 0 || latest_indices.size() == 0 || preds.size() != latest_indices.size()) {
                        cout << COLOR_WARN << "\nWarning: model did not produce valid predictions." << COLOR_RESET << endl;
                        cout << "\nPress any key to continue...";
                        _getch();
                        break;
                    }
                    while (true) {
                        int idx = pick_company_index_subset(store, latest_indices, "Select Company for Next-Year Price Prediction");
                        if (idx < 0) {
                            break;
                        }
                        Stock* s = &store.all_stocks[idx];
                        double predicted = s->price;
                        for (int pi = 0; pi < latest_indices.size(); ++pi) {
                            if (latest_indices[pi] == idx) {
                                predicted = preds[pi];
                                break;
                            }
                        }
                        render_header();
                        cout << COLOR_DIM << "Predicted Next-Year Price Details" << COLOR_RESET << endl << endl;
                        cout << "Company: " << COLOR_HIGHLIGHT << s->company_name << COLOR_RESET << endl;
                        cout << "Sector : " << s->sector << endl;
                        cout << "Year   : " << s->year << endl;
                        cout << "Current Price   : " << s->price << endl;
                        cout << "Predicted Price : " << predicted << endl;
                        double diff = predicted - s->price;
                        cout << "Difference      : " << diff << endl;

                        export_company_history_to_csv(store, idx, s->year + 1, predicted, "company_graph_data.csv");
                        cout << "\nLaunching matplotlib graph (requires Python + matplotlib)...";
                        system("python plot_company.py");

                        cout << "\n\nPress any key to pick another company (or q in list to exit)...";
                        _getch();
                    }
                    break;
                }
                case 5: {
                    Vector<string> strategies;
                    strategies.push_back("Growth");
                    strategies.push_back("Value");
                    strategies.push_back("Income");
                    strategies.push_back("Balanced");
                    int s_choice = pick_option(strategies, "Select Recommendation Strategy");
                    if (s_choice < 0) {
                        break;
                    }
                    int strategy_id = s_choice;

                    Vector<string> topNOptions;
                    topNOptions.push_back("Top 5");
                    topNOptions.push_back("Top 10");
                    topNOptions.push_back("Top 20");
                    topNOptions.push_back("Top 50");
                    int n_choice = pick_option(topNOptions, "Select Top N");
                    if (n_choice < 0) {
                        break;
                    }
                    int n = 5;
                    if (n_choice == 1) {
                        n = 10;
                    } else if (n_choice == 2) {
                        n = 20;
                    } else if (n_choice == 3) {
                        n = 50;
                    }
                    Vector<Stock*> recs = store.recommend(strategy_id, store.all_stocks.size());
                    Vector<Stock*> unique_recs;
                    for (int i = 0; i < recs.size(); ++i) {
                        Stock* s = recs[i];
                        bool found = false;
                        for (int j = 0; j < unique_recs.size(); ++j) {
                            if (unique_recs[j]->company_name == s->company_name) {
                                if (s->year > unique_recs[j]->year) {
                                    unique_recs[j] = s;
                                }
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            unique_recs.push_back(s);
                        }
                    }
                    int available = unique_recs.size();
                    int show_n = n;
                    if (available < show_n) {
                        show_n = available;
                    }
                    cout << "\nFound " << available << " recommended companies, showing top " << show_n << ":\n";
                    Vector<Stock*> top_list;
                    for (int i = 0; i < show_n; ++i) {
                        top_list.push_back(unique_recs[i]);
                    }
                    string title = "Top " + std::to_string(show_n) + " Recommendations";
                    browse_sorted_list(top_list, title);
                    break;
                }
                case 6: {
                    string sec;
                    if (!pick_sector(store, sec, "Select Sector for Analysis")) {
                        break;
                    }
                    DataStore::SectorStats st = store.sector_stats(sec);
                    if (st.count > 0) {
                        cout << "\nSector Statistics for " << sec << ":\n";
                        cout << "  Companies: " << st.count << endl;
                        cout << "  Avg P/E: " << st.avg_pe << endl;
                        cout << "  Avg ROE: " << st.avg_roe << endl;
                        cout << "  Avg Dividend Yield: " << st.avg_div_yield << endl;
                        cout << "  Price Range: " << st.min_price << " - " << st.max_price << endl;
                    } else {
                        cout << COLOR_WARN << "Sector not found or has no data" << COLOR_RESET << endl;
                    }
                    cout << "\nPress any key to continue...";
                    _getch();
                    break;
                }
                case 7: {
                    show_advanced_company_analysis(store);
                    break;
                }
                case 8: {
                    currency_converter();
                    break;
                }
                case 9: {
                    run_stock_simulation(logged_in_username);
                    break;
                }
                case 10: {

                    render_header();
                    user_manager.save_users();
                    cout << COLOR_SUCCESS << "Data saved successfully!" << COLOR_RESET << endl;
                    cout << COLOR_DIM << "Logging out..." << COLOR_RESET << endl;
                    cout << "\nPress any key to return to login page...";
                    _getch();
                    logged_in_username = "";
                    running = false;
                    break;
                }
                case 11: {

                    user_manager.save_users();
                    running = false;
                    break;
                }
            }
        } else if (ch == 224 || ch == 0) {
            int arrow = _getch();
            if (arrow == 72) {
                selected--;
                if (selected < 0) {
                    selected = menu.size() - 1;
                }
            } else if (arrow == 80) {
                selected++;
                if (selected >= menu.size()) {
                    selected = 0;
                }
            }
        }
        }



        if (logged_in_username.empty()) {
            continue;
        } else {
            break;
        }
    }
    return 0;
}

