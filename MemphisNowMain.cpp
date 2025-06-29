/*
 * This file is part of MemphisNow.
 *
 * MemphisNow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MemphisNow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MemphisNow.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MPSMainWindow.hpp"
#include <QApplication>
//#include <QDesktopWidget>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    MPSMainWindow window;
    window.adjustSize();
    //window.move(QApplication::desktop()->screen()->rect().center() - window.rect().center());
    window.show();

    return app.exec();
}
