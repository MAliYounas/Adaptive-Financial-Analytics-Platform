#ifndef CSV_PARSER_H
#define CSV_PARSER_H
#include "Vector.h"
#include "Stock.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
class CsvParser {
private:
    static string trim(const string& s) {
        int start = 0;
        while (start < (int)s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) {
            start++;
        }
        int end = (int)s.size() - 1;
        while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
            end--;
        }
        if (end < start) {
            return "";
        }
        return s.substr(start, end - start + 1);
    }

    static string remove_commas(const string& s) {
        string out = "";
        for (int i = 0; i < (int)s.size(); ++i) {
            if (s[i] != ',') {
                out.push_back(s[i]);
            }
        }
        return out;
    }

    static double to_double(const string& token, bool& ok) {
        ok = true;
        string cleaned = trim(token);
        cleaned = remove_commas(cleaned);
        if (cleaned.size() == 0) {
            ok = false;
            return 0.0;
        }
        std::istringstream iss(cleaned);
        double v = 0.0;
        iss >> v;
        if (iss.fail()) {
            ok = false;
            return 0.0;
        }
        return v;
    }

    static Vector<string> split(const string& line) {
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
        return tokens;
    }

public:
    static bool parse(const string& path, Vector<Stock>& out_stocks, Vector<string>& header_out) {
        std::ifstream in(path.c_str());
        if (!in.is_open()) {
            return false;
        }
        static bool seeded = false;
        if (!seeded) {
            std::srand((unsigned int)std::time(NULL));
            seeded = true;
        }
        string line;
        bool first = true;
        while (std::getline(in, line)) {
            if (line.size() == 0) {
                continue;
            }
            Vector<string> cols = split(line);
            if (first) {
                for (int i = 0; i < cols.size(); ++i) {
                    header_out.push_back(trim(cols[i]));
                }
                first = false;
                continue;
            }
            if (cols.size() < 21) {
                continue;
            }
            Stock s;
            s.price = to_double(cols[1], s.valid);
            s.sector = trim(cols[2]);
            s.company_name = trim(cols[3]);
            s.year = 2022;
            bool ok = true;
            s.latest_eps = to_double(cols[4], ok);
            s.eps_last_quarter = to_double(cols[5], ok);
            s.last_annual_eps = to_double(cols[6], ok);
            s.pe = to_double(cols[7], ok);
            s.expected_pe = to_double(cols[8], ok);
            s.expected_growth = to_double(cols[9], ok);
            s.peg = to_double(cols[10], ok);
            s.book_value = to_double(cols[11], ok);
            s.expected_book_value = to_double(cols[12], ok);
            s.pb = to_double(cols[13], ok);
            s.expected_pb = to_double(cols[14], ok);
            s.roe = to_double(cols[15], ok);
            s.expected_roe = to_double(cols[16], ok);
            s.equity_to_asset = to_double(cols[17], ok);
            s.roa = to_double(cols[18], ok);
            s.last_dividend = to_double(cols[19], ok);
            s.expected_dividend = to_double(cols[20], ok);
            s.validate();
            if (!s.valid) {
                continue;
            }
            s.compute_derived();
            out_stocks.push_back(s);

            for (int year_offset = 1; year_offset <= 10; ++year_offset) {
                Stock sy = s;
                sy.year = 2022 + year_offset;

                double base = 1.0 + 0.03 * year_offset;
                double noise_price = (std::rand() / (double)RAND_MAX) * 0.10 - 0.05;
                double noise_eps = (std::rand() / (double)RAND_MAX) * 0.08 - 0.04;
                double noise_pe = (std::rand() / (double)RAND_MAX) * 0.06 - 0.03;
                double factor_price = base + noise_price;
                double factor_eps = base + noise_eps;
                double factor_pe = base + noise_pe;

                sy.price = s.price * factor_price;
                sy.latest_eps = s.latest_eps * factor_eps;
                sy.eps_last_quarter = s.eps_last_quarter * factor_eps;
                sy.last_annual_eps = s.last_annual_eps * factor_eps;
                sy.pe = s.pe * factor_pe;
                sy.expected_pe = s.expected_pe * factor_pe;
                sy.expected_growth = s.expected_growth * factor_eps;
                sy.peg = s.peg * factor_pe;
                sy.book_value = s.book_value * factor_price;
                sy.expected_book_value = s.expected_book_value * factor_price;
                sy.pb = s.pb * factor_pe;
                sy.expected_pb = s.expected_pb * factor_pe;
                sy.roe = s.roe * factor_eps;
                sy.expected_roe = s.expected_roe * factor_eps;
                sy.equity_to_asset = s.equity_to_asset;
                sy.roa = s.roa * factor_eps;
                sy.last_dividend = s.last_dividend * factor_price;
                sy.expected_dividend = s.expected_dividend * factor_price;
                sy.validate();
                if (sy.valid) {
                    sy.compute_derived();
                    out_stocks.push_back(sy);
                }
            }
        }
        in.close();
        return true;
    }
};

#endif

