#ifndef DATA_STORE_H
#define DATA_STORE_H

#include "Vector.h"
#include "HashMap.h"
#include "AVLTree.h"
#include "MinHeap.h"
#include "MaxHeap.h"
#include "Stock.h"
#include "AdjacencyListGraph.h"
#include "CsvParser.h"
#include "Matrix.h"
#include "LinearRegression.h"
#include <string>
#include <cmath>

using namespace std;

struct StockPeKey {
    Stock* ref;
    bool operator<(const StockPeKey& other) {
        if (ref->pe == other.ref->pe) {
            return ref->company_name < other.ref->company_name;
        }
        return ref->pe < other.ref->pe;
    }
    bool operator>(const StockPeKey& other) {
        if (ref->pe == other.ref->pe) {
            return ref->company_name > other.ref->company_name;
        }
        return ref->pe > other.ref->pe;
    }
    bool operator<=(const StockPeKey& other) {
        return !(*this > other);
    }
    bool operator>=(const StockPeKey& other) {
        return !(*this < other);
    }
    bool operator==(const StockPeKey& other) {
        return ref == other.ref;
    }
};

struct StockRoeKey {
    Stock* ref;
    bool operator<(const StockRoeKey& other) {
        if (ref->roe == other.ref->roe) {
            return ref->company_name < other.ref->company_name;
        }
        return ref->roe < other.ref->roe;
    }
    bool operator>(const StockRoeKey& other) {
        if (ref->roe == other.ref->roe) {
            return ref->company_name > other.ref->company_name;
        }
        return ref->roe > other.ref->roe;
    }
    bool operator<=(const StockRoeKey& other) {
        return !(*this > other);
    }
    bool operator>=(const StockRoeKey& other) {
        return !(*this < other);
    }
    bool operator==(const StockRoeKey& other) {
        return ref == other.ref;
    }
};

class DataStore {
public:
    HashMap<string, Vector<Stock*>> sectors;
    HashMap<string, Stock*> by_name;
    AVLTree<StockPeKey> by_pe;
    MinHeap<StockPeKey> low_pe_heap;
    MaxHeap<StockRoeKey> high_roe_heap;
    AdjacencyListGraph<int> similarity_graph;
    Vector<Stock> all_stocks;

    DataStore() : sectors(), by_name(), by_pe(), low_pe_heap(), high_roe_heap(), similarity_graph(false), all_stocks() {
    }

    void clear() {
        sectors = HashMap<string, Vector<Stock*>>();
        by_name = HashMap<string, Stock*>();
        by_pe = AVLTree<StockPeKey>();
        low_pe_heap = MinHeap<StockPeKey>();
        high_roe_heap = MaxHeap<StockRoeKey>();
        similarity_graph = AdjacencyListGraph<int>(false);
        all_stocks.clear();
    }

    bool load_csv(const string& path) {
        clear();
        Vector<string> headers;
        bool ok = CsvParser::parse(path, all_stocks, headers);
        if (!ok) {
            return false;
        }
        for (int i = 0; i < all_stocks.size(); ++i) {
            Stock* ptr = &all_stocks[i];
            insert_stock(ptr);
        }
        build_similarity_graph();
        return true;
    }

    void insert_stock(Stock* s) {
        string name_key = normalize_key(s->company_name);
        by_name.insert(name_key, s);
        Vector<Stock*>* sector_vec_ptr;
        string sector_key = normalize_key(s->sector);
        if (sectors.contains(sector_key)) {
            sector_vec_ptr = &sectors[sector_key];
        } else {
            Vector<Stock*> new_vec;
            sectors.insert(sector_key, new_vec);
            sector_vec_ptr = &sectors[sector_key];
        }
        sector_vec_ptr->push_back(s);

        StockPeKey pe_key;
        pe_key.ref = s;
        by_pe.insert(pe_key);
        low_pe_heap.push(pe_key);

        StockRoeKey roe_key;
        roe_key.ref = s;
        high_roe_heap.push(roe_key);
    }

    string normalize_key(string s) {
        int start = 0;
        while (start < (int)s.size() && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) {
            start++;
        }
        int end = (int)s.size() - 1;
        while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
            end--;
        }
        string out = "";
        for (int i = start; i <= end; ++i) {
            char c = s[i];
            if (c >= 'a' && c <= 'z') {
                c = (char)(c - 'a' + 'A');
            }
            out.push_back(c);
        }
        return out;
    }

    Stock* search_by_name(const string& name) {
        string key = normalize_key(name);
        if (by_name.contains(key)) {
            return by_name[key];
        }
        return nullptr;
    }

    Vector<Stock*> filter_by_sector(const string& sector) {
        Vector<Stock*> result;
        string key = normalize_key(sector);
        for (int i = 0; i < all_stocks.size(); ++i) {
            string stock_key = normalize_key(all_stocks[i].sector);
            if (stock_key == key) {
                result.push_back(&all_stocks[i]);
            }
        }
        return result;
    }

    void collect_pe_range(double min_pe, double max_pe, Vector<Stock*>& out) {
        for (int i = 0; i < all_stocks.size(); ++i) {
            double v = all_stocks[i].pe;
            if (v >= min_pe && v <= max_pe) {
                out.push_back(&all_stocks[i]);
            }
        }
        if (out.size() > 1) {
            sort_by_metric(out, 1);
        }
    }

    Vector<Stock*> top_n_roe(int n) {
        Vector<Stock*> out;
        MaxHeap<StockRoeKey> copy = high_roe_heap;
        int count = 0;
        while (!copy.empty() && count < n) {
            StockRoeKey k = copy.top();
            copy.pop();
            out.push_back(k.ref);
            count++;
        }
        return out;
    }

    Vector<Stock*> lowest_n_pe(int n) {
        Vector<Stock*> out;
        MinHeap<StockPeKey> copy = low_pe_heap;
        int count = 0;
        while (!copy.empty() && count < n) {
            StockPeKey k = copy.top();
            copy.pop();
            out.push_back(k.ref);
            count++;
        }
        return out;
    }

    void sort_by_metric(Vector<Stock*>& arr, int metric_id) {
        quick_sort(arr, 0, arr.size() - 1, metric_id);
    }

    struct SectorStats {
        double avg_pe;
        double avg_roe;
        double avg_div_yield;
        double min_price;
        double max_price;
        int count;
    };

    SectorStats sector_stats(const string& sector) {
        SectorStats st;
        st.avg_pe = 0.0;
        st.avg_roe = 0.0;
        st.avg_div_yield = 0.0;
        st.min_price = 1e18;
        st.max_price = -1e18;
        st.count = 0;

        string key = normalize_key(sector);
        double sum_pe = 0.0;
        double sum_roe = 0.0;
        double sum_div = 0.0;

        for (int i = 0; i < all_stocks.size(); ++i) {
            string stock_key = normalize_key(all_stocks[i].sector);
            if (stock_key == key) {
                Stock* s = &all_stocks[i];
                sum_pe = sum_pe + s->pe;
                sum_roe = sum_roe + s->roe;
                sum_div = sum_div + s->dividend_yield;
                if (s->price < st.min_price) {
                    st.min_price = s->price;
                }
                if (s->price > st.max_price) {
                    st.max_price = s->price;
                }
                st.count = st.count + 1;
            }
        }

        if (st.count > 0) {
            st.avg_pe = sum_pe / (double)st.count;
            st.avg_roe = sum_roe / (double)st.count;
            st.avg_div_yield = sum_div / (double)st.count;
        }
        return st;
    }

    string compare_vs_sector(Stock* s) {
        if (s == nullptr) {
            return "No stock";
        }
        SectorStats st = sector_stats(s->sector);
        if (st.count == 0) {
            return "Sector data unavailable";
        }
        string res = "";
        res += "P/E: ";
        if (s->pe < st.avg_pe) {
            res += "below sector avg; ";
        } else {
            res += "above sector avg; ";
        }
        res += "ROE: ";
        if (s->roe < st.avg_roe) {
            res += "below sector avg; ";
        } else {
            res += "above sector avg; ";
        }
        res += "Dividend Yield: ";
        if (s->dividend_yield < st.avg_div_yield) {
            res += "below sector avg";
        } else {
            res += "above sector avg";
        }
        return res;
    }

    struct RecScore {
        double score;
        Stock* ref;
        bool operator<(const RecScore& other) {
            if (score == other.score) {
                return ref->company_name < other.ref->company_name;
            }
            return score < other.score;
        }
        bool operator>(const RecScore& other) {
            if (score == other.score) {
                return ref->company_name > other.ref->company_name;
            }
            return score > other.score;
        }
        bool operator<=(const RecScore& other) {
            return !(*this > other);
        }
        bool operator>=(const RecScore& other) {
            return !(*this < other);
        }
        bool operator==(const RecScore& other) {
            return ref == other.ref && score == other.score;
        }
    };

    Vector<Stock*> recommend(int strategy_id, int top_n) {
        double w_value = 0.25;
        double w_growth = 0.25;
        double w_health = 0.25;
        double w_div = 0.25;
        if (strategy_id == 0) {
            w_growth = 0.5;
            w_value = 0.1;
            w_health = 0.2;
            w_div = 0.2;
        }
        if (strategy_id == 1) {
            w_value = 0.5;
            w_growth = 0.15;
            w_health = 0.2;
            w_div = 0.15;
        }
        if (strategy_id == 2) {
            w_div = 0.5;
            w_value = 0.2;
            w_growth = 0.1;
            w_health = 0.2;
        }
        if (strategy_id == 3) {
            w_value = 0.25;
            w_growth = 0.25;
            w_health = 0.25;
            w_div = 0.25;
        }
        MaxHeap<RecScore> heap;
        for (int i = 0; i < all_stocks.size(); ++i) {
            Stock* s = &all_stocks[i];
            double value_score = 0.0;
            if (s->pe > 0.0) {
                value_score = 1.0 / s->pe;
            }
            double growth_score = s->expected_growth;
            double health_score = (s->roe * 0.5) + (s->equity_to_asset * 0.5);
            double dividend_score = 0.0;
            if (s->price > 0.0) {
                dividend_score = s->dividend_yield;
            }
            double total = w_value * value_score + w_growth * growth_score + w_health * health_score + w_div * dividend_score;
            RecScore r;
            r.score = total;
            r.ref = s;
            heap.push(r);
        }
        Vector<Stock*> out;
        int count = 0;
        while (!heap.empty() && count < top_n) {
            RecScore r = heap.top();
            heap.pop();
            out.push_back(r.ref);
            count++;
        }
        return out;
    }

    string trend_flag(Stock* s) {
        if (s == nullptr) {
            return "No stock";
        }
        int score = 0;
        if (s->expected_roe > s->roe) {
            score++;
        } else if (s->expected_roe < s->roe) {
            score--;
        }
        if (s->expected_pe < s->pe) {
            score++;
        } else if (s->expected_pe > s->pe) {
            score--;
        }
        if (s->expected_growth > 0.0) {
            score++;
        }
        if (score >= 2) {
            return "Improving";
        }
        if (score <= -1) {
            return "Declining";
        }
        return "Stable";
    }

    Vector<Stock*> similar(int index, int k) {
        Vector<Stock*> out;
        if (index < 0 || index >= all_stocks.size()) {
            return out;
        }
        Vector<Stock*> temp;
        for (int i = 0; i < all_stocks.size(); ++i) {
            if (i == index) {
                continue;
            }
            temp.push_back(&all_stocks[i]);
        }
        Vector<double> dists;
        for (int i = 0; i < temp.size(); ++i) {
            double dist = distance_between(&all_stocks[index], temp[i]);
            dists.push_back(dist);
        }
        for (int i = 0; i < temp.size() - 1; ++i) {
            for (int j = i + 1; j < temp.size(); ++j) {
                if (dists[i] > dists[j]) {
                    double td = dists[i];
                    dists[i] = dists[j];
                    dists[j] = td;
                    Stock* ts = temp[i];
                    temp[i] = temp[j];
                    temp[j] = ts;
                }
            }
        }
        for (int i = 0; i < temp.size() && i < k; ++i) {
            out.push_back(temp[i]);
        }
        return out;
    }

    LinearRegression<double> train_linear_regression(
        Vector<int>& feature_indices,
        Vector<Stock*>& dataset,
        double train_ratio,
        double& r2_out,
        double& mse_out,
        double& rmse_out,
        double& mae_out,
        Vector<int>& latest_indices_out,
        Vector<double>& preds_out) {
        int total = dataset.size();
        r2_out = 0.0;
        mse_out = 0.0;
        rmse_out = 0.0;
        mae_out = 0.0;
        latest_indices_out.clear();
        preds_out.clear();

        if (total == 0) {
            LinearRegression<double> lr_empty;
            return lr_empty;
        }

        Vector<string> seen_names;
        Vector<double> all_true;
        Vector<double> all_pred;

        for (int i = 0; i < total; ++i) {
            Stock* s = dataset[i];
            string name = s->company_name;
            bool exists = false;
            for (int k = 0; k < seen_names.size(); ++k) {
                if (seen_names[k] == name) {
                    exists = true;
                    break;
                }
            }
            if (exists) {
                continue;
            }
            seen_names.push_back(name);

            Vector<Stock*> series;
            for (int j = 0; j < total; ++j) {
                if (dataset[j]->company_name == name) {
                    series.push_back(dataset[j]);
                }
            }

            int len = series.size();
            for (int a = 0; a < len - 1; ++a) {
                for (int b = a + 1; b < len; ++b) {
                    if (series[b]->year < series[a]->year) {
                        Stock* tmp = series[a];
                        series[a] = series[b];
                        series[b] = tmp;
                    }
                }
            }

            if (len < 2) {
                continue;
            }

            double sum_x = 0.0;
            double sum_y = 0.0;
            double sum_xx = 0.0;
            double sum_xy = 0.0;
            for (int t = 0; t < len; ++t) {
                double x = (double)series[t]->year;
                double y = series[t]->price;
                sum_x = sum_x + x;
                sum_y = sum_y + y;
                sum_xx = sum_xx + x * x;
                sum_xy = sum_xy + x * y;
            }

            double n = (double)len;
            double denom = n * sum_xx - sum_x * sum_x;
            double slope = 0.0;
            double intercept = 0.0;
            if (denom != 0.0) {
                slope = (n * sum_xy - sum_x * sum_y) / denom;
                intercept = (sum_y - slope * sum_x) / n;
            } else {
                intercept = sum_y / n;
                slope = 0.0;
            }

            for (int t = 0; t < len; ++t) {
                double x = (double)series[t]->year;
                double y = series[t]->price;
                double y_hat = intercept + slope * x;
                all_true.push_back(y);
                all_pred.push_back(y_hat);
            }

            Stock* latest = series[len - 1];
            double next_year = (double)(latest->year + 1);
            double next_price = intercept + slope * next_year;

            int idx_latest = (int)(latest - &all_stocks[0]);
            latest_indices_out.push_back(idx_latest);
            preds_out.push_back(next_price);
        }

        int n_points = all_true.size();
        if (n_points == 0) {
            LinearRegression<double> lr_none;
            return lr_none;
        }

        double sum_y = 0.0;
        for (int i = 0; i < n_points; ++i) {
            sum_y = sum_y + all_true[i];
        }
        double mean_y = sum_y / (double)n_points;

        double ss_res = 0.0;
        double ss_tot = 0.0;
        double sum_abs = 0.0;
        for (int i = 0; i < n_points; ++i) {
            double err = all_true[i] - all_pred[i];
            ss_res = ss_res + err * err;
            if (err < 0.0) {
                sum_abs = sum_abs - err;
            } else {
                sum_abs = sum_abs + err;
            }
            double diff_tot = all_true[i] - mean_y;
            ss_tot = ss_tot + diff_tot * diff_tot;
        }

        if (ss_tot == 0.0) {
            r2_out = 1.0;
        } else {
            r2_out = 1.0 - (ss_res / ss_tot);
        }
        mse_out = ss_res / (double)n_points;
        rmse_out = sqrt(mse_out);
        mae_out = sum_abs / (double)n_points;

        LinearRegression<double> lr_dummy;
        return lr_dummy;
    }

private:
    double feature_value(Stock* s, int id) {
        if (id == 0) {
            return s->latest_eps;
        }
        if (id == 1) {
            return s->pe;
        }
        if (id == 2) {
            return s->book_value;
        }
        if (id == 3) {
            return s->roe;
        }
        if (id == 4) {
            return s->last_dividend;
        }
        if (id == 5) {
            return s->price;
        }
        if (id == 6) {
            return s->expected_growth;
        }
        if (id == 7) {
            return s->dividend_yield;
        }
        return 0.0;
    }

    void quick_sort(Vector<Stock*>& arr, int l, int r, int metric_id) {
        if (l >= r) {
            return;
        }
        int i = l;
        int j = r;
        double pivot = feature_value(arr[(l + r) / 2], metric_id);
        while (i <= j) {
            while (feature_value(arr[i], metric_id) < pivot) {
                i++;
            }
            while (feature_value(arr[j], metric_id) > pivot) {
                j--;
            }
            if (i <= j) {
                Stock* tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                i++;
                j--;
            }
        }
        if (l < j) {
            quick_sort(arr, l, j, metric_id);
        }
        if (i < r) {
            quick_sort(arr, i, r, metric_id);
        }
    }

    void build_similarity_graph() {
        similarity_graph = AdjacencyListGraph<int>(false, all_stocks.size());
        for (int i = 0; i < all_stocks.size(); ++i) {
            similarity_graph.set_vertex_data(i, i);
        }
        double threshold = 0.3;
        for (int i = 0; i < all_stocks.size(); ++i) {
            for (int j = i + 1; j < all_stocks.size(); ++j) {
                double dist = distance_between(&all_stocks[i], &all_stocks[j]);
                if (dist < threshold) {
                    similarity_graph.add_edge(i, j);
                }
            }
        }
    }

    double distance_between(Stock* a, Stock* b) {
        double sum = 0.0;
        sum += diff_sq(a->pe, b->pe);
        sum += diff_sq(a->roe, b->roe);
        sum += diff_sq(a->book_value, b->book_value);
        sum += diff_sq(a->latest_eps, b->latest_eps);
        sum += diff_sq(a->dividend_yield, b->dividend_yield);
        return sqrt(sum);
    }

    double diff_sq(double x, double y) {
        double d = x - y;
        return d * d;
    }
};

#endif

