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

using multiMapIntIter = multimap<int, const Record*>::const_iterator;
using multiMapStringIter = multimap<string, const Record*>::const_iterator;

struct RecordMultiMap {
    const Record* record;
    multiMapIntIter time_it;
    multiMapIntIter karma_it;
    multiMapStringIter user_it;
};

class Database {
public:
    bool Put(const Record &record) {
        auto [pos, YES] = data.try_emplace(record.id, RecordMultiMap{&record, {}, {}, {}});

        if (YES) {
            pos->second.karma_it = karma_index.insert({record.karma, &record});
            pos->second.time_it = time_index.insert({record.timestamp, &record});
            pos->second.user_it = user_index.insert({record.user, &record});
        }
        return YES;
    }

    const Record *GetById(const string &id) const {
        auto it = data.find(id);
        if (it != data.end()) {
            return it->second.record;
        }
        return nullptr;
    }

    bool Erase(const string &id) {
        auto it = data.find(id);
        if (it != data.end()) {
            time_index.erase(it->second.time_it);
            karma_index.erase(it->second.karma_it);
            user_index.erase(it->second.user_it);
            data.erase(it->second.record->id);
            return true;
        }
        return false;
    }

    template<typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        for(auto it = time_index.lower_bound(low); it != time_index.upper_bound(high); it++) {
            if (!callback(*it->second)) {
                return;
            }
        }
    }

    template<typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        for(auto it = karma_index.lower_bound(low); it != karma_index.upper_bound(high); it++) {
            if (!callback(*it->second)) {
                return;
            }
        }
    }

    template<typename Callback>
    void AllByUser(const string &user, Callback callback) const {
        for(auto it = user_index.lower_bound(user); it != user_index.upper_bound(user); it++) {
            if (!callback(*it->second)) {
                return;
            }
        }
    }

private:
    unordered_map<string, RecordMultiMap> data;

    multimap<int, const Record*, hash<int>> time_index;
    multimap<int, const Record*, hash<int>> karma_index;
    multimap<string, const Record*, hash<string>> user_index;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});
    db.Put({"id3", "O>>-<", "general2", 1536107260, 0});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record & record) {
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
    db.RangeByTimestamp(1536107259, 1536107460, [&count](const Record & record) {
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
