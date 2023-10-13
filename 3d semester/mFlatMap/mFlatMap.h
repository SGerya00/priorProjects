#ifndef FLATMAP_INTEREST_MFLATMAP_H
#define FLATMAP_INTEREST_MFLATMAP_H

#include <iostream>
#include <cstdlib>
#include <algorithm>

template <class VALUE, class KEY>

class mFlatMap {
public:
    mFlatMap() :
            _size_of_arr(_default_size_of_arr), _occupied(0)
    {
        _data_arr = new VALUE[_size_of_arr];
        _keys_synced_arr = new KEY[_size_of_arr];
    }
    //default ctor

    explicit mFlatMap(size_t size) :
            _size_of_arr(size), _occupied(0)
    {
        _data_arr = new VALUE[_size_of_arr];
        _keys_synced_arr = new KEY[_size_of_arr];
    }
    //size-specific ctor

    mFlatMap(const mFlatMap& f_m) :
            _size_of_arr(f_m._size_of_arr), _occupied(f_m._occupied)
    {
        _data_arr = new VALUE[_size_of_arr];
        _keys_synced_arr = new KEY[_size_of_arr];

        std::copy_n(f_m._data_arr, _size_of_arr, _data_arr);
        std::copy_n(f_m._keys_synced_arr, _size_of_arr, _keys_synced_arr);
    }
    //copy ctor

    mFlatMap(mFlatMap&& f_m) noexcept:
            _size_of_arr(f_m._size_of_arr), _occupied(f_m._occupied)
    {
        std::swap(_data_arr, f_m._data_arr);
        std::swap(_keys_synced_arr, f_m._keys_synced_arr);
    }
    //move ctor

    ~mFlatMap() {
        delete [] _data_arr;
        delete [] _keys_synced_arr;
    }
    //dtor
    //----------------------------------------------------------------------

    void swap_with(mFlatMap<VALUE, KEY>& f_m2) {
        mFlatMap<VALUE, KEY> temp;
        temp = std::move(f_m2);
        f_m2 = std::move(*this);
        *this = std::move(temp);
    }
    //made with std::move and operator=(&&)

    mFlatMap<VALUE, KEY>& operator=(const mFlatMap<VALUE, KEY>& b) {
        if (this == &b) {
            return *this;
        }
        delete [] _data_arr;
        delete [] _keys_synced_arr;
        _size_of_arr = b._size_of_arr;
        _occupied = b._occupied;
        _data_arr = new VALUE[_size_of_arr];
        _keys_synced_arr = new KEY[_size_of_arr];

        std::copy_n(b._data_arr, _size_of_arr, _data_arr);
        std::copy_n(b._keys_synced_arr, _size_of_arr, _keys_synced_arr);

        return *this;
    }
    //copy assignment

    mFlatMap<VALUE, KEY>& operator=(mFlatMap<VALUE, KEY>&& b) noexcept {
        if (this == &b) {
            return *this;
        }
        delete [] _data_arr;
        delete [] _keys_synced_arr;

        _size_of_arr = b._size_of_arr;
        _occupied = b._occupied;

        _keys_synced_arr = nullptr;
        _data_arr = nullptr;

        std::swap(_data_arr, b._data_arr);
        std::swap(_keys_synced_arr, b._keys_synced_arr);

        return *this;
    }
    //move assignment

    void clear_f_m() {
        delete [] _data_arr;
        delete [] _keys_synced_arr;
        _data_arr = new VALUE[_size_of_arr];
        _keys_synced_arr = new KEY[_size_of_arr];
        _occupied = 0;
    }
    //clears both arrays and resets variables

    bool contains(const KEY& key) const {
        std::pair<bool, size_t> answer = contains_recursive_binary(key, 0, _occupied);
        return answer.first;
    }
    //returns false if FM does not contain item matching given key

    bool erase_by_key(const KEY& key) {
        std::pair<bool, size_t> result = contains_recursive_binary(key, 0, _occupied);
        if (!(result.first)) {
            std::cout << "Cannot erase by given key, key is not in FlatMap" << std::endl;
            return false;
        }
        shift_all_one_idx_left(result.second);
        return true;
    }
    //returns false if no matching key, true if successfully erased

    bool insert_by_key(const KEY& key, const VALUE& value) {
        std::pair<bool, size_t> result = insert_by_key_sub_function(key, value);
        return result.first;
    }
    //returns false if unable to insert or if element with matching key already exists

    VALUE& operator[](const KEY& key) {
        std::pair<bool, size_t> result = contains_recursive_binary(key, 0, _occupied);
        if (!(result.first)) {
            VALUE new_default;
            std::pair<bool, size_t> result_2 = insert_by_key_sub_function(key, new_default);
            return _data_arr[result_2.second];
        }
        return _data_arr[result.second];
    }
    //get element with given key. If no such element - default ctor the element
    //no exception (maybe on resize if new cannot allocate enough)

    VALUE& at(const KEY& key) {
        std::pair<bool, size_t> result = contains_recursive_binary(key, 0, _occupied);
        if (!(result.first)) {
            throw std::invalid_argument("No element with matching key");
        }
        return _data_arr[result.second];
    }
    //returns Value& if matching key exists
    //throws exception if no element with given key

    const VALUE& at(const KEY& key) const {
        std::pair<bool, size_t> result = contains_recursive_binary(key, 0, _occupied);
        if (!(result.first)) {
            throw std::invalid_argument("No element with matching key");
        }
        return _data_arr[result.second];
    }
    //returns const Value& if matching key exists
    //throws exception if no element with given key

    size_t size() const {
        return _occupied;
    }

    bool empty() const {
        return (0 == _occupied);
    }

    template <class V, class K>
    friend bool operator==(const mFlatMap<V, K>& a, const mFlatMap<V, K>& b);

    template <class V, class K>
    friend bool operator!=(const mFlatMap<V, K>& a, const mFlatMap<V, K>& b);

private:
    static const size_t _default_size_of_arr = 10;
    static const size_t _expansion_constant = 2;
    size_t _size_of_arr = 0;
    size_t _occupied = 0;
    VALUE *_data_arr = nullptr; //array of Values
    KEY *  _keys_synced_arr = nullptr; //array of KEYs for Values

    std::pair<bool, size_t> contains_recursive_binary(const KEY& key, size_t start_idx, size_t range) const {
        size_t middle_idx = start_idx + (range / 2); //center or right
        size_t new_range = range / 2;

        if (_keys_synced_arr[middle_idx] == key) {
            return {true, middle_idx};
        } else if (1 >= range) {
            return {false, 0};
        } else if (_keys_synced_arr[middle_idx] < key) {
            return contains_recursive_binary(key, start_idx, new_range);
        } else {
            if (1 == (range % 2)) {
                middle_idx += 1;
            }
            return contains_recursive_binary(key, middle_idx, new_range);
        }
    }

    void shift_all_one_idx_left(size_t erased_element_idx) {
        size_t last_element_idx = _occupied - 1;
        size_t n_o_iterations_needed = last_element_idx - erased_element_idx;
        for (size_t i = 0; i < n_o_iterations_needed; i++) {
            size_t current_idx = erased_element_idx + i;
            _data_arr[current_idx] = _data_arr[current_idx + 1]; //class Value specific assignment
            _keys_synced_arr[current_idx] = _keys_synced_arr[current_idx + 1]; //class KEY specific assignment
        }
        VALUE temp_data; //default ctor
        KEY temp_key; //default ctor
        _data_arr[last_element_idx] = temp_data; //class Value specific assignment
        _keys_synced_arr[last_element_idx] = temp_key; //class KEY specific assignment
        //at this moment both temp_data and temp_key are dtor-ed
        _occupied -= 1;
    }

    size_t find_proper_idx_recursive_binary(const KEY& key, size_t start_idx, size_t range) {
        //it is guaranteed that array can hold 1 more element
        size_t middle_idx = start_idx + (range / 2); //center or right
        size_t new_range = range / 2;

        if (_keys_synced_arr[middle_idx] > key) {
            if ((middle_idx + 1) == _occupied) {
                //no array elements after middle_idx
                return (middle_idx + 1);
            }
            if (_keys_synced_arr[middle_idx + 1] <= key) {
                return (middle_idx + 1);
            } else {
                //move right
                if (1 == (range % 2)) {
                    middle_idx += 1;
                }
                return find_proper_idx_recursive_binary(key, middle_idx, new_range);
            }
        } else {
            //move left
            if (0 == new_range) {
                //insertion before first element
                return start_idx;
            }
            return find_proper_idx_recursive_binary(key, start_idx, new_range);
        }
    }

    void shift_all_one_idx_right(size_t start_idx, size_t first_empty_idx) {
        //it is guaranteed that there is at least one free array slot to the right
        size_t iterator_idx = first_empty_idx;

        while(iterator_idx > start_idx) {
            _data_arr[iterator_idx] = _data_arr[iterator_idx - 1];
            _keys_synced_arr[iterator_idx] = _keys_synced_arr[iterator_idx - 1];
            iterator_idx -= 1;
        }
    }

    std::pair<bool, size_t> insert_by_key_sub_function(const KEY& key, const VALUE& value) {
        expand_if_necessary();
        size_t proper_idx = 0;
        if (0 == _occupied) {
            proper_idx = 0;
        } else {
            proper_idx = find_proper_idx_recursive_binary(key, 0, _occupied);
            if ((proper_idx < _occupied) && (_keys_synced_arr[proper_idx] == key)) {
                std::cout << "Element with given key already exists, consider using \"at\" to access data" << std::endl;
                return {false, 0};
            } //to prevent data loss
        }
        shift_all_one_idx_right(proper_idx, _occupied);
        _data_arr[proper_idx] = value;
        _keys_synced_arr[proper_idx] = key;
        _occupied += 1;
        return {true, proper_idx};
    }
    //trying to insert element with a key that already exists is considered unable to insert
    //returns false if unable to insert and informs via std out

    void expand_if_necessary() {
        if (_occupied == _size_of_arr) {
            expand_flatmap();
        }
    }
    //inner function may throw exception

    void expand_flatmap() {
        size_t new_size = _size_of_arr * _expansion_constant;
        VALUE *new_data_arr = new VALUE[new_size];
        KEY *new_keys_synced_arr = new KEY[new_size];

        std::copy_n(_data_arr, _size_of_arr, new_data_arr);
        std::copy_n(_keys_synced_arr, _size_of_arr, new_keys_synced_arr);

        delete[] _data_arr;
        delete[] _keys_synced_arr;

        _data_arr = new_data_arr;
        _keys_synced_arr = new_keys_synced_arr;
        _size_of_arr = new_size;
    }
    //throws exception if insufficient space (new)
};

template <class VALUE, class KEY>
bool operator==(const mFlatMap<VALUE, KEY>& a, const mFlatMap<VALUE, KEY>& b) {
    //first stage
    if (a._occupied != b._occupied) {
        return false;
    }
    //second stage
    for (size_t i = 0; i < a._occupied; i++) {
        if (a._keys_synced_arr[i] != b._keys_synced_arr[i]) {
            return false;
        }
        if (a._data_arr[i] != b._data_arr[i]) {
            return false;
        }
    }
    return true;
}

template <class VALUE, class KEY>
bool operator!=(const mFlatMap<VALUE, KEY>& a, const mFlatMap<VALUE, KEY>& b) {
    return (!(a==b));
}

#endif //FLATMAP_INTEREST_MFLATMAP_H
