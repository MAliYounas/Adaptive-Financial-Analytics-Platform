#ifndef STOCK_H
#define STOCK_H

#include "Vector.h"
#include <string>

using namespace std;

struct Stock {
    string company_name;
    string sector;
    int year;
    double price;
    double latest_eps;
    double eps_last_quarter;
    double last_annual_eps;
    double pe;
    double expected_pe;
    double expected_growth;
    double peg;
    double book_value;
    double expected_book_value;
    double pb;
    double expected_pb;
    double roe;
    double expected_roe;
    double equity_to_asset;
    double roa;
    double last_dividend;
    double expected_dividend;

    double dividend_yield;
    double peg_ratio;
    double book_value_growth;
    double asset_return;

    bool valid;

    Stock() {
        company_name = "";
        sector = "";
        year = 0;
        price = 0.0;
        latest_eps = 0.0;
        eps_last_quarter = 0.0;
        last_annual_eps = 0.0;
        pe = 0.0;
        expected_pe = 0.0;
        expected_growth = 0.0;
        peg = 0.0;
        book_value = 0.0;
        expected_book_value = 0.0;
        pb = 0.0;
        expected_pb = 0.0;
        roe = 0.0;
        expected_roe = 0.0;
        equity_to_asset = 0.0;
        roa = 0.0;
        last_dividend = 0.0;
        expected_dividend = 0.0;
        dividend_yield = 0.0;
        peg_ratio = 0.0;
        book_value_growth = 0.0;
        asset_return = 0.0;
        valid = false;
    }

    void compute_derived() {
        if (price > 0.0 && last_dividend != 0.0) {
            dividend_yield = (last_dividend / price) * 100.0;
        } else {
            dividend_yield = 0.0;
        }
        if (expected_growth != 0.0) {
            peg_ratio = pe / expected_growth;
        } else {
            peg_ratio = 0.0;
        }
        if (book_value != 0.0) {
            book_value_growth = (expected_book_value - book_value) / book_value;
        } else {
            book_value_growth = 0.0;
        }
        asset_return = roe * equity_to_asset;
    }

    void validate() {
        valid = true;
        if (price <= 0.0) {
            valid = false;
        }
        if (pe < 0.0 || pe > 2000.0) {
            valid = false;
        }
        if (roe < -100.0 || roe > 100.0) {
            valid = false;
        }
        if (company_name.size() == 0) {
            valid = false;
        }
        if (sector.size() == 0) {
            valid = false;
        }
    }
};

#endif

