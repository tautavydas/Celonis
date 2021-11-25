#pragma once

#include <fstream>
#include <iostream>
#include <optional>
#include <shared_mutex>

using namespace std;

class Disk final {
 private:
  string const filename_keys{"Storage_keys"};
  string const filename_values{"Storage_values"};
  shared_mutex mutex;

 public:
  explicit Disk() {
    ofstream stream_keys(filename_keys, ios::app);
    ofstream stream_values(filename_values, ios::app);
  }
  ~Disk() { delAll(); }
  Disk(Disk const&) = delete;
  Disk(Disk &&) noexcept = delete;
  Disk &operator=(Disk const&) = delete;
  Disk &operator=(Disk &&) noexcept = delete;

  void put(string const& key, string const& value) {
    unique_lock<shared_mutex> write_lock(mutex);
    ofstream stream_keys(filename_keys, ios::app);
    ofstream stream_values(filename_values, ios::app);
    if (stream_keys.fail() || stream_values.fail()) {
      throw ios::failure("Error putting data into file");
    } else {
      stream_keys << key << endl;
      stream_values << value << endl;
    }
  }

  optional<string> get(string const& key) {
    shared_lock<shared_mutex> read_lock(mutex);
    ifstream stream_keys(filename_keys);
    ifstream stream_values(filename_values);
    if (stream_keys.fail() || stream_values.fail()) {
      throw ios::failure("Error getting data from file");
    } else {
      string string_key, string_value;
      while (getline(stream_keys, string_key) &&
             getline(stream_values, string_value)) {
        if (string_key == key) {
          return optional<string>{string_value};
        }
      }
    }
    return nullopt;
  }

  bool del(string const& key) {
    unique_lock<shared_mutex> write_lock(mutex);
    bool ret{false};
    ifstream stream_keys(filename_keys), stream_values(filename_values);
    ofstream stream_keys_temp("temp_keys"), stream_values_temp("temp_values");
    if (stream_keys.fail() ||
        stream_values.fail() ||
        stream_keys_temp.fail() ||
        stream_values_temp.fail()) {
      throw ios::failure("Error getting data");
    } else {
      string string_key, string_value;
      while (getline(stream_keys, string_key) &&
             getline(stream_values, string_value)) {
        if (string_key != key) {
          stream_keys_temp << string_key << endl;
          stream_values_temp << string_value << endl;
        } else {
          ret = true;
        }
      }
      stream_keys.close();
      stream_values.close();
      stream_keys_temp.close();
      stream_values_temp.close();
      remove(filename_keys.c_str());
      rename("temp_keys", filename_keys.c_str());
      remove(filename_values.c_str());
      rename("temp_values", filename_values.c_str());
    }
    return ret;
  }

  // used for interactive demonstration
  void delAll() {
    unique_lock<shared_mutex> write_lock(mutex);
    ofstream stream_keys(filename_keys),
             stream_values(filename_values);
    if (stream_keys.fail() || stream_values.fail()) {
      throw ios::failure("Error putting data into file");
    }
  }

  // used for interactive demonstration
  void printAll() {
    shared_lock<shared_mutex> write_lock(mutex);
    ifstream stream_keys(filename_keys),
             stream_values(filename_values);
    if (stream_keys.fail() || stream_values.fail()) {
      throw ios::failure("Error getting data");
    } else {
      string string_key, string_value;
      if (stream_keys.peek() == ifstream::traits_type::eof()) {
        cout << "Disk is empty" << endl;
      } else {
        while (getline(stream_keys, string_key) &&
               getline(stream_values, string_value)) {
          cout << string_key << ":" << string_value << endl;
        }
      }
    }
  }
};
