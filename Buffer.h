#ifndef BUFFER_H
#define BUFFER_H

class Buffer {
  public:
    int gap_begin = 0; // la prima posizione
    int gap_end = 9; // l'ultima posizione
    char* buffer;
    void grow();
    int get_gap_size();
    char* get_content();
    /*
     * Il livello di indentazione
     */
    int indent_level = 0;
  public:
    Buffer();
    Buffer(const Buffer& b);
    ~Buffer();
    /*
     * Inserisce un carattere nel buffer
     */
    void put(char c);
    void write(bool w);
    /*
     * Muove il cursore alla posizione <i>pos</i>.
     */
    void cursor_move(int pos);
    /*
     * Sposta il cursore a sinistra
     */
    bool cursor_left();
    /*
     * Sposta il cursore a destra
     */
    bool cursor_right();
    /*
     * Memorizza in <i>text</i> il testo contenuto nel buffer.
     * <i>text</i> deve avere una dimensione sufficiente per
     * poter contenere il testo. Essa può essere richiesta con
     * <i>get_size()</i>.
     */
    void get_text(char* text, int lenght);
    bool delete_char();
    bool backspace();
    /*
     * Ritorna la posizione iniziale del gap del buffer
     * che corrisponde in pratica alla posizione dove
     * si trova attualmente il cursore.
     */
    int get_gap_begin();
    /*
     * Ritorna la posizione finale del gap del buffer.
     */
    int get_gap_end();
    /*
     * Ritorna la lunghezza del testo memorizzato nel buffer
     */
    int get_text_size();
    /*
     * Copia il testo del buffer in un nuovo buffer <i>b</i>
     * a partire dal punto dove si trova il cursore.
     */
    void copy_from_cursor_pos(Buffer* b);
    /*
     * Tronca il testo del buffer a partire dal punto
     * dove si trova il cursore. Rimane cioè solo la
     * prima parte del testo.
     */
    void truncate_line();
    int get_indent_level();
    void set_indent_level(int l);
  char* tmp_get_content(); 
  int tmp_get_indent_level();
    int get_buffer_size() const;
    bool is_empty();
};

#endif
