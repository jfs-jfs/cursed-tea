#include "cursed-tea/application.h"
#include <cursed-tea/core.h>
#include <cursed-tea/ui/table.h>
#include <stdlib.h>

typedef struct Root {
  struct CtModel base;
  struct CursedTable table;
} Root;

#define SAMPLE_COLUMN_COUNT 4
#define SAMPLE_ROW_COUNT 40

static const wchar_t *SAMPLE_COLUMNS[SAMPLE_COLUMN_COUNT] = {
    L"ID", L"Name", L"Role", L"Location"};

static const wchar_t *SAMPLE_ROWS[SAMPLE_ROW_COUNT][SAMPLE_COLUMN_COUNT] = {
    {L"01", L"Ada Lovelace", L"Analyst", L"London"},
    {L"02", L"Alan Turing", L"Cryptographer", L"Bletchley Park"},
    {L"03", L"Grace Hopper", L"Rear Admiral", L"New York"},
    {L"04", L"Dennis Ritchie", L"Language Designer", L"Murray Hill"},
    {L"05", L"Ken Thompson", L"Systems Programmer", L"Murray Hill"},
    {L"06", L"Barbara Liskov", L"Professor", L"Cambridge"},
    {L"07", L"Donald Knuth", L"Author", L"Stanford"},
    {L"08", L"Edsger Dijkstra", L"Theorist", L"Eindhoven"},
    {L"09", L"John McCarthy", L"Lisp Inventor", L"Stanford"},
    {L"10", L"Margaret Hamilton", L"Software Lead", L"Cambridge"},
    {L"11", L"Bjarne Stroustrup", L"Language Designer", L"Aarhus"},
    {L"12", L"Linus Torvalds", L"Kernel Hacker", L"Helsinki"},
    {L"13", L"Katherine Johnson", L"Mathematician", L"Hampton"},
    {L"14", L"Tim Berners-Lee", L"Web Inventor", L"Geneva"},
    {L"15", L"Radia Perlman", L"Network Engineer", L"Boston"},
    {L"16", L"Brian Kernighan", L"Author", L"Princeton"},
    {L"17", L"Frances Allen", L"Compiler Expert", L"Yorktown"},
    {L"18", L"Claude Shannon", L"Information Theorist", L"Gaylord"},
    {L"19", L"John von Neumann", L"Polymath", L"Budapest"},
    {L"20", L"Adele Goldberg", L"Smalltalk Designer", L"Cleveland"},
    {L"21", L"Rob Pike", L"Systems Programmer", L"Toronto"},
    {L"22", L"Guido van Rossum", L"Language Designer", L"Haarlem"},
    {L"23", L"Anita Borg", L"Advocate", L"Chicago"},
    {L"24", L"Niklaus Wirth", L"Language Designer", L"Winterthur"},
    {L"25", L"Jean Bartik", L"ENIAC Programmer", L"Alanthus Grove"},
    {L"26", L"Seymour Cray", L"Supercomputer Architect", L"Chippewa Falls"},
    {L"27", L"Hedy Lamarr", L"Inventor", L"Vienna"},
    {L"28", L"Alonzo Church", L"Logician", L"Washington"},
    {L"29", L"Betty Holberton", L"ENIAC Programmer", L"Philadelphia"},
    {L"30", L"Ivan Sutherland", L"Graphics Pioneer", L"Hastings"},
    {L"31", L"Vint Cerf", L"Internet Architect", L"New Haven"},
    {L"32", L"Lynn Conway", L"VLSI Pioneer", L"Mount Vernon"},
    {L"33", L"Doug Engelbart", L"Mouse Inventor", L"Portland"},
    {L"34", L"Sophie Wilson", L"ARM Designer", L"Leeds"},
    {L"35", L"Gordon Moore", L"Chemist", L"San Francisco"},
    {L"36", L"Mary Allen Wilkes", L"LINC Programmer", L"Chicago"},
    {L"37", L"John Backus", L"Fortran Lead", L"Philadelphia"},
    {L"38", L"Erna Hoover", L"Switching Inventor", L"Irvington"},
    {L"39", L"Steve Wozniak", L"Hardware Hacker", L"San Jose"},
    {L"40", L"The End", L"Last Row", L"Nowhere"},
};

#define TABLE_STYLE_COUNT 7

static const wchar_t *style_name(enum CursedTableStyles style) {
  switch (style) {
  case CT_NORMAL:
    return L"normal";
  case CT_DOUBLE:
    return L"double";
  case CT_BLOCK:
    return L"block";
  case CT_THICK:
    return L"thick";
  case CT_ASCII:
    return L"ascii";
  case CT_MODERN:
    return L"modern";
  case CT_MODERN_THICK:
    return L"modern-thick";
    break;
  }
  return L"?";
}

void render(const void *uncasted_model, struct CtCanvas *canvas) {
  const Root *model = uncasted_model;
  struct CtCanvas inner;
  wchar_t status_line[128];

  ct_cborder(canvas, &inner);
  ct_cwrite_cs(canvas, L"[ TABLE ]");
  swprintf(status_line, 128, L"[ s:style=%ls  j/k:scroll  q:quit ]",
           style_name(model->table.style));
  ct_cwrite_ce(canvas, status_line);

  model->table.base.render(&model->table, &inner);
}

void handle(void *uncasted_model, const struct CtEvent *event) {
  Root *model = uncasted_model;

  if (event->type != KEY_EVENT)
    return;

  switch (event->key) {
  case L'q':
  case L'Q':
    ct_event_send_exit();
    break;
  case L's':
  case L'S':
    model->table.style =
        (enum CursedTableStyles)((model->table.style + 1) % TABLE_STYLE_COUNT);
    break;
  default:
    model->table.base.handler(&model->table, event);
    break;
  }
}

void root_cleanup(void *uncasted_model) {
  Root *root = uncasted_model;
  root->table.base.cleanup(&root->table);
}

void root_setup(struct Root *root) {
  root->base.cleanup = root_cleanup;
  root->base.render = render;
  root->base.handler = handle;

  // A 2D array is not a pointer-to-pointer table: build row pointers.
  static const wchar_t **row_pointers[SAMPLE_ROW_COUNT];
  for (size_t row_index = 0; row_index < SAMPLE_ROW_COUNT; row_index++)
    row_pointers[row_index] = SAMPLE_ROWS[row_index];

  ctu_table_setup(&root->table, CT_MODERN, SAMPLE_COLUMN_COUNT,
                  SAMPLE_ROW_COUNT, SAMPLE_COLUMNS, row_pointers);
}

int main(void) {
  Root root;
  ct_app_init();
  root_setup(&root);
  ct_app_start(&root.base);
  root.table.base.cleanup(&root.table);
  return EXIT_SUCCESS;
}
