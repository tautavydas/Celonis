#include <Cache.hpp>

int main(int argc, char* argv[]) {
  string const usage{"h - help\n"
                     "r - record into Cache\n"
                     "p - print from Cache\n"
                     "d - delete from Cache\n"
                     "P - print all Cache\n"
                     "D - delete all Cache\n"
                     "s - cache size in bytes\n"
                     "q - quit"};
  Cache cache;

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
        if (cache.put(input, input2))
          cout << "key and value inserted" << endl;
        else
          cout << "key already exist, value was updated" << endl;
      } else if (input == "p") {
        cout << ">> key: ";
        cin >> input;
        optional<string> maybe_value = cache.get(input);
        if (maybe_value.has_value())
          cout << ">> value: " << maybe_value.value() << endl;
        else
          cout << ">> key does not exist in cache" << endl;
      } else if (input == "d") {
        cout << ">> key: ";
        cin >> input;
        if (cache.del(input))
          cout << "key and value deleted successfully" << endl;
        else
          cout << "key does not exist in cache" << endl;
      } else if (input == "P") {
        cache.printAll();
      } else if (input == "D") {
        cache.delAll();
      } else if (input == "s") {
        cout << "Cache size: " << cache.size() << endl;
      } else if (input == "q") {
        break;
      } else {
        cout << "wrong command" << endl;
      }
    }
  }

  return EXIT_SUCCESS;
}
