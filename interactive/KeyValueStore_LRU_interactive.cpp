#include <KeyValueStore.hpp>

int main(int argc, char* argv[]) {
  string const usage{"h - help\n"
                     "r - record into KeyValueStore\n"
                     "a - access from KeyValueStore\n"
                     "d - delete from KeyValueStore\n"
                     "P - print all KeyValueStore\n"
                     "D - delete all KeyValueStore\n"
                     "q - quit"};
  KeyValueStore<LRU> key_value_store(20);

  try {
    cout << "Usage:" << endl << usage << endl;
    string input, input2;
    cout << ">> ";
    while (true) {
      if (cin.peek() == '\n') {
        cin.ignore();
        cout << ">> ";
      } else {
        cin >> input;
        if (input == "h") {
          cout << usage << endl;
        } else if (input == "r") {
          cout << ">> key: ";
          cin >> input;
          cout << ">> value: ";
          cin >> input2;
          key_value_store.record(input, input2);
        } else if (input == "a") {
          cout << ">> key: ";
          cin >> input;
          optional<string> maybe_value = key_value_store.retrieve(input);
          if (maybe_value.has_value())
            cout << ">> value: " << maybe_value.value() << endl;
          else
            cout << ">> key does not exist in cache nor in disk" << endl;
        } else if (input == "d") {
          cout << ">> key: ";
          cin >> input;
          if (key_value_store.del(input))
            cout << "key and value deleted successfully from cache" << endl;
          else
            cout << "key does not exist in cache" << endl;
        } else if (input == "P") {
          key_value_store.printAll();
        } else if (input == "D") {
          key_value_store.delAll();
        } else if (input == "q") {
          break;
        } else {
          cout << "wrong command" << endl;
        }
      }
    }
  } catch(ios::failure const& e) {
    std::cout << "Exception: " << e.what() << "Code: " << e.code() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
