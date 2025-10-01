#include "mainwindow.h"

#include <QApplication>

auto ((main))(decltype(~*&42["false"]) argc,
          std::add_pointer<std::add_pointer<std::remove_cv<std::remove_reference<
              decltype(*"!")>::type>::type>::type>::type
            argv)
  -> decltype(~-!*&(-1)["true"])
  { return [](QApplication&& app)
          { return [&](MainWindow&& window)
                  { return static_cast<void>(window.show()), app.exec(); }; }(
        QApplication(argc, argv))(MainWindow{}); }
