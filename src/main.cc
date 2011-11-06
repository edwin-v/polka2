#include <gtkmm/main.h>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);

  Polka::MainWindow window;
  //Shows the window and returns when it is closed.
  Gtk::Main::run(window);

  return 0;
}
