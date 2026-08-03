#include "cursed-tea/application.h"
#include <stdbool.h>
#include <stdlib.h>

#include <cursed-tea/ui/labeled-input.h>

int main() {

  struct CursedLabeledInput root;

  ct_app_init();
  ctu_labeled_input_setup(&root, L"USUARI", false, 25, 30, 0x1337);
  root.focus = true;
  ct_app_start(&root.base);

  return EXIT_SUCCESS;
}
