#include <iostream>
#include <fstream> // for test
#include <string.h>
#include "Buffer.h"
#define GROW_SIZE 10
using namespace std;
//TODO size non funziona più se metto
// indent-level e se uso GROW_SIZE.
// size deve essere determinata dalla
// lunghezza del char array e non così.
//
// buffer deve allora terminare con \0
// e size viene calcolata di volta in volta


// +---+---+---+---+---+---+---+---+---+---+---*
// | A | B | C | D |   |   |   |   |   | E | F |
// +---+---+---+---+---+---+---+---+---+---+---*
//                   b               e

Buffer::Buffer() {

  // GROW_SIZE = la dimensione di cui cresce
  // indent_level = i caratteri iniziali con eventuali \t
  // +1 per il \0 finale
  int new_size = GROW_SIZE + 1;
  buffer = new char[new_size];
  for (int i = 0; i < new_size; i++) {
    buffer[i]='-';
  }
  buffer[new_size - 1] = '\0';
  gap_end = GROW_SIZE - 1;
}

void Buffer::set_indent_level(int l) {
indent_level = l; 
}

Buffer::Buffer(const Buffer& b) {
  this->indent_level = b.indent_level;
  this->gap_begin = b.gap_begin;
  this->gap_end = b.gap_end;
  this->buffer = new char[b.get_buffer_size() + 1];
  // Qui sotto scrivo <= e non solo <
  // perché get_buffer_size() ritorna la lunghezza
  // del buffer senza lo \0 terminatore,
  // che è alla posizione get_buffer_size.
  // Scrivendo <= copio anche lo \0 finale.
  for (int i = 0; i <= b.get_buffer_size(); i++) {
    buffer[i] = b.buffer[i];
  }
}

Buffer::~Buffer() {
  delete[] buffer;
}

// +---+---+---+---+---+---+---+---+---+---+---*
// | A | B | C | D |   |   |   |   |   | E | F |
// +---+---+---+---+---+---+---+---+---+---+---*
//                   b               e

bool Buffer::cursor_left() {
  if (gap_begin == 0) return false;
  //  cout << "Cursor left" << endl;
  gap_begin--;
  // copia il carattere sotto il cursore in fondo al gap
  buffer[gap_end] = buffer[gap_begin];
  // elimina il valore spostato dalla cella di origine
  buffer[gap_begin] = '-';
  gap_end--;
  // write(false);
  return true;
}

bool Buffer::cursor_right() {
  if (gap_end == get_buffer_size() - 1) return false;
  //  cout << "Cursor right" << endl;
  gap_end++;
  // copia il carattere sotto il cursore all'inizio del gap
  buffer[gap_begin] = buffer[gap_end];
  // elimina il valore spostato dalla cella di origine
  buffer[gap_end] = '-';
  gap_begin++;
  // write(false);
  return true;
}

//TODO -> Se il cursore viene mosso a destra
//e il buffer è vuoto a destra (qui per esempio pos. 5)
// abc-------
//il loop gira all'infinito
void Buffer::cursor_move(int pos) {
  if (pos < gap_begin) {
    while (pos < gap_begin) {
      cursor_left();
    }
  }
  else if (pos > gap_begin) {
    while (pos > gap_begin) {
      cursor_right();
    }
  }
}

void Buffer::grow() {
  // cout << "grow()" << endl;
  int old_size = get_buffer_size();
  int new_size = old_size + GROW_SIZE + 1;
  char a[get_buffer_size()];
  a[get_buffer_size()] = '\0';
  // Copy characters of buffer to a[]
  // escluso il \0, alla posizione size() + 1
  // Quindi il terminatore di stringa è eliminato
  for (int i = 0; i < get_buffer_size(); i++) {
    a[i] = buffer[i];
  }

  delete[] buffer;
  buffer = new char[new_size];
  fill(buffer, buffer + new_size, '-');
  buffer[new_size - 1] = '\0';
  //cout << "gap_begin: " << gap_begin <<
  //" gap_end: " << gap_end << 
  //" new_size: " << new_size << endl;

  for (int i = 0; i<gap_begin; i++) {
    buffer[i] = a[i];
  }
  for (int i = gap_end + 1; a[i] != '\0'; i++) {
    buffer[i + GROW_SIZE] = a[i];
  }

  gap_end+=GROW_SIZE;
}

void Buffer::put(char c) {
  if (get_gap_size() == 0) grow();
  buffer[gap_begin] = c;
  gap_begin++;
}

bool Buffer::backspace() {
  if (gap_begin == 0) return false;
  gap_begin--;
  return true;
}

bool Buffer::delete_char() {
  if (gap_end == get_buffer_size()-1) return false;
  gap_end++;
  return true;
}

int Buffer::get_gap_begin() {
  return gap_begin;
}

int Buffer::get_gap_end() {
  return gap_end;
}

void Buffer::copy_from_cursor_pos(Buffer* b) {
  for (int i = gap_end + 1; i < get_buffer_size(); i++) {
    b->put(buffer[i]);
  }
}

void Buffer::truncate_line() {
  gap_end = get_buffer_size() - 1;
}

char* Buffer::get_content() {
  return buffer;
}

char* Buffer::tmp_get_content() {
  return buffer;
}

int Buffer::get_indent_level() {
  return indent_level;
}

// +---+---+---+---+---+---+---+---+---+---+---*
// | A | B | C | D |   |   |   |   |   | E | F |
// +---+---+---+---+---+---+---+---+---+---+---*
//                   b               e


void Buffer::write(bool w) {

  cout << "Size: " << get_buffer_size() << endl;
  // cout << " Text: " << get_text_size();
  // cout << " Gap_begin: " << gap_begin;
  // cout << " Gap_end: " << gap_end << endl;
  if (w) {
    std::cout <<buffer<< std::endl;
    int i = 0;
    while (buffer[i] != '\0') {
      if (i == gap_begin) cout << "b";
      else if (i == gap_end) cout << "e";
      else cout << " ";
      i++;
    }
    cout << endl;
  }
  else {
    for (int i = 0; i < gap_begin; i++)
      cout << buffer[i];
    for (int i = gap_end + 1; i < get_buffer_size(); i++)
      cout << buffer[i];
    cout << endl;
  }
}

int Buffer::get_buffer_size() const {
  return strlen(buffer);    
}

int Buffer::get_gap_size() {
  return  gap_end - gap_begin;  
}

int Buffer::get_text_size() {
  return get_buffer_size() - (gap_end - gap_begin) - 1;
}

bool Buffer::is_empty() {
return get_text_size() == 0;
}

void Buffer::get_text(char* text, int lenght) {
  int j = 0;
  for (int i = 0; i < gap_begin; i++) {
    text[j++] = buffer[i];
  }
  for (int i = gap_end + 1; i < get_buffer_size(); i++) {
    text[j++] = buffer[i];
  }
  text[lenght - 1] = '\0';
}

int Buffer::tmp_get_indent_level() {
  return indent_level;
}
