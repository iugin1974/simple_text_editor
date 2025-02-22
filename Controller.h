#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <cstddef>
#include "Observable.h"

using namespace std;

class View;
class Buffer;
class Controller : public Observable {
private:
  char* file_name = NULL;
  vector <Buffer*> lines;
  Buffer* buffer;
  View* view;
  /*
   * Ritorna <i>true</i> se sotto la posizione attuale del
   * cursore ci sono altre linee di testo, altrimenti <i>false</i>.
   */
  bool has_lines_below();
  int current_line = 0;
 int indent_level = 0;
 void load(const char* file_name, vector <Buffer*>* lines);
 void save(const char* file_name, vector <Buffer*>* lines);
 void delete_lines(vector <Buffer*>* lines);
 bool exists(const char* file_name);
 enum Key { K_UP = 3, K_DOWN = 2, K_LEFT = 4, K_RIGHT = 5 };


public:
    ~Controller();
  void addObserver (Observer* o) override;
  void removeObserver (Observer* o) override;
  void stateChanged (int row) override;
  void char_typed (char c);
  void start(Controller* c);
  bool special_key (char c);
  bool key_up();
  bool key_down();
  void page_down(int vertical_screen_size);
  void page_up(int vertical_screen_size);
  void enter();
  void backspace();
  void delete_char();
  void tab_key();
  int get_line_length(int l);
  int get_gap_begin();
  void save();
  void set_file_name(char* fn);
};



#endif
