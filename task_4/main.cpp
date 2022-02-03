#include "test_runner.h"
#include "profile.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

template<typename Type>
using Index = multimap<Type, const Record *>;

struct Data {
    Record record;
    Index<int>::iterator timestamp_iter;
    Index<int>::iterator karma_iter;
    Index<string>::iterator user_iter;
};

class Database {
public:
    bool Put(const Record &record) {
        auto[it, inserted] = storage.insert({record.id, Data{record, {}, {}, {}}});

        if (!inserted) {
            return false;
        }

        auto &data = it->second;
        const Record *ptr = &data.record;
        data.timestamp_iter = timestamp_index.insert({record.timestamp, ptr});
        data.karma_iter = karma_index.insert({record.karma, ptr});
        data.user_iter = user_index.insert({record.user, ptr});

        return true;
    }

    const Record *GetById(const string &id) const {
        auto it = storage.find(id);
        if (it == storage.end()) {
            return nullptr;
        }
        return &it->second.record;
    }

    bool Erase(const string &id) {
        auto it = storage.find(id);
        if (it == storage.end()) {
            return false;
        }

        const auto& data = it->second;
        timestamp_index.erase(data.timestamp_iter);
        karma_index.erase(data.karma_iter);
        user_index.erase(data.user_iter);
        storage.erase(it);
        return true;
    }

    template<typename Callback>
    void RangeByTimestamp(int low, int high, const Callback &callback) const {
        auto it_begin = timestamp_index.lower_bound(low);
        auto it_end = timestamp_index.upper_bound(high);
        for (auto it = it_begin; it != it_end; it++) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

    template<typename Callback>
    void RangeByKarma(int low, int high, const Callback &callback) const {
        auto it_begin = karma_index.lower_bound(low);
        auto it_end = karma_index.upper_bound(high);
        for (auto it = it_begin; it != it_end; it++) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

    template<typename Callback>
    void AllByUser(const string &user, const Callback &callback) const {
        auto[it_begin, it_end] = user_index.equal_range(user);
        for (auto it = it_begin; it != it_end; it++) {
            if (!callback(*it->second)) {
                break;
            }
        }
    }

private:
    unordered_map<string, Data> storage;

    Index<int> timestamp_index;
    Index<int> karma_index;
    Index<string> user_index;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});
    db.Put({"id3", "O>>-<", "general2", 1536107260, 0});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record &record) {
        if (record.id == "id1") {
            return false;
        }
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestRangeTime() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107360, bad_karma});
    db.Put({"id3", "O>>-<", "general2", 1536107460, 0});

    int count = 0;
    db.RangeByTimestamp(1536107259, 1536107460, [&count](const Record &record) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(3, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});
    db.Put({"id3", "Rethink life", "master1", 1536107260, 2000});
    db.Put({"id4", "Rethink life", "maste", 1536107260, 2000});
    db.Put({"id5", "Rethink life", "master", 1536107260, 2000});
    db.Put({"id6", "Rethink life", "aster", 1536107260, 2000});
    db.Put({"id5", "Rethink life", " master ", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(3, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");

    cout << (record != nullptr) << endl;
    cout << (record->title == final_body) << endl;

//    ASSERT(record != nullptr);
//    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    RUN_TEST(tr, TestRangeTime);
    return 0;
}
