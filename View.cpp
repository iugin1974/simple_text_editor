#include "View.h"
#include <curses.h>
#include "Controller.h"
#include <vector>
#include <regex>
#include <iostream>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
using namespace std;
#define WIN_STATUS_HEIGHT 2

View::View (Controller* c) {
  controller = c;
}

void View::initScreen() {
  set_escdelay(0);
  initscr();
  noecho();
  cbreak();
  nonl();
  start_color();
  set_tabsize(4);
  use_default_colors();
  init_pair(1, COLOR_MAGENTA, -1);
  init_pair(2, COLOR_WHITE, COLOR_BLUE);
  init_pair(10, COLOR_WHITE, COLOR_BLACK);
  int l = 0;
  int c = 0;
  getmaxyx(stdscr, l, c);
  win_line_number = newwin(l - WIN_STATUS_HEIGHT, 4, 0, 0);
  wbkgd(win_line_number, COLOR_PAIR(1));

  wattron(win_line_number, A_BOLD);
  for (int i = 0; i < l; i++) {
    mvwprintw(win_line_number, i, 0, "%d", i + 1);
    if (i == 0) wattroff(win_line_number, A_BOLD);
  }
  wrefresh(win_line_number);
  win_editor = newwin(l - WIN_STATUS_HEIGHT, c - 4, 0, 4);
  //wbkgd(win_editor, COLOR_PAIR(2));
  keypad(win_editor, true);
  wrefresh(win_editor);

  win_status = newwin(WIN_STATUS_HEIGHT, c, l-WIN_STATUS_HEIGHT, 0);
  //wbkgd(win_status, COLOR_PAIR(10));
  wrefresh(win_status);
}

void View::close_editor() {
  endwin();
}

void View::start_editor() {

  controller->stateChanged(0);
  int return_value = action::NOTHING;
  while (true) {
    if (mode == COMMAND_MODE) return_value = command_mode();
    else if (mode == INSERT_MODE) return_value = insert_mode();

    switch (return_value) {
      case action::EXIT_PROGRAM: return;
      case action::SAVE_FILE: controller->save();
                              break;
      case action::MAKE: make();
                         break;
    }
  }
}

int View::command_mode() {
  wclear(win_status);
  nl();
  cbreak();
  int ch = wgetch(win_status);
  if (ch != ':') {
    quick_command(ch);
    return action::NOTHING;
  }
  int lc = long_command();
  nonl();
  return lc;
}

void View::quick_command(char ch) {
  if (ch == 'i') {
    mode = INSERT_MODE;
    noecho();
    nonl();
  }
}

int View::long_command() {
  mvwprintw(win_status, 0, 0, ":");
  string s;
  int i = 1;
  while (true) {
    int ch = wgetch(win_status);
    if (ch == '\n') break;
    mvwprintw(win_status, 0, i++, "%c", ch);
    s.push_back((char)ch);
  }
  return evaluate_user_input(&s);

}

//TODO: cambia il tutto in uno switch-case
int View::insert_mode() {
  while (true) {
    int ch = wgetch(win_editor);
    //ESC-Taste
    if (ch == 27) {
      mode = COMMAND_MODE;
      return action::NOTHING;
    }
    last_pressed_key = (char)ch; // usato per lo scroll
    if (ch == KEY_LEFT ||
        ch == KEY_RIGHT ||
        ch == KEY_UP ||
        ch == KEY_DOWN) {
      controller->special_key((char)ch);
    }
    else if (ch == 13) /*KEY_ENTER*/ {
      controller->enter();
    }
    else if (ch == KEY_BACKSPACE) {
      controller->backspace();
    }
    else if (ch == KEY_DC) {
      controller->delete_char();
    }
    else if (ch == KEY_NPAGE) {
      int x_dim = 0;
      int y_dim = 0;
      getmaxyx(stdscr, y_dim, x_dim);
      controller->page_down(y_dim);
    }
    else if (ch == KEY_PPAGE) {
      int x_dim = 0;
      int y_dim = 0;
      getmaxyx(stdscr, y_dim, x_dim);
      controller->page_up(y_dim);
    }
    else {
      controller->char_typed((char)ch);
    }
  }
}

void View::clear_line(WINDOW* w, int l) {
  wmove(w,l,0);
  wclrtoeol(w);
}

void View::updateView(vector<Buffer*>* lines, int current_line) {
    int x_dim = 0, y_dim = 0;
    getmaxyx(win_editor, y_dim, x_dim);
    int last_displayed_line = first_displayed_line + y_dim - 1; // -1 perché le righe sono 0-based

    if (current_line < first_displayed_line) { 
        // Se il cursore si sposta sopra la prima riga visibile
        first_displayed_line = max(0, current_line);  
        last_displayed_line = first_displayed_line + y_dim - 1;
        redraw = true;
    } 
    else if (current_line > last_displayed_line) { 
        // Se il cursore va oltre l'ultima riga visibile
        first_displayed_line = min((int)lines->size() - y_dim, current_line - y_dim + 1);
        last_displayed_line = first_displayed_line + y_dim - 1;
        redraw = true;
    }

  
  Buffer* buffer = lines->at(current_line);

  // Disegna solo la parte di testo nello schermo.
  // last_displayed_line indica il last_displayed_lineite fino al quale il testo viene scritto:
  // o la parte tra first_displayed_line e current_line'altezza della finestra
  // o la fine del vector
  if (redraw) {

    wclear(win_line_number);
    wclear(win_editor);
    int last_displayed_line = (first_displayed_line + y_dim < lines->size())? first_displayed_line + y_dim : lines->size();
    for (int i = first_displayed_line; i < last_displayed_line; i++) {
      Buffer* buffer = lines->at(i);
      int indent_level = buffer->get_indent_level();
      int s = buffer->get_text_size();
      char text[s+1];
      buffer->get_text(text, s+1);
      mvwprintw(win_line_number, i - first_displayed_line, 0 , "%d", i + 1);
      mvwprintw(win_editor, i - first_displayed_line, TABSIZE * indent_level, text);
      ///////
      //////
      ///
      //highlight_text(text, i-first_displayed_line);
      ///////
      /////
      ////
		if (i == current_line - 1) wattron(win_line_number, A_BOLD);
			else wattroff(win_line_number, A_BOLD);
      
    }
        wrefresh(win_line_number);
      wrefresh(win_editor);
}
      int cursor_pos_X = buffer->get_gap_begin();
      int tab = buffer->get_indent_level();
      cursor_pos_X = cursor_pos_X + (tab * TABSIZE);
      wmove(win_editor, current_line - first_displayed_line, cursor_pos_X);
}

// calcola il row_gap in base all'altezza dello schermo
int View::get_row_gap(int current_line) {
  int l = 0, c = 0;
  getmaxyx(stdscr, l, c);
  int row_gap = 0;
  if (current_line > (l - 1)) row_gap = current_line - l + 1;
  return row_gap;
}

//TODO -> questo rallenta il tutto
void View::highlight_text(const char* text, int line) {
  string str(text);
  // ci sono //?
  size_t pos = str.find("//");
  if (pos != string::npos) {
    init_pair(4, COLOR_GREEN, -1);
    mvwchgat(win_editor, line, static_cast<int>(pos), -1, A_NORMAL, 4, NULL);
    return;
  }

  pos = str.find("\"");
  if (pos != string::npos) {
    size_t pos2 = str.find("\"", pos+1);
    int ln = -1;
    if (pos2 != string::npos)
      ln = static_cast<int>(pos2 - pos);
    init_pair(5, COLOR_YELLOW, -1);
    mvwchgat(win_editor, line, static_cast<int>(pos), ln, A_NORMAL, 5, NULL);
  }
  // 81 è il numero delle parole chiave
  for (int j = 0; j < 81; j++) {
    regex rx("\\b" + keywords[j] + "\\b");
    auto word_begin = sregex_iterator(str.begin(), str.end(), rx);
    auto word_end = sregex_iterator();
    for (auto it = word_begin; it != word_end; it++) {
      init_pair(3, COLOR_RED, -1);
      mvwchgat(win_editor, line, it->position(), keywords[j].length(), A_NORMAL, 3, NULL);
    }
  }
}
/*
 * Passato un array con dei caratteri per argomento,
 * controlla se vi è una corrsipondenza col char c.
 * Se si ritorna true, altrimenti false.
 */
bool View::match(const char* key, const char* c) {
  int l = strlen(key);
  for (int i = 0; i < l; i++) {
    if (*c == key[i]) return true;
  }
  return false;
}

void View::get_substring(const char* str, char* pos_a, char* pos_b, char* s) {
  int i = 0;
  for (char* j = pos_a; j < pos_b; j++) {
    s[i++] = *j;
  }
  s[i] = '\0';
}

void View::get_user_input(string* s) {
  nl();
  while (true) {
    int ch = wgetch(win_status);
    if (ch == '\n') break;
    s->push_back((char)ch);
  }
  nonl();
}

// ritorna true se il programma termina
int View::evaluate_user_input(string* s) {
  if (s == NULL || *s == "") return action::NOTHING;
  if (*s == "q")  return action::EXIT_PROGRAM;
  if (*s == "w") return action::SAVE_FILE;
  if (*s == "make") return action::MAKE;
  return action::EXIT_PROGRAM;
}

void View::make() {
  pid_t pid = fork();
  if (pid < 0); //ERROR
  /* il figlio */
  else if (pid == 0) {
    def_prog_mode();
    endwin();
    execl("/bin/make", "make", NULL);
    exit(0);
  }
  /* il padre */
  else {
    wait (NULL);
    reset_prog_mode();
    wrefresh(win_editor);
  }

}
