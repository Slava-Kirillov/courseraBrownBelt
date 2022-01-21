#include "test_runner.h"

#include <forward_list>
#include <iterator>
#include <algorithm>

using namespace std;

template<typename Type, typename Hasher>
class HashSet {
public:
    using BucketList = forward_list<Type>;

    explicit HashSet(size_t num_buckets, const Hasher &hasher = {}) : hash_set(num_buckets), hasher_(hasher) {}

    void Add(const Type &value) {
        size_t ind = getIndex(value);

        BucketList& bucketList = hash_set[ind];
        auto it = find(bucketList.begin(), bucketList.end(), value);

        if (it == bucketList.end()) {
            bucketList.push_front(value);
        }
    }

    bool Has(const Type &value) const {
        size_t ind = getIndex(value);

        const BucketList& bucketList = hash_set[ind];
        return find(bucketList.begin(), bucketList.end(), value) != bucketList.end();
    }

    void Erase(const Type &value) {
        size_t ind = getIndex(value);

        BucketList& bucketList = hash_set[ind];
        bucketList.remove(value);
    }

    const BucketList &GetBucket(const Type &value) const {
        size_t ind = getIndex(value);

        return hash_set[ind];
    };

private:
    vector<BucketList> hash_set;
    Hasher hasher_;

    size_t getIndex(const Type & value) const {
        return hasher_(value) % hash_set.size();
    }
};
