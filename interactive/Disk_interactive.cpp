#include <Disk.hpp>

int main(int argc, char* argv[]) {
  string const usage{"h - help\n"
                     "r - record into Disk\n"
                     "p - print from Disk\n"
                     "d - delete from Disk\n"
                     "P - print all Disk\n"
                     "D - delete all Disk\n"
                     "q - quit"};
  Disk disk;

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
          disk.put(input, input2);
        } else if (input == "p") {
          cout << ">> key: ";
          cin >> input;
          optional<string> maybe_value = disk.get(input);
          if (maybe_value.has_value())
            cout << ">> value: " << maybe_value.value() << endl;
          else
            cout << ">> key does not exist in disk" << endl;
        } else if (input == "d") {
          cout << ">> key: ";
          cin >> input;
          if (disk.del(input))
            cout << "key and value deleted successfully" << endl;
          else
            cout << "key does not exist in disk" << endl;
        } else if (input == "P") {
          disk.printAll();
        } else if (input == "D") {
          disk.delAll();
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
