#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

using id_pointer = string*;

struct Record {
    string id;
    string title;
    string user;
    int timestamp;
    int karma;

    bool operator<(const Record &record) const {
        return this->id < record.id;
    }

    bool operator==(const Record &record) const {
        return this->id == record.id &&
               this->title == record.title &&
               this->user == record.user &&
               this->timestamp == record.timestamp &&
               this->karma == record.karma;
    }
};

class Database {
public:
    bool Put(const Record &record) {
        if (id_map.find(record.id) != id_map.end()) {
            return false;
        }

        id_map[record.id] = &record;
        time_map.insert(make_pair(record.timestamp, &record.id));
        karma_map.insert(make_pair(record.karma, &record.id));
        user_map.insert(make_pair(record.user, &record.id));

        return true;
    }

    const Record *GetById(const string &id) const {
        auto it = id_map.find(id);
        if (it != id_map.end()) {
            return it->second;
        }
        return nullptr;
    }

    bool Erase(const string &id) {
        auto it = id_map.find(id);
        if (it != id_map.end()) {
            auto time_it = time_map.find(it->second->timestamp);
            while (*time_it->second != it->first) {
                time_it++;
            }

            auto karma_it = karma_map.find(it->second->karma);
            while (*karma_it->second != it->first) {
                karma_it++;
            }

            auto user_it = user_map.find(it->second->user);
            while (*user_it->second != it->first) {
                user_it++;
            }

            time_map.erase(time_it);
            karma_map.erase(karma_it);
            user_map.erase(user_it);

            id_map.erase(it);

            return true;
        }
        return false;
    }

    template<typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const {
        for (int timestamp = low; timestamp <= high; timestamp++) {
            auto it = time_map.find(timestamp);
            while (it != time_map.end() && it->first == timestamp) {
                if (!callback(*id_map.find(*it->second)->second)) {
                    return;
                }
                it++;
            }
        }
    }

    template<typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const {
        for (int karma = low; karma <= high; karma++) {
            auto it = karma_map.find(karma);
            while (it != karma_map.end() && it->first == karma) {
                if (!callback(*id_map.find(*it->second)->second)) {
                    return;
                }
                it++;
            }
        }
    }

    template<typename Callback>
    void AllByUser(const string &user, Callback callback) const {
        auto it = user_map.find(user);
        while (it != user_map.end() && it->first == user) {
            if (!callback(*id_map.find(*it->second)->second)) {
                return;
            }
            it++;
        }
    }

private:
    unordered_map<string, const Record*> id_map;

    multimap<int, const string*> time_map;
    multimap<int, const string*> karma_map;
    multimap<string, const id_pointer> user_map;
};

void TestRangeBoundaries() {
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record &) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser() {
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record &) {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement() {
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
