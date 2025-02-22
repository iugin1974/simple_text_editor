/*
 * Ho affidato la gestione delle coordinate della View alla classe Controller.
 * La classe Controller ha un vector di Buffer, e sa in quale indice del vettore ci troviamo.
 * Inoltre chiede alla classe Buffer, dove si trova il cursore (che altro non è che gap_begin).
 * Così l'indice del vettore e la posizione del cursore indicano le coordinate x e y del cursore della View.
 *
 * In questo modo elimino molti controlli: per esempio premendo i tasti freccia che il cursore nella View non
 * esca dal campo (meno di 0 o più lungo della linea).
 * Inoltre posiziono il cursore automaticamente senza dover anche nella View tenerne traccia.
 */
#include "Controller.h"
#include "View.h"
#include <vector>
#include <iostream> //for test
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {

  Controller c;
  c.set_file_name(argv[1]);
  c.start(&c);
  return 0;
}

Controller::~Controller() {
  delete_lines(&lines);
}

void Controller::set_file_name(char* fn) {
  file_name = fn;
}

void Controller::start(Controller* c) {
  buffer = new Buffer();
  lines.push_back(buffer);
  if (exists(file_name)) {
    load(file_name, &lines);
    // Sposta il cursore a 0 sulla prima linea
    // in modo che venga visualizzato correttamente nell'editor
    lines.at(0)->cursor_move(0);
  }
  View v(c);
  view = &v;
  addObserver(view);
  view->initScreen();
  view->start_editor();
  view->close_editor();

}

void Controller::char_typed(char c) {
  if (c == '{') indent_level++;
  else if (c == '}' && indent_level > 0) indent_level--;

  buffer->put(c);
  stateChanged(current_line);
}

bool Controller::special_key(char c) {
  bool b = false;
  if (c == K_LEFT) b = buffer->cursor_left();
  else if (c == K_RIGHT) b = buffer->cursor_right();
  else if (c == K_UP) b = key_up();
  else if (c == K_DOWN) b = key_down();
  stateChanged(current_line);
  return b;
}

bool Controller::key_up() {
  if (current_line == 0) return false;
  int cursor_pos = buffer->get_gap_begin();
  current_line--;
  buffer = lines.at(current_line);
  if (cursor_pos >= buffer->get_text_size())
    cursor_pos = buffer->get_text_size();
  buffer->cursor_move(cursor_pos);
  return true;
}

//TODO unisci le due funzioni passando come
//argomento +1 o -1 o KEY_UP e KEY_DOWN
bool Controller::key_down() {
  if (current_line == lines.size() - 1) return false;
  int cursor_pos = buffer->get_gap_begin();
  current_line++;
  buffer = lines.at(current_line);
  if (cursor_pos >= buffer->get_text_size())
    cursor_pos = buffer->get_text_size();
  buffer->cursor_move(cursor_pos);
  return true;
}

void Controller::page_down(int vertical_screen_size) {
  current_line += vertical_screen_size;
  if (current_line >= lines.size()) current_line = lines.size()-1;
  stateChanged(current_line);
}

void Controller::page_up(int vertical_screen_size) {
  current_line -= vertical_screen_size;
  if (current_line < 0) current_line = 0;
  stateChanged(current_line);
}

void Controller::enter() {
  // Crea un nuovo Buffer che contiene i caratteri dal
  // punto dove si trova il cursore alla fine della riga
  // (i caratteri che verranno scritti nella riga sotto.
  Buffer* b = new Buffer();
  b->set_indent_level(indent_level);
  buffer->copy_from_cursor_pos(b);
  buffer->truncate_line();
  // Copia il nuovo buffer in un nuovo oggetto Buffer,
  // che conterrà appunto i caratteri della nuova riga.
  buffer = new Buffer(*b);
  delete b;

  if (has_lines_below()) {
    lines.insert(lines.begin()+current_line+1, buffer);
  }
  else {
    lines.push_back(buffer);
  }
  // Disegna la linea corrente, col testo troncato
  current_line++;
  // Disegna la nuova linea
  buffer->cursor_move(0);
  stateChanged(current_line);
}


bool Controller::has_lines_below() {
  return current_line < lines.size() - 1;
}

void Controller::backspace() {
  if (buffer->is_empty() && current_line == 0) return;
  if (buffer->is_empty()) {
delete buffer;
buffer = lines.at(current_line - 1);
lines.erase(lines.begin() + current_line);
current_line--;
stateChanged(current_line);
  } else {
  buffer->backspace();
  stateChanged(current_line);
  }
}

void Controller::delete_char() {
  if (!buffer->is_empty()) {
    buffer->delete_char();
    stateChanged(current_line);
  }
  else if (current_line < lines.size() - 1) {
    // se la linea cancellata non è l'ultima
    // buffer punta al buffer successivo
    // cancella la linea dal vector
      delete buffer;
    buffer = lines.at(current_line + 1);
    buffer->cursor_move(0);
    lines.erase(lines.begin() + current_line);
  stateChanged(current_line);
  }
}

void Controller::stateChanged(int current_line) {
  view->updateView(&lines, current_line);
}

int Controller::get_line_length(int l) {
  Buffer* b = lines.at(l);
  return b->get_text_size();
}

void Controller::addObserver(Observer* o) {

}

void Controller::removeObserver (Observer* o) {

}

void Controller::save() {
  save(file_name, &lines);
}

void Controller::save(const char* file_name, vector <Buffer*>* lines) {
  ofstream out_file;
  out_file.open(file_name);
  for (unsigned int i = 0; i < lines->size(); i++) {
    Buffer* b = lines->at(i);
    char* ch = new char[b->get_text_size() + 1];
    b->get_text(ch, b->get_text_size() + 1);
    out_file << ch;
    // mette i \n solo fino alla penultima riga
    // altrimenti il file viene salvato con
    // una riga in più
    if (i < lines->size() - 1) out_file << endl;
    delete[] ch;
  }
  out_file.close();
}

void Controller::load(const char* file_name, vector <Buffer*>* lines) {
  delete_lines(lines);
  ifstream in_file;
  in_file.open(file_name);
  char c;
  Buffer* b = new Buffer();
  while (in_file.get(c)) {
    if (c == '\n') {
      lines->push_back(b);
      b = new Buffer();
    } else {
      b->put(c);
    }
  }
  lines->push_back(b);
  in_file.close();
}



bool Controller::exists(const char* file_name) {
  ifstream f(file_name);
  return f.good();
}

void Controller::delete_lines(vector <Buffer*>* lines) {
  for (unsigned int i = 0; i < lines->size(); i++) {
    delete lines->at(i);
  }
  lines->clear();
}
