#ifndef VIEW_H
#define VIEW_H
#include <curses.h>
#include "Observer.h"
#include <string>
#define INSERT_MODE 0
#define COMMAND_MODE 1

class Controller;
class View : public Observer {
  private:
    enum action : int{ NOTHING, EXIT_PROGRAM, SAVE_FILE, MAKE };
    int mode = COMMAND_MODE;
    Controller* controller;
    WINDOW* win_line_number;
    WINDOW* win_editor;
    WINDOW* win_status;
    char last_pressed_key = 0; // per lo scroll
    int first_displayed_line = 0;
    void highlight_text(const char* text, int line);
    int get_row_gap(int current_line);
    bool redraw = true;
    /*
     * Cancella la linea <i>l</i>.
     */
    void clear_line(WINDOW* w, int l);
    std::string keywords[81] = {
      "auto","break","case","continue","default","do","else","enum",
      "extern","for","goto","if","register","return","sizeof","static",
      "struct","switch","typedef","union","volatile","while","NULL",
      "alignas","alignof","and","and_eq","asm","bitand","bitor","class",
      "compl","constexpr","const_cast","deltype","delete","dynamic_cast",
      "explicit","export","false","friend","inline","mutable","namespace",
      "new","noexcept","not","not_eq","nullptr","operator","or","or_eq",
      "private","protected","public","reinterpret_cast","static_assert",
      "static_cast","template","this","thread_local","throw","true","try",
      "typeid","typename","virtual","xor","xor_eq",
      "int","long","double","float","char","unsigned","signed",
      "void","short","auto","const","bool"};
    void get_word_vector(vector<char*>* v, char* text);
    void get_substring(const char* str, char* pos_a, char* pos_b, char* s);
    bool match(const char* key, const char* c);
    char separator[8] = " ,.;:!?";
    void get_user_input(string* s);
    int evaluate_user_input(string* s);
    int command_mode();
    int insert_mode();
    void quick_command(char c);
    void make();
    int long_command();
  public:
    View (Controller* c);
    void updateView (vector<Buffer*>* lines, int l) override;
    /*
     * Inizializza lo schermo con le ncurses.
     */
    void initScreen();
    /*
     * Avvia l'editor vero e prorio
     */
    void start_editor();
    void close_editor();
};

#endif
