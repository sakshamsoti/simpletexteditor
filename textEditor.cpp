#include <ncurses.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class TextEditor {
public:
    TextEditor() {
        // Initialize ncurses
        initscr();
        raw();
        keypad(stdscr, TRUE);
        noecho();
        getmaxyx(stdscr, screen_height, screen_width);
    }

    ~TextEditor() {
        // End ncurses mode
        endwin();
    }

    void openFile(const std::string& filename) {
        std::ifstream file(filename);
        if (file) {
            std::string line;
            while (getline(file, line)) {
                lines.push_back(line);
            }
            file.close();
        } else {
            // If file does not exist, create an empty one
            std::ofstream outfile(filename);
            outfile.close();
        }
        this->filename = filename;
    }

    void saveFile() {
        std::ofstream file(filename);
        for (const auto& line : lines) {
            file << line << std::endl;
        }
        file.close();
    }

    void run() {
        while (true) {
            display();
            int ch = getch();
            switch (ch) {
                case KEY_UP:
                    if (cursor_y > 0) cursor_y--;
                    break;
                case KEY_DOWN:
                    if (cursor_y < lines.size() - 1) cursor_y++;
                    break;
                case KEY_LEFT:
                    if (cursor_x > 0) cursor_x--;
                    break;
                case KEY_RIGHT:
                    if (cursor_x < lines[cursor_y].length()) cursor_x++;
                    break;
                case 10: // Enter key
                    lines.insert(lines.begin() + cursor_y + 1, "");
                    cursor_y++;
                    cursor_x = 0;
                    break;
                case KEY_BACKSPACE:
                case 127: // Delete key
                    if (cursor_x > 0) {
                        lines[cursor_y].erase(cursor_x - 1, 1);
                        cursor_x--;
                    }
                    break;
                case 19: // Ctrl + S to save
                    saveFile();
                    break;
                case 6: // Ctrl + F to find
                    findWord();
                    break;
                case 27: // ESC key to exit
                    return;
                default:
                    if (ch >= 32 && ch <= 126) { // Printable characters
                        lines[cursor_y].insert(cursor_x, 1, ch);
                        cursor_x++;
                    }
                    break;
            }
        }
    }

private:
    std::vector<std::string> lines;  // Stores the content of the text file
    int cursor_x = 0;                // Cursor X position
    int cursor_y = 0;                // Cursor Y position
    int screen_width, screen_height; // Terminal screen dimensions
    std::string filename;            // Name of the file being edited

    void display() {
        clear();
        for (int i = 0; i < lines.size(); i++) {
            mvprintw(i, 0, "%s", lines[i].c_str());
        }
        move(cursor_y, cursor_x);
        refresh();
    }

    void findWord() {
        echo();
        char search_word[256];
        mvprintw(screen_height - 1, 0, "Find: ");
        getstr(search_word);
        noecho();

        std::string word(search_word);
        for (int i = 0; i < lines.size(); i++) {
            size_t pos = lines[i].find(word);
            if (pos != std::string::npos) {
                cursor_y = i;
                cursor_x = pos;
                break;
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./text_editor <filename>" << std::endl;
        return 1;
    }

    TextEditor editor;
    editor.openFile(argv[1]);
    editor.run();

    return 0;
}